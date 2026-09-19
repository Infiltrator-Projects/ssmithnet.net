// SPDX-License-Identifier: GPL-3.0-or-later
// Shannon Smith's website family: canonical presentation shared by the personal
// site and a byte-verified, pinned snapshot in Package Repository. Common owns
// neutral primitives; this header owns our identity and cross-site navigation.
#pragma once
#include <infiltratr/escape.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace infiltrator_web {
inline std::string esc(const std::string& value) {
    std::size_t required = 0;
    if (!infiltratr_escape_html(value.c_str(), nullptr, 0, &required) || !required)
        throw std::runtime_error("Common HTML escaping measurement failed");
    std::vector<char> buffer(required);
    if (!infiltratr_escape_html(value.c_str(), buffer.data(), buffer.size(), nullptr))
        throw std::runtime_error("Common HTML escaping failed");
    return std::string(buffer.data());
}
inline std::string head(const std::string& title, const std::string& description,
                        const std::string& canonical, const std::string& assets = "assets/") {
    return "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
        "<meta name=\"theme-color\" content=\"#090b0f\"><meta name=\"color-scheme\" content=\"dark\">"
        "<title>" + esc(title) + "</title><meta name=\"description\" content=\"" + esc(description) +
        "\"><link rel=\"canonical\" href=\"" + esc(canonical) + "\">"
        "<meta property=\"og:type\" content=\"website\"><meta property=\"og:title\" content=\"" + esc(title) +
        "\"><meta property=\"og:description\" content=\"" + esc(description) +
        "\"><meta property=\"og:url\" content=\"" + esc(canonical) +
        "\"><link rel=\"stylesheet\" href=\"" + esc(assets) + "infiltrator-web-v1.css?v=common-1.19.3\">"
        "<link rel=\"stylesheet\" href=\"" + esc(assets) + "site.css?v=family-20260919\">"
        "<link rel=\"stylesheet\" href=\"" + esc(assets) + "site-overrides.css?v=family-20260919\">";
}
inline std::string navigation(const std::string& active, const std::string& home = "") {
    std::string html = "<a class=\"skip-link\" href=\"#main\">Skip to content</a><div class=\"shell\">"
        "<nav aria-label=\"Primary\"><a class=\"brand\" href=\"" + esc(home) +
        "index.html\">ssmith<span>net</span>.net</a><div class=\"navlinks\">";
    for (const auto& item : {std::pair<std::string,std::string>{"Home","index.html"},
         {"Workbench","workbench.html"},{"Garage","garage.html"},{"Archive","archive.html"},
         {"Software Centre","https://infiltrator-projects.github.io/Infiltrator-Repository/"}}) {
        const auto href = item.first == "Software Centre" ? item.second : home + item.second;
        html += "<a" + std::string(item.first == active ? " class=\"active\" aria-current=\"page\"" : "") +
                " href=\"" + esc(href) + "\">" + esc(item.first) + "</a>";
    }
    return html + "</div></nav>";
}
inline std::string footer(const std::string& note) {
    return "<footer><div class=\"footerline\"><span>© Shannon Smith · ssmithnet.net</span><span>" +
           esc(note) + "</span></div></footer></div></body></html>";
}
}
