// SPDX-License-Identifier: GPL-3.0-or-later
// Native shared website-family validation. No Python runtime is required.
#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
namespace fs = std::filesystem;
namespace {
using Attrs = std::map<std::string, std::string>;
void require(bool ok, const std::string& what) {
    if (!ok) throw std::runtime_error(what);
}
std::string read(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    require(static_cast<bool>(input), "Missing file: " + path.string());
    return std::string(std::istreambuf_iterator<char>(input), {});
}
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}
Attrs attributes(const std::string& raw) {
    static const std::regex pattern(
        R"attr(([a-zA-Z_:][a-zA-Z0-9_:.-]*)\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+)))attr");
    Attrs result;
    for (auto it = std::sregex_iterator(raw.begin(), raw.end(), pattern);
         it != std::sregex_iterator(); ++it) {
        const auto& match = *it;
        result[lower(match[1].str())] = match[2].matched ? match[2].str()
                                          : match[3].matched ? match[3].str() : match[4].str();
    }
    return result;
}
std::string value(const Attrs& attrs, const std::string& key) {
    const auto it = attrs.find(key);
    return it == attrs.end() ? "" : it->second;
}
struct Page {
    std::vector<std::string> ids, nav, links;
    int h1 = 0, canonical = 0;
    bool og = false, skip = false;
};
Page inspect(const fs::path& path) {
    const std::string html = read(path);
    static const std::regex tags(R"tag(<\s*(/?)\s*([a-zA-Z][a-zA-Z0-9-]*)\b([^<>]*)>)tag");
    Page result;
    bool in_primary_nav = false;
    for (auto it = std::sregex_iterator(html.begin(), html.end(), tags);
         it != std::sregex_iterator(); ++it) {
        const bool closing = (*it)[1].matched && (*it)[1].str() == "/";
        const std::string tag = lower((*it)[2].str());
        if (closing) {
            if (tag == "nav") in_primary_nav = false;
            continue;
        }
        const Attrs attrs = attributes((*it)[3].str());
        if (attrs.count("id")) result.ids.push_back(value(attrs, "id"));
        if (tag == "nav" && value(attrs, "aria-label") == "Primary")
            in_primary_nav = true;
        if (tag == "a") {
            const std::string href = value(attrs, "href");
            if (in_primary_nav) result.nav.push_back(href);
            if (href == "#main") result.skip = true;
        }
        if (tag == "h1") ++result.h1;
        if (tag == "link" && value(attrs, "rel") == "canonical") ++result.canonical;
        if (tag == "meta" && value(attrs, "property") == "og:title") result.og = true;
        for (const char* key : {"href", "src"})
            if (attrs.count(key)) result.links.push_back(value(attrs, key));
    }
    return result;
}
bool external(const std::string& url) {
    if (url.rfind("//", 0) == 0) return true;
    static const std::regex scheme(R"(^[a-zA-Z][a-zA-Z0-9+.-]*:)");
    return std::regex_search(url, scheme);
}
void validate_links(const fs::path& page, const Page& result) {
    for (const std::string& ref : result.links) {
        if (external(ref)) continue;
        const std::string::size_type end = ref.find_first_of("?#");
        const std::string path = ref.substr(0, end);
        if (path.empty()) continue;
        const fs::path target = (path[0] == '/' ? page.root_path() / path.substr(1)
                                                : page.parent_path() / path).lexically_normal();
        require(fs::is_regular_file(target), "Broken local resource: " + page.string() + " -> " + ref);
    }
}
void check(const fs::path& root) {
    const std::array<std::string, 6> possible{
        "index.html", "workbench.html", "garage.html", "about.html",
        "archive.html", "downloads/model-11-sandy/index.html"};
    require(fs::is_regular_file(root / possible[0]), "Missing homepage");
    unsigned checked = 0;
    for (const auto& relative : possible) {
        const fs::path page_path = root / relative;
        if (relative != possible[0] && !fs::is_regular_file(page_path)) continue;
        const Page page = inspect(page_path);
        ++checked;
        require(page.h1 == 1 && page.canonical == 1 && page.og,
                relative + ": h1, canonical or OpenGraph contract failed");
        require(page.skip && std::find(page.ids.begin(), page.ids.end(), "main") != page.ids.end(),
                relative + ": skip link or main landmark missing");
        require(std::set<std::string>(page.ids.begin(), page.ids.end()).size() == page.ids.size(),
                relative + ": duplicate IDs");
        const auto has_about = [](const std::string& ref) {
            return ref == "about.html" || ref == "../../about.html" ||
                   (ref.size() >= 11 && ref.substr(ref.size() - 11) == "/about.html");
        };
        const auto has_archive = [](const std::string& ref) {
            return ref == "archive.html" ||
                   (ref.size() >= 13 && ref.substr(ref.size() - 13) == "/archive.html");
        };
        require(std::any_of(page.nav.begin(), page.nav.end(), has_about),
                relative + ": About missing from primary navigation");
        require(!std::any_of(page.nav.begin(), page.nav.end(), has_archive),
                relative + ": legacy Archive must not be in primary navigation");
        require(page.nav.size() == 5 &&
                std::find(page.nav.begin(), page.nav.end(),
                   "https://infiltrator-projects.github.io/Infiltrator-Repository/") != page.nav.end(),
                relative + ": five-link website family navigation mismatch");
        validate_links(page_path, page);
        const auto content = read(page_path);
        require(content.find("@@HEAD@@") == std::string::npos &&
                content.find("@@APP_CARDS@@") == std::string::npos,
                relative + ": unexpanded template marker");
    }
    const std::string css = read(root / "assets/site.css");
    constexpr std::array<const char*, 3> fonts{
        "mb-corpo-a-cond-regular.woff2",
        "mb-corpo-s-regular.woff2",
        "mb-corpo-s-bold.woff2"};
    for (const char* name : fonts) {
        require(css.find(name) != std::string::npos, std::string("Font absent from CSS: ") + name);
        const std::string bytes = read(root / "assets/fonts" / name);
        require(bytes.size() >= 4 && bytes.substr(0, 4) == "wOF2",
                std::string("Not a genuine WOFF2 font: ") + name);
    }
    size_t faces = 0, offset = 0;
    while ((offset = css.find("@font-face", offset)) != std::string::npos) {
        ++faces;
        offset += 10;
    }
    require(faces == 3, "Expected exactly three authored MB Corpo font faces");
    require(std::regex_search(css, std::regex(R"(--accent\s*:\s*#00adef\b)", std::regex::icase)),
            "Official Mercedes app blue #00ADEF must remain");
    std::cout << "PASS: shared navigation, metadata, landmarks, assets and three real Corpo fonts across "
              << checked << " pages\n";
}
} // namespace
int main(int argc, char** argv) {
    try {
        require(argc <= 2, "Usage: check-web-family [site-root]");
        check(fs::absolute(argc == 2 ? fs::path(argv[1]) : fs::path(".")));
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
