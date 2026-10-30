// SPDX-License-Identifier: GPL-3.0-or-later
// Validate C++ sitegen output, mirrored pages and the complete Garage contract.
// Native C++17 port of the former Python page/generator test.
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
namespace fs = std::filesystem;
namespace {
using std::string;
using Attrs = std::map<string, string>;
const std::array<string, 6> pages{
    "index.html", "workbench.html", "garage.html", "about.html",
    "archive.html", "downloads/model-11-sandy/index.html"
};
void require(bool yes, const string& why) {
    if (!yes) throw std::runtime_error(why);
}
string read(const fs::path& file) {
    std::ifstream stream(file, std::ios::binary);
    require(static_cast<bool>(stream), "File missing: " + file.string());
    return string(std::istreambuf_iterator<char>(stream), {});
}
string trim_ws(const string& s) {
    std::istringstream input(s);
    string token, out;
    while (input >> token) { if (!out.empty()) out += ' '; out += token; }
    return out;
}
string entities(string s) {
    const std::array<std::pair<string, string>, 6> pairs{{
        {"&amp;", "&"}, {"&quot;", "\""}, {"&#39;", "'"},
        {"&apos;", "'"}, {"&lt;", "<"}, {"&gt;", ">"}
    }};
    for (const auto& [from, to] : pairs) {
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != string::npos) {
            s.replace(pos, from.size(), to);
            pos += to.size();
        }
    }
    return s;
}
Attrs attrs(const string& text) {
    static const std::regex expr(
       R"re(([A-Za-z_:][A-Za-z0-9_:.-]*)\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+)))re");
    Attrs values;
    for (auto it = std::sregex_iterator(text.begin(), text.end(), expr);
         it != std::sregex_iterator(); ++it) {
        const auto& m = *it;
        values[m[1].str()] = entities(m[2].matched ? m[2].str()
                              : m[3].matched ? m[3].str() : m[4].str());
    }
    return values;
}
string get(const Attrs& a, const string& key) {
    auto it = a.find(key);
    return it == a.end() ? string() : it->second;
}
std::set<string> split(const string& value) {
    std::istringstream input(value);
    return {std::istream_iterator<string>(input), std::istream_iterator<string>()};
}
struct Tag {
    string tag, between;
    Attrs a;
    bool closing;
    size_t begin, end;
};
std::vector<Tag> tags(const string& html) {
    static const std::regex tag_regex(
        R"tag(<\s*(/?)\s*([A-Za-z][A-Za-z0-9-]*)\b([^<>]*)>)tag");
    std::vector<Tag> result;
    size_t previous = 0;
    for (auto it = std::sregex_iterator(html.begin(), html.end(), tag_regex);
         it != std::sregex_iterator(); ++it) {
        const auto& m = *it;
        const size_t begin = static_cast<size_t>(m.position());
        const size_t end = begin + static_cast<size_t>(m.length());
        result.push_back(Tag{m[2].str(), html.substr(previous, begin - previous),
                             attrs(m[3].str()), m[1].str() == "/", begin, end});
        previous = end;
    }
    return result;
}
struct Page {
    std::vector<string> ids, links;
    unsigned h1 = 0, current = 0, canonical = 0;
};
Page parse_page(const string& html) {
    Page page;
    for (const auto& t : tags(html)) {
        if (t.closing) continue;
        if (t.a.count("id")) page.ids.push_back(get(t.a, "id"));
        if (t.tag == "h1") ++page.h1;
        if (get(t.a, "aria-current") == "page") ++page.current;
        if (t.tag == "link" && get(t.a, "rel") == "canonical") ++page.canonical;
        for (const auto& key : {"href", "src"})
            if (t.a.count(key)) page.links.push_back(get(t.a, key));
    }
    return page;
}
string url_path(const string& url) {
    if (url.rfind("//", 0) == 0 || std::regex_search(
        url, std::regex(R"(^[A-Za-z][A-Za-z0-9+.-]*:)"))) return "";
    const size_t end = url.find_first_of("?#");
    return url.substr(0, end);
}
string fragment(const string& url) {
    const size_t start = url.find('#');
    if (start == string::npos) return "";
    const size_t end = url.find('?', start);
    return url.substr(start + 1, end == string::npos ? string::npos : end - start - 1);
}
void execute_sitegen(const fs::path& generator, const fs::path& output) {
    const pid_t child = fork();
    require(child >= 0, "Could not create child for C++ site generator");
    if (child == 0) {
        execl(generator.c_str(), generator.c_str(), output.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }
    int status = 0;
    require(waitpid(child, &status, 0) == child && WIFEXITED(status) && WEXITSTATUS(status) == 0,
            "Native site generator exited unsuccessfully");
}
struct GarageCard {
    string id;
    std::set<string> classes;
    std::map<string, string> fields;
    std::vector<string> spec_labels, history_labels, equipment;
};
GarageCard parse_card(const string& markup) {
    GarageCard car;
    string list_class, label, capture, buffer;
    auto finish = [&]() {
        const string result = trim_ws(entities(buffer));
        if (capture == "dt") {
            label = result;
            if (list_class == "vehicle-specifications") car.spec_labels.push_back(result);
            if (list_class == "car-facts") car.history_labels.push_back(result);
        } else if (capture == "dd") {
            car.fields[label] = result;
        } else if (capture == "th") {
            car.equipment.push_back(result);
        }
        capture.clear();
        buffer.clear();
    };
    for (const auto& t : tags(markup)) {
        if (!capture.empty()) buffer += t.between;
        if (t.closing) {
            if (t.tag == capture) finish();
            if (t.tag == "dl") list_class.clear();
            continue;
        }
        if (t.tag == "article") {
            car.id = get(t.a, "id");
            car.classes = split(get(t.a, "class"));
        }
        if (t.tag == "dl") list_class = get(t.a, "class");
        if (t.tag == "dt" || t.tag == "dd" ||
            (t.tag == "th" && get(t.a, "scope") == "row")) {
            require(capture.empty(), "Malformed Garage field markup (nested dt/dd/th)");
            capture = t.tag;
        }
    }
    return car;
}
std::vector<GarageCard> read_cards(const string& garage) {
    static const std::regex article(R"re(<article\b[^>]*>[\s\S]*?</article>)re");
    std::vector<GarageCard> found;
    for (auto it = std::sregex_iterator(garage.begin(), garage.end(), article);
         it != std::sregex_iterator(); ++it) found.push_back(parse_card(it->str()));
    return found;
}
void validate_garage(const fs::path& root, const string& garage) {
    const auto cards = read_cards(garage);
    std::vector<GarageCard> vehicles, featured;
    for (const auto& car : cards) {
        if (car.classes.count("garage-vehicle")) vehicles.push_back(car);
        if (car.classes.count("vehicle-card")) featured.push_back(car);
    }
    require(vehicles.size() == 12, "Garage must retain all twelve vehicle records");
    const std::vector<string> history{
        "Make", "Model", "Chassis / Series", "Powertrain"};
    for (size_t i = 1; i < vehicles.size(); ++i) {
        const auto& car = vehicles[i];
        require(car.history_labels == history, "Historical vehicle field labels changed");
        for (const auto& label : history) {
            const auto it = car.fields.find(label);
            require(it != car.fields.end() && !it->second.empty(),
                    "Historical vehicle field missing: " + label);
        }
    }
    require(featured.size() == 2 && featured[0].id == "e250-cdi" &&
            featured[1].id == "500-sec", "2011 E-Class must lead the two Mercedes cards");
    const auto& current = featured[0];
    const auto& sec = featured[1];
    require(current.classes.count("vehicle-card--hero") == 1 &&
            sec.classes.count("vehicle-card--hero") == 0, "Hero card assignment changed");
    const std::vector<string> specs{
        "Chassis / Baumuster", "Engine", "Power / torque", "Transmission",
        "Paint", "Upholstery"};
    require(current.spec_labels == specs && sec.spec_labels == specs,
            "Mercedes factory specification label parity failed");
    const std::array<std::pair<string,string>, 7> current_fields{{
        {"VIN", "WDD2073032F129158"}, {"Original delivery", "Australia"},
        {"Engine number", "651911 30 638336"}, {"Transmission number", "722646 08 520376"},
        {"Release", "13 May 2011"}, {"Delivery", "13 May 2011"},
        {"Registration", "YSF597"}
    }};
    const std::array<std::pair<string,string>, 8> sec_fields{{
        {"VIN", "WDB1260442A171593"}, {"Original delivery", "United Kingdom"},
        {"Engine number", "117963 22 081711"},
        {"Transmission number", "722311 02 661254"},
        {"Production", "Approximately June 1985"},
        {"Order location", "Great Britain"}, {"Registration", "1ARK884"},
        {"Acquired", "17 December 2002"}
    }};
    const auto exact = [&](const GarageCard& car, const string& label, const string& expected) {
        const auto it = car.fields.find(label);
        require(it != car.fields.end() && it->second == expected,
                car.id + ": " + label + " is not " + expected);
    };
    for (const auto& [label,value] : current_fields) exact(current,label,value);
    for (const auto& [label,value] : sec_fields) exact(sec,label,value);
    auto production = current.fields.find("Production");
    require(production != current.fields.end() && production->second.rfind("12 May 2011", 0) == 0,
            "Production date must remain separate from delivery/release date");
    require(current.equipment.size() == 69 &&
            std::set<string>(current.equipment.begin(), current.equipment.end()).size() == 69,
            "E-Class must retain 69 unique equipment codes");
    for (const char* code : {"197U", "251A", "527", "625", "901L", "R11", "VR"})
        require(std::find(current.equipment.begin(), current.equipment.end(), code) != current.equipment.end(),
                string("Missing factory equipment code ") + code);
    require(sec.equipment.empty(), "Do not fabricate SEC equipment codes");
    const size_t id_pos = garage.find("id=\"500-sec\"");
    require(id_pos != string::npos, "SEC record missing");
    const size_t end_pos = garage.find("</article>", id_pos);
    require(end_pos != string::npos, "SEC record not closed");
    const string markup = garage.substr(id_pos, end_pos - id_pos);
    const auto count = [&](const string& part) {
        size_t hits = 0, offset = 0;
        while ((offset = markup.find(part, offset)) != string::npos) {
            ++hits; offset += part.size();
        }
        return hits;
    };
    require(count("<figure>") == 2 && count("alt=\"") == 2,
            "SEC must retain exactly two accessible original photographs");
    const std::array<std::pair<string,string>, 2> photos{{
        {"500sec-amg-side.jpg", "width=\"1021\" height=\"409\""},
        {"500sec-amg-front.jpg", "width=\"660\" height=\"507\""}
    }};
    for (const auto& [filename, dimensions] : photos) {
        require(count("src=\"assets/photos/" + filename + "\"") == 1 &&
                markup.find(dimensions) != string::npos &&
                fs::file_size(root / "assets/photos" / filename) > 0,
                "SEC original photo or dimensions missing: " + filename);
    }
    require(garage.find("Mushroom leather") == string::npos &&
            garage.find("275A") == string::npos,
            "Removed unsupported factory equipment returned");
    size_t technical = 0, from = 0;
    while ((from = garage.find("class=\"vehicle-record technical-detail\"", from)) != string::npos) {
        ++technical; ++from;
    }
    require(technical == 2 && garage.find("<script") == string::npos,
            "Technical record structure or script-free policy changed");
    require(garage.find("href=\"assets/garage.css?v=") != string::npos,
            "Garage stylesheet missing");
    std::cout << "PASS: complete twelve-car Garage history, Mercedes datacards, original SEC photos and 69 unique codes\n";
}
void validate(const fs::path& root) {
    const fs::path generator = root / "build/sitegen";
    require(fs::is_regular_file(generator), "C++ site generator not built: " + generator.string());
    string temp = (fs::temp_directory_path() / "ssmithnet-native-XXXXXX").string();
    std::vector<char> name(temp.begin(), temp.end());
    name.push_back('\0');
    char* directory = mkdtemp(name.data());
    require(directory != nullptr, "Could not create temporary output directory");
    const fs::path work(directory);
    struct Cleaner { fs::path path; ~Cleaner() { std::error_code error; fs::remove_all(path, error); } } cleaner{work};
    const fs::path output = work / "site";
    execute_sitegen(generator, output);
    std::map<string, string> first;
    std::map<string, Page> inspected;
    std::set<string> known(pages.begin(), pages.end());
    for (const auto& relative : pages) {
        const string generated = read(output / relative);
        require(generated == read(root / relative),
                relative + ": committed generated mirror is stale; regenerate with make");
        first[relative] = generated;
        const Page page = parse_page(generated);
        inspected[relative] = page;
        require(page.h1 == 1, relative + ": expected exactly one h1");
        require(page.current == (relative.rfind("downloads/", 0) == 0 ? 0u : 1u),
                relative + ": expected exactly one active page link outside downloads");
        require(page.canonical == 1, relative + ": expected exactly one canonical link");
        require(std::set<string>(page.ids.begin(), page.ids.end()).size() == page.ids.size(),
                relative + ": duplicate IDs");
    }
    for (const auto& [relative, page] : inspected) {
        for (const string& link : page.links) {
            if (link.rfind("//", 0) == 0 ||
                std::regex_search(link, std::regex(R"(^[A-Za-z][A-Za-z0-9+.-]*:)"))) continue;
            const string path = url_path(link);
            const fs::path local = (fs::path(relative).parent_path() / path).lexically_normal();
            const string target = path.empty() ? relative : local.generic_string();
            require(!target.empty() && target.rfind("../", 0) != 0,
                    relative + ": local resource escapes site: " + link);
            const fs::path target_file = known.count(target) ? output / target : root / target;
            require(fs::is_regular_file(target_file),
                    relative + ": broken resource " + link);
            const string anchor = fragment(link);
            if (!anchor.empty()) {
                require(inspected.count(target) &&
                        std::find(inspected.at(target).ids.begin(),
                                  inspected.at(target).ids.end(), anchor) != inspected.at(target).ids.end(),
                        relative + ": broken fragment " + link);
            }
        }
    }
    execute_sitegen(generator, output);
    for (const auto& relative : pages)
        require(first.at(relative) == read(output / relative),
                relative + ": site generator changed between identical runs");
    require(first.at("archive.html") == first.at("about.html"),
            "Legacy Archive must contain full canonical About content");
    require(first.at("archive.html").find("href=\"https://ssmithnet.net/about.html\"") != string::npos,
            "Legacy Archive must point to canonical About URL");
    const string garage = first.at("garage.html");
    validate_garage(root, garage);
    for (const auto& [relative, content] : first)
        if (relative != "garage.html")
            require(content.find("assets/garage.css") == string::npos,
                    relative + ": Garage-only CSS leaked into another page");
    std::cout << "PASS: deterministic native output, committed mirrors, page landmarks, assets and local links\n";
}
} // namespace
int main(int argc, char** argv) {
    try {
        require(argc == 2, "Usage: check-site <repository-root>");
        validate(fs::absolute(argv[1]));
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
