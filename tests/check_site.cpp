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
const std::array<string, 7> pages{
    "index.html", "workbench.html", "projects.html", "garage.html", "about.html",
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
    // All historical cars must have a discoverable jump link without changing their order.
    const auto jump_start = garage.find("class=\"garage-jump\"");
    const auto jump_end = garage.find("</nav>", jump_start);
    require(jump_start != string::npos && jump_end != string::npos,
            "Garage history needs a vehicle index");
    const string jump_markup = garage.substr(jump_start, jump_end - jump_start);
    for (const char* id : {"ba-ltd", "ba-fairmont", "discovery-2", "au-fairmont",
                           "el-falcon", "peugeot-405-gld", "500-sec", "xf-s-pack",
                           "450sel-69", "xc-falcon-ute-1", "xc-falcon-ute-2",
                           "280-se", "lc-torana"}) {
        require(jump_markup.find("href=\"#" + string(id) + "\"") != string::npos,
                string("Garage jump index missing ") + id);
    }
    const auto cards = read_cards(garage);
    std::vector<GarageCard> vehicles, featured;
    for (const auto& car : cards) {
        if (car.classes.count("garage-vehicle")) vehicles.push_back(car);
        if (car.classes.count("vehicle-card")) featured.push_back(car);
    }
    require(vehicles.size() == 14, "Garage must retain all fourteen vehicle records");
    // Full manufacture-year ordering where documented, with remaining vehicles
    // grouped by visibly labelled model era. Keep chronological order across makes.
    const std::array<string,14> chronology{{
        "e250-cdi","ba-ltd","ba-fairmont","discovery-2","au-fairmont",
        "el-falcon","peugeot-405-gld","500-sec","xf-s-pack",
        "450sel-69","xc-falcon-ute-1","xc-falcon-ute-2","280-se","lc-torana"
    }};
    size_t previous_position = 0;
    for (size_t i = 0; i < chronology.size(); ++i) {
        const auto position = garage.find("id=\"" + chronology[i] + "\"");
        require(position != string::npos && (i == 0 || position > previous_position),
                "Garage chronology changed at " + chronology[i]);
        previous_position = position;
    }
    for (const char* id : {"xc-falcon-ute-1", "xc-falcon-ute-2"}) {
        const size_t start = garage.find("id=\"" + string(id) + "\"");
        require(start != string::npos, "Missing separate XC Falcon ute entry");
        const auto end = garage.find("</article>", start);
        require(end != string::npos, "Unclosed XC Falcon ute entry");
        const string card = garage.substr(start, end - start);
        require(card.find("blue") != string::npos &&
                card.find("Six-cylinder") != string::npos &&
                card.find("1976–1979 · model era") != string::npos,
                "XC Falcon ute must retain colour, engine and non-invented year label");
    }
    for (const char* id : {"discovery-2", "au-fairmont"}) {
        const size_t start = garage.find("id=\"" + string(id) + "\"");
        require(start != string::npos, "Missing 2002 vehicle");
        const auto end = garage.find("</article>", start);
        const string card = garage.substr(start, end - start);
        require(card.find("2002 · recorded year") != string::npos,
                "Discovery II and AU Fairmont must reflect owner's 2002 correction");
    }
    require(garage.find("id=\"vehicle-history\"") != string::npos &&
            garage.find("model generation rather than a verified manufacture year") != string::npos,
            "Chronological register or model-era disclosure missing");
    require(garage.find("class=\"year-marker\"") != string::npos &&
            garage.find("class=\"garage-photo-link\"") != string::npos,
            "Historical year markers or full-size photograph links missing");

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
    require(featured.size() == 4 && featured[0].id == "e250-cdi" &&
            featured[1].id == "500-sec" && featured[2].id == "450sel-69" &&
            featured[3].id == "280-se",
            "Mercedes featured cards must remain in E 250, 500SEC, 450SEL 6.9, 280SE order");
    require(featured[1].fields.at("Chassis / Series") == "126.044 · C126" &&
            featured[1].fields.at("Powertrain") == "4,973 cc M117.963 V8" &&
            featured[2].fields.at("Chassis / Series") == "116.036 · W116" &&
            featured[2].fields.at("Powertrain") == "6,834 cc M100.985 V8" &&
            featured[3].fields.at("Chassis / Series") == "116.024 · W116" &&
            featured[3].fields.at("Powertrain") == "2,746 cc M110 I6",
            "Mercedes chassis and powertrain formatting must remain consistent");
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
    const size_t e250_pos = garage.find("id=\"e250-cdi\"");
    require(e250_pos != string::npos, "E 250 CDI record missing");
    const size_t e250_end = garage.find("</article>", e250_pos);
    require(e250_end != string::npos, "E 250 CDI record not closed");
    const string e250_markup = garage.substr(e250_pos, e250_end - e250_pos);
    const auto e250_count = [&](const string& token) {
        size_t hits = 0, offset = 0;
        while ((offset = e250_markup.find(token, offset)) != string::npos) {
            ++hits; offset += token.size();
        }
        return hits;
    };
    require(e250_count("<figure>") == 2 && e250_count("alt=\"") == 2 &&
            e250_count("class=\"vehicle-photo-pair\"") == 1,
            "E 250 CDI must mirror the SEC two-photo layout");
    const std::array<std::pair<string,string>, 2> e250_photos{{
        {"e250-cdi-three-quarter.webp", "width=\"1021\" height=\"409\""},
        {"e250-cdi-front.webp", "width=\"520\" height=\"400\""}
    }};
    for (const auto& [filename, dimensions] : e250_photos) {
        require(e250_count("src=\"assets/photos/" + filename + "\"") == 1 &&
                e250_markup.find(dimensions) != string::npos &&
                fs::exists(root / "assets/photos" / filename) &&
                fs::file_size(root / "assets/photos" / filename) > 1000,
                "E 250 CDI photo missing, empty or wrong dimensions: " + filename);
    }
    require(e250_count("e250-cdi-grille") == 0, "Unwanted E 250 grille-only photo was reintroduced");
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
    require(count("<figure>") == 2 && count("alt=\"") == 2 &&
            count("class=\"vehicle-photo-pair\"") == 1,
            "SEC must mirror the E 250 two-photo gallery layout");
    const std::array<std::pair<string,string>, 2> photos{{
        {"500sec-amg-side.jpg", "width=\"1021\" height=\"409\""},
        {"500sec-amg-front.jpg", "width=\"520\" height=\"400\""}
    }};
    for (const auto& [filename, dimensions] : photos) {
        require(count("src=\"assets/photos/" + filename + "\"") == 1 &&
                markup.find(dimensions) != string::npos &&
                fs::exists(root / "assets/photos" / filename) &&
                fs::file_size(root / "assets/photos" / filename) > 1000,
                "SEC displayed photo or dimensions missing: " + filename);
    }
    require(fs::is_regular_file(root / "assets/photos/500sec-amg-side.jpg") &&
            fs::is_regular_file(root / "assets/photos/500sec-amg-front.jpg"),
            "Original unmodified SEC photographic sources must remain available");
    const size_t el_pos = garage.find("id=\"el-falcon\"");
    require(el_pos != string::npos, "EL Falcon entry missing");
    const size_t el_end = garage.find("</article>", el_pos);
    require(el_end != string::npos, "EL Falcon article not closed");
    const string el_markup = garage.substr(el_pos, el_end - el_pos);
    require(el_markup.find("Tickford") != string::npos &&
            el_markup.find("taxi yellow") != string::npos &&
            el_markup.find("privately owned") != string::npos &&
            el_markup.find("neither is a photograph of my actual car") != string::npos,
            "EL Falcon colour, provenance, or original LPG description missing");
    for (const auto& [filename, dimensions] :
         std::array<std::pair<string, string>, 2>{{
            {"el-falcon-three-quarter.webp", "width=\"1021\" height=\"409\""},
            {"el-falcon-front.webp", "width=\"520\" height=\"400\""}
         }}) {
        require(el_markup.find("src=\"assets/photos/" + filename + "\"") != string::npos &&
                el_markup.find(dimensions) != string::npos &&
                fs::is_regular_file(root / "assets/photos" / filename) &&
                fs::file_size(root / "assets/photos" / filename) > 1000,
                "EL Falcon standardised gallery photo missing: " + filename);
    }
    require(el_markup.find("three-quarter side view") != string::npos &&
            el_markup.find("front view") != string::npos,
            "EL Falcon gallery must match the existing two-photo arrangement");
    const size_t xf_pos = garage.find("id=\"xf-s-pack\"");
    require(xf_pos != string::npos, "Beige 1985 XF S-Pack gallery missing");
    const size_t xf_end = garage.find("</article>", xf_pos);
    require(xf_end != string::npos, "XF S-Pack article not closed");
    const string xf_markup = garage.substr(xf_pos, xf_end - xf_pos);
    require(xf_markup.find("1985") != string::npos &&
            xf_markup.find("XF Falcon S-Pack") != string::npos &&
            xf_markup.find("neither shows the actual car I owned") != string::npos,
            "XF S-Pack year, model or illustrative-image disclosure missing");
    for (const auto& [filename, dimensions] :
         std::array<std::pair<string, string>, 2>{{
            {"xf-s-pack-three-quarter.webp", "width=\"1021\" height=\"409\""},
            {"xf-s-pack-front.webp", "width=\"520\" height=\"400\""}
         }}) {
        require(xf_markup.find("src=\"assets/photos/" + filename + "\"") != string::npos &&
                xf_markup.find(dimensions) != string::npos &&
                fs::is_regular_file(root / "assets/photos" / filename) &&
                fs::file_size(root / "assets/photos" / filename) > 1000,
                "XF S-Pack standardised photograph missing: " + filename);
    }
    require(xf_markup.find("three-quarter side view") != string::npos &&
            xf_markup.find("front view") != string::npos,
            "XF S-Pack photographs must follow the standard Garage arrangement");

    // The LC Torana must use both supplied views, without cropping away the visor
    // or mislabelling illustrative references as original owner photographs.
    const size_t torana_start = garage.find("id=\"lc-torana\"");
    require(torana_start != string::npos, "LC Torana gallery section missing");
    const size_t torana_end = garage.find("</article>", torana_start);
    require(torana_end != string::npos, "LC Torana gallery section is unclosed");
    const string torana_markup = garage.substr(torana_start, torana_end - torana_start);
    require(torana_markup.find("Reference images supplied for this LC Torana entry.") != string::npos &&
            torana_markup.find("vehicle-photo-pair") != string::npos &&
            torana_markup.find("external windscreen sun visor") != string::npos &&
            torana_markup.find("central Holden grille badge") != string::npos,
            "LC Torana images, attribution or identifying details missing");
    for (const auto& [filename, dimensions] :
         std::array<std::pair<string, string>, 2>{{
            {"lc-torana-three-quarter.webp", "width=\"1021\" height=\"510\""},
            {"lc-torana-front.webp", "width=\"520\" height=\"510\""}
         }}) {
        require(torana_markup.find("src=\"assets/photos/" + filename + "\"") != string::npos &&
                torana_markup.find(dimensions) != string::npos &&
                fs::is_regular_file(root / "assets/photos" / filename) &&
                fs::file_size(root / "assets/photos" / filename) > 1000,
                "LC Torana gallery image missing or dimensions wrong: " + filename);
    }

    // The supplied Discovery II and Peugeot 405 GLD images must be committed
    // and rendered in the same side/front format as the other garage entries.
    const std::array<std::array<string, 4>, 2> supplied_galleries{{
        { "discovery-2", "Discovery 2 TDI", "discovery-2-three-quarter.webp", "discovery-2-front.webp" },
        { "peugeot-405-gld", "405 GLD", "peugeot-405-gld-three-quarter.webp", "peugeot-405-gld-front.webp" }
    }};
    for (const auto& gallery : supplied_galleries) {
        const string id = gallery[0];
        const size_t pos = garage.find("id=\"" + id + "\"");
        require(pos != string::npos, "Supplied gallery is missing: " + id);
        const size_t end = garage.find("</article>", pos);
        require(end != string::npos, "Supplied gallery article not closed: " + id);
        const string markup = garage.substr(pos, end - pos);
        require(markup.find("<h3>" + gallery[1] + "</h3>") != string::npos &&
                markup.find("vehicle-reconstruction-note") != string::npos,
                "Supplied gallery vehicle identity or attribution missing: " + id);
        for (size_t i = 2; i < 4; ++i) {
            const string filename = gallery[i];
            require(markup.find("src=\"assets/photos/" + filename + "\"") != string::npos &&
                    fs::is_regular_file(root / "assets/photos" / filename) &&
                    fs::file_size(root / "assets/photos" / filename) > 1000,
                    "Supplied gallery image missing: " + filename);
        }
        require(markup.find("width=\"642\" height=\"348\"") != string::npos &&
                markup.find("width=\"642\" height=\"387\"") != string::npos,
                "Supplied gallery photo dimensions must match supplied images: " + id);
    }
    require(garage.find("504 Diesel") == string::npos &&
            garage.find("white Peugeot 405 GLD diesel with a black interior") != string::npos,
            "Peugeot garage history must identify the white 405 GLD, not a 504");
    // Every referenced photo must exist in the committed source tree.
    // The GitHub Pages source-branch publisher cannot see build-only assets.
    const std::regex gallery_ref(R"photo(src="(assets/photos/[^"]+)")photo");
    for (std::sregex_iterator it(garage.begin(), garage.end(), gallery_ref), last; it != last; ++it) {
        const auto relative = (*it)[1].str();
        require(fs::is_regular_file(root / relative) &&
                fs::file_size(root / relative) > 1000,
                "Garage references a missing published photograph: " + relative);
    }
    const size_t se_start = garage.find("id=\"280-se\"");
    require(se_start != string::npos, "280SE section missing");
    const size_t se_end = garage.find("</article>", se_start);
    require(se_end != string::npos, "280SE section not closed");
    const string se_markup = garage.substr(se_start, se_end - se_start);
    const std::array<std::pair<string, string>, 2> se_photos{{
        {"280se-side-view.webp", "width=\"1021\" height=\"409\""},
        {"280se-front-view.webp", "width=\"660\" height=\"507\""}
    }};
    for (const auto& [filename, dimensions] : se_photos) {
        require(se_markup.find("src=\"assets/photos/" + filename + "\"") != string::npos &&
                se_markup.find(dimensions) != string::npos,
                "280SE gallery image missing or incorrectly sized: " + filename);
    }
    require(se_markup.find("class=\"vehicle-reconstruction-note\"") != string::npos &&
            se_markup.find("Illustrative reconstruction") != string::npos &&
            se_markup.find("Illustrative reconstruction") < se_markup.find("</div><dl class=\"car-facts\""),
            "280SE reconstruction note must appear directly below the description");
    const size_t sel69_start = garage.find("id=\"450sel-69\"");
    require(sel69_start != string::npos, "450SEL 6.9 illustrated entry missing");
    const size_t sel69_end = garage.find("</article>", sel69_start);
    require(sel69_end != string::npos, "450SEL 6.9 entry not closed");
    const string sel69_markup = garage.substr(sel69_start, sel69_end - sel69_start);
    require(sel69_markup.find("vehicle-reconstruction-note") != string::npos &&
            sel69_markup.find("fully optioned") != string::npos,
            "450SEL 6.9 recreation attribution missing");
    const std::array<std::pair<string, string>, 2> sel69_photos{{
        {"450sel-69-side-view.webp", "width=\"1021\" height=\"409\""},
        {"450sel-69-front-view.webp", "width=\"520\" height=\"400\""}
    }};
    for (const auto& [filename, dimensions] : sel69_photos) {
        require(sel69_markup.find("src=\"assets/photos/" + filename + "\"") != string::npos &&
                sel69_markup.find(dimensions) != string::npos,
                "450SEL 6.9 missing gallery image or dimensions: " + filename);
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
    std::cout << "PASS: complete fourteen-car Garage history, Mercedes datacards, standardised SEC photos and 69 unique codes\n";
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
        require(generated.find("site-redesign.css?v=20261116-editorial1") != string::npos, relative + ": refreshed UI stylesheet missing");
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
    // Historical identity and navigation must agree across all published pages.
    for (const auto& [relative, content] : first) {
        require(content.find("Online since 1993.") != string::npos,
                relative + ": shared 1993 founding history missing");
        require(content.find("site-redesign.css?v=20261116-editorial1") != string::npos,
                relative + ": updated mobile presentation was not referenced");
    }
    const string design_doc = read(root / "docs/DESIGN.md");
    require(design_doc.find("Peugeot 405 GLD") != string::npos &&
            design_doc.find("Peugeot 504") == string::npos &&
            design_doc.find("two separate blue six-cylinder XC Falcon utes") != string::npos &&
            design_doc.find("1993") != string::npos,
            "Historical design documentation contradicts recorded vehicle or site identity");
    const string validation_doc = read(root / "docs/VALIDATION.md");
    require(validation_doc.find("fourteen-car history") != string::npos &&
            validation_doc.find("check_site.cpp") != string::npos &&
            validation_doc.find("check_web_family.cpp") != string::npos,
            "Validation document must describe the current native tests and garage");
    const string browser_review = read(root / "tests/browser-review.html");
    require(browser_review.find("href=\"../assets/site.css") != string::npos &&
            browser_review.find("src=\"../index.html") != string::npos &&
            browser_review.find("../downloads/model-11-sandy/index.html") != string::npos,
            "The responsive review shell must load real generated pages from its subdirectory");
    const string responsive_css = read(root / "assets/site-redesign.css");
    require(responsive_css.find("@media(max-width:600px)") != string::npos &&
            responsive_css.find(".home-editorial{padding:28px 0 49px!important") != string::npos &&
            responsive_css.find(".workbench-intro .workbench-directory{grid-template-columns:1fr}") != string::npos &&
            responsive_css.find(".about-structure-grid{grid-template-columns:1fr}") != string::npos,
            "Responsive editorial sections must stack cleanly on phones");
    require(first.at("archive.html") == first.at("about.html"),
            "Legacy Archive must contain full canonical About content");
    require(first.at("archive.html").find("href=\"https://ssmithnet.net/about.html\"") != string::npos,
            "Legacy Archive must point to canonical About URL");
    const string home = first.at("index.html");
    require(home.find("Projects, cars,") != string::npos &&
            home.find("ReVox B77 Mk II") != string::npos &&
            home.find("assets/photos/revox-b77-mkii-dbx224.webp") != string::npos &&
            home.find("2002 Discovery II") != string::npos,
            "Refreshed homepage content missing or contains wrong garage history");
    require(first.at("workbench.html").find("id=\"revox-b77\"") != string::npos,
            "ReVox hardware section is not reachable from the homepage");
    require(fs::is_regular_file(root / "assets/photos/revox-b77-mkii-dbx224.webp") &&
            fs::file_size(root / "assets/photos/revox-b77-mkii-dbx224.webp") > 10000,
            "ReVox illustration is missing from published sources");
    require(home.find("assets/graphics/ui-workbench.svg") != string::npos &&
            home.find("assets/graphics/ui-garage.svg") != string::npos &&
            home.find("assets/graphics/ui-code.svg") != string::npos &&
            home.find("assets/graphics/ui-about.svg") != string::npos,
            "Homepage destination icons missing");
    require(home.find("software, and<br><span>machines.</span>") != string::npos &&
            home.find("class=\"home-editorial\"") != string::npos &&
            home.find("In 1993 this website started") != string::npos &&
            home.find("2016") == string::npos,
            "Homepage visual hierarchy or grounded site history missing");
    for (const char* filename : {"ui-workbench.svg", "ui-garage.svg", "ui-code.svg", "ui-about.svg", "audio-waveform.svg"}) {
        const fs::path asset = root / "assets/graphics" / filename;
        require(fs::is_regular_file(asset) && fs::file_size(asset) > 200,
                string("Homepage visual asset missing: ") + filename);
    }
    require(first.at("workbench.html").find("id=\"revox-b77\"") != string::npos,
            "ReVox workbench section unexpectedly removed");
    for (const auto& [relative, content] : first) {
        require(content.find("aria-label=\"Footer\"") != string::npos &&
                content.find("class=\"footer-brand\"") != string::npos,
                relative + ": revised website footer missing");
    }
    require(home.find("class=\"bench-register\"") == string::npos &&
            home.find("class=\"landing-lower\"") == string::npos &&
            home.find("href=\"projects.html\"") != string::npos &&
            home.find("href=\"workbench.html#revox-b77\"") != string::npos,
            "Homepage must introduce subjects rather than repeat Workbench content");
    const string brand_icon = read(root / "assets/brand-2026.svg");
    require(brand_icon == read(root / "assets/favicon.svg"),
            "Legacy favicon and canonical brand mark must remain identical");
    require(brand_icon.find("viewBox=\"0 0 64 48\"") != string::npos &&
            brand_icon.find("#27dce9") != string::npos &&
            brand_icon.find("twin-slash") != string::npos,
            "Browser tab icon no longer matches the twin-slash brand identity");
    // Consolidation: the redesign is one authored system, not cumulative hotfixes.
    const string system_css = read(root / "assets/site-redesign.css");
    require(system_css.find("one presentation layer for Home") != string::npos &&
            system_css.find("--ui-card:") != string::npos &&
            system_css.find("2026 personal site UI refresh") == string::npos,
            "Site-wide CSS consolidation regressed");
    require(system_css.find(".project-section") != string::npos &&
            system_css.find(".about-interests") != string::npos &&
            system_css.find(".garage-page .garage-entry") != string::npos &&
            system_css.find(".home-editorial") != string::npos &&
            system_css.find(".about-structure-grid") != string::npos,
            "Shared theme must cover Home, Workbench, Garage and About");
    require(system_css.find(".home-page main>section,.workbench-page main>section") != string::npos &&
            system_css.find(".about-page main>section{padding:52px 0}") != string::npos,
            "Global section spacing must not override the Software Centre catalogue");
    const string& garage_page = first.at("garage.html");
    require(garage_page.find("garage.css?v=") < garage_page.find("site-redesign.css?v="),
            "The shared theme must be the final stylesheet on Garage");
    const string branding_css = read(root / "assets/site-redesign.css");
    require(branding_css.find("background:url(\"brand-2026.svg\")") != string::npos,
            "Header and footer must use the browser-tab logo artwork");
    for (const auto& [relative, content] : first) {
        const string icon_path = relative.rfind("downloads/", 0) == 0
            ? "../../assets/brand-2026.svg" : "assets/brand-2026.svg";
        require(content.find("rel=\"icon\" type=\"image/svg+xml\" href=\"" + icon_path + "\"") != string::npos,
                relative + ": old tab favicon unexpectedly retained");
    }
    const string about = first.at("about.html");
    require(about.find("<span class=\"era\">1993</span>") != string::npos &&
            about.find("It began with a Mercedes-Benz 450SEL 6.9 in 1993") != string::npos &&
            about.find("id=\"how-it-fits\"") != string::npos &&
            about.find("c. 1996") == string::npos &&
            about.find("<span class=\"era\">2016</span>") == string::npos,
            "Website's original 1993 starting year must be consistent across About");
    require(home.find("since 1993.") != string::npos &&
            first.at("garage.html").find("which I began in 1993.") != string::npos,
            "Homepage and Garage must preserve the 1993 founding year");
    const string& workbench_page = first.at("workbench.html");
    require(workbench_page.find("class=\"workbench-directory\"") != string::npos &&
            workbench_page.find("aria-label=\"Workbench sections\"") != string::npos &&
            workbench_page.find("href=\"#revox-b77\"") != string::npos,
            "Workbench subject directory missing");
    require(first.at("about.html").find("aria-label=\"About sections\"") != string::npos &&
            first.at("about.html").find("id=\"interests\"") != string::npos &&
            first.at("about.html").find("id=\"principles\"") != string::npos,
            "About reading order or anchors missing");
    require(home.find("href=\"projects.html\"") != string::npos &&
            home.find("class=\"home-editorial\"") != string::npos &&
            home.find("The workshop has grown.") != string::npos,
            "Homepage must link to rather than duplicate Workbench and Projects");
    require(workbench_page.find("id=\"platform\"") != string::npos &&
            workbench_page.find("id=\"creative\"") != string::npos &&
            workbench_page.find("id=\"infiltratorfs\"") != string::npos &&
            workbench_page.find("id=\"software\"") != string::npos &&
            workbench_page.find("id=\"link\"") != string::npos &&
            workbench_page.find("id=\"revox-b77\"") != string::npos,
            "Workbench must explain all six project subjects");
    require(workbench_page.find("integration package") != string::npos &&
            workbench_page.find("0.19</dd>") != string::npos &&
            workbench_page.find("qualified bootable release") != string::npos,
            "Workbench must describe verified development limitations and current format");
    require(about.find("id=\"how-it-fits\"") != string::npos &&
            about.find("Beta Tester of the Month") != string::npos,
            "About must retain substantial history and clarify the site's sections");
    require(home.find("104</strong>") == string::npos,
            "Homepage must not duplicate Projects statistics");
    // Compare every visible repository to the retained GitHub organisation snapshot.
    const string project_html = first.at("projects.html");
    const string catalogue_manifest = read(root / "data/github-portfolio-2026-11-16.tsv");
    std::set<string> repository_names;
    std::map<string, size_t> family_counts;
    std::istringstream inventory(catalogue_manifest);
    string inventory_line;
    while (std::getline(inventory, inventory_line)) {
        if (inventory_line.empty() || inventory_line[0] == '#') continue;
        const auto tab = inventory_line.find('\t');
        require(tab != string::npos, "Invalid repository snapshot row");
        const string category = inventory_line.substr(0, tab);
        const string name = inventory_line.substr(tab + 1);
        require(!name.empty() && std::regex_match(name, std::regex(R"([A-Za-z0-9_.-]+)")),
                "Invalid or unsafe repository name in the snapshot");
        require(repository_names.insert(name).second, "Duplicate GitHub repository " + name);
        ++family_counts[category];
        const string link = "href=\"https://github.com/Infiltrator-Projects/" + name + "\"";
        const string marker = "data-repo-name=\"" + name + "\"";
        require(project_html.find(link) != string::npos && project_html.find(marker) != string::npos,
                "Portfolio omits the actual repository " + name);
        require(project_html.find(marker, project_html.find(marker) + marker.size()) == string::npos,
                "Portfolio repeats repository " + name);
    }
    const std::array<std::pair<string,size_t>,8> expected_families{{
        {"foundations",9}, {"applications",7}, {"diagnostics",6},
        {"creative",3}, {"publishing",2}, {"calendar-modules",31},
        {"clock-modules",23}, {"filesystem-modules",23}
    }};
    require(repository_names.size() == 104 && family_counts.size() == expected_families.size(),
            "Portfolio must contain all 104 public repositories in eight groups");
    for (const auto& [group, count] : expected_families) {
        require(family_counts[group] == count &&
                project_html.find("id=\"group-" + group + "\"") != string::npos,
                "Portfolio family missing or has the wrong count: " + group);
    }
    const std::regex repository_entry(R"(data-repo-name=")");
    const size_t entries = static_cast<size_t>(std::distance(
        std::sregex_iterator(project_html.begin(), project_html.end(), repository_entry),
        std::sregex_iterator()));
    require(entries == 104 && project_html.find("id=\"repository-search\"") != string::npos &&
            project_html.find("id=\"repository-count\"") != string::npos &&
            project_html.find("addEventListener('input'") != string::npos,
            "Full catalogue, search controls or progressive enhancement missing");
    require(home.find("href=\"projects.html\"") != string::npos &&
            first.at("workbench.html").find("href=\"projects.html\"") != string::npos &&
            first.at("workbench.html").find("104 repositories") != string::npos,
            "Project catalogue is not discoverable from both Home and Workbench");
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
