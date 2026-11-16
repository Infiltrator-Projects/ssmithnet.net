#include <algorithm>
#include <filesystem>
#include "../include/site-family.hpp"
#include <cstring>
#include <iostream>
#include <infiltratr/escape.h>
#include <infiltratr/posix.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
using std::string;
using std::vector;

struct Car {
    string tag, title, make, model, series, powertrain, description, anchor, year_label;
    int sort_year;
};

struct VehicleFact {
    string label, value, detail;
};

struct VehicleRecord {
    string id, title, subtitle, year, vin, market, description;
    bool current;
    vector<VehicleFact> specifications, identity, dates, history;
    vector<string> highlights;
    vector<std::pair<string, string>> equipment;
    string note;
};

struct Event {
    string date, title, description;
};

using infiltrator_web::esc;

static void write(const fs::path& p, const string& s) {
    fs::create_directories(p.parent_path());
    const int error = infiltratr_atomic_file_write_bytes(
        p.c_str(), INFILTRATR_ATOMIC_FILE_PRESERVE_PERMISSIONS,
        s.data(), s.size());
    if (error != 0)
        throw std::runtime_error("COMMON atomic write failed for " + p.string() +
                                 ": " + std::strerror(error));
}

static string page_start(const string& title, const string& description, const string& active, const string& body_class = "") {
    const string filename = active == "Home" ? "" : active == "Workbench" ? "workbench.html" : active == "Garage" ? "garage.html" : active == "Download" ? "downloads/model-11-sandy/" : "about.html";
    const string prefix = active == "Download" ? "../../" : "";
    return infiltrator_web::head(title + " — ssmithnet.net", description, "https://ssmithnet.net/" + filename, prefix + "assets/") +
        "<link rel=\"icon\" type=\"image/svg+xml\" href=\"" + esc(prefix + "assets/brand-2026.svg") + "\">" +
        "<link rel=\"stylesheet\" href=\"" + esc(prefix + "assets/site-redesign.css?v=20261116-system1") + "\">" +
        "</head><body class=\"" + esc(body_class) + "\">" + infiltrator_web::navigation(active, prefix) + "<main id=\"main\">";
}

static string page_end(const string& right) {
    return "</main>" + infiltrator_web::footer(right);
}

static string historical_car(const Car& c) {
    const bool is_280se = c.model == "280SE";
    const bool is_69 = c.model == "450SEL 6.9";
    const bool is_xf = c.model == "XF Falcon S-Pack";
    const bool is_el = c.model == "EL Falcon";
    const bool is_torana = c.model == "Torana";
    const bool is_discovery = c.model == "Discovery 2";
    const bool is_peugeot = c.model == "405 GLD";
    std::ostringstream o;
    o << "<article class=\"garage-entry garage-vehicle";
    if (is_280se || is_69) o << " vehicle-card vehicle-card--historic";
    o << "\" id=\"" << esc(c.anchor) << "\"><div class=\"vehicle-heading\">"
      << "<div class=\"tag\">" << esc(c.tag) << "</div><h3>" << esc(c.title)
      << "</h3><p class=\"year-marker\">" << esc(c.year_label)
      << "</p><p class=\"vehicle-description\">" << esc(c.description) << "</p>";
    if (is_280se) o << R"(<p class="vehicle-reconstruction-note">Illustrative reconstruction of my former white 280SE with original-style hubcaps and WA plates; not an original photograph.</p>)";
    if (is_69) o << R"(<p class="vehicle-reconstruction-note">Illustrative reconstruction of my fully optioned, yellow-beige Australian 450SEL 6.9; not an original photograph.</p>)";
    if (is_xf) o << R"(<p class="vehicle-reconstruction-note">Representative photograph of a beige XF S-Pack and a reconstructed head-on view; neither shows the actual car I owned.</p>)";
    if (is_el) o << R"(<p class="vehicle-reconstruction-note">Illustrative reconstructions of a yellow EL Falcon; neither is a photograph of my actual car.</p>)";
    if (is_torana) o << R"(<p class="vehicle-reconstruction-note">Reference images supplied for this LC Torana entry.</p>)";
    if (is_discovery) o << R"(<p class="vehicle-reconstruction-note">Reference images supplied for this Discovery II entry.</p>)";
    if (is_peugeot) o << R"(<p class="vehicle-reconstruction-note">Reference images supplied for this Peugeot 405 GLD entry.</p>)";
    o << "</div><dl class=\"car-facts\">"
      << "<div><dt>Make</dt><dd>" << esc(c.make) << "</dd></div>"
      << "<div><dt>Model</dt><dd>" << esc(c.model) << "</dd></div>"
      << "<div><dt>Chassis / Series</dt><dd>" << esc(c.series) << "</dd></div>"
      << "<div><dt>Powertrain</dt><dd>" << esc(c.powertrain) << "</dd></div>"
      << "</dl>";
    if (is_69) o << R"(<div class="vehicle-photo-pair" aria-label="Reconstructed photographs of the yellow-beige Mercedes-Benz 450SEL 6.9"><figure><a class="garage-photo-link" href="assets/photos/450sel-69-side-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/450sel-69-side-view.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Reconstruction of a fully optioned right-hand-drive W116 450SEL 6.9 in pale ochre yellow-beige, front three-quarter view with original chrome trim and alloy wheels; no exterior front or quarter-panel 6.9 emblems"></a><figcaption>450SEL 6.9 · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/450sel-69-front-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/450sel-69-front-view.webp" width="520" height="400" loading="lazy" decoding="async" alt="Reconstruction of a pale yellow-beige Australian W116 450SEL 6.9 straight on, with right-hand steering, European headlamps and unbadged grille"></a><figcaption>450SEL 6.9 · front view</figcaption></figure></div>)";
    if (is_280se) o << R"(<div class="vehicle-photo-pair" aria-label="Reconstructed images of the white Mercedes-Benz 280SE"><figure><a class="garage-photo-link" href="assets/photos/280se-side-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/280se-side-view.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Reconstructed white W116 Mercedes-Benz 280SE standard-wheelbase saloon in three-quarter side view, with original-style hubcaps and Western Australian number-plate format"></a><figcaption>280SE · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/280se-front-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/280se-front-view.webp" width="660" height="507" loading="lazy" decoding="async" alt="Reconstructed head-on view of the white Mercedes-Benz W116 280SE, showing its chrome grille, European headlights and Western Australian-style registration plate"></a><figcaption>280SE · front view</figcaption></figure></div>)";
    if (is_xf) o << R"(<div class="vehicle-photo-pair" aria-label="Reference images of the beige Ford XF Falcon S-Pack"><figure><a class="garage-photo-link" href="assets/photos/xf-s-pack-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/xf-s-pack-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Beige XF Falcon S-Pack photographed from the front-left three-quarter angle, nose pointing left, showing the body-coloured Ford grille and five-spoke-style alloy wheels"></a><figcaption>XF Falcon S-Pack · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/xf-s-pack-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/xf-s-pack-front.webp" width="520" height="400" loading="lazy" decoding="async" alt="Illustrative head-on view of a beige XF Falcon S-Pack with body-coloured horizontal grille, blue Ford oval, rectangular headlights and black bumper"></a><figcaption>XF Falcon S-Pack · front view</figcaption></figure></div>)";
    if (is_el) o << R"(<div class="vehicle-photo-pair" aria-label="Illustrative views of a Victorian taxi-yellow Ford EL Falcon"><figure><a class="garage-photo-link" href="assets/photos/el-falcon-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/el-falcon-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Illustrative Victorian taxi-yellow EL Falcon four-door sedan from the front-left three-quarter angle, with oval Ford grille and standard wheel covers, without taxi signs"></a><figcaption>EL Falcon · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/el-falcon-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/el-falcon-front.webp" width="520" height="400" loading="lazy" decoding="async" alt="Illustrative head-on view of a Victorian taxi-yellow Ford EL Falcon showing the oval grille, rounded headlights and regular bumper, without taxi markings"></a><figcaption>EL Falcon · front view</figcaption></figure></div>)";
    if (is_torana) o << R"(<div class="vehicle-photo-pair" aria-label="Supplied views of the white Holden LC Torana"><figure><a class="garage-photo-link" href="assets/photos/lc-torana-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/lc-torana-three-quarter.webp" width="1021" height="510" loading="lazy" decoding="async" alt="White Holden LC Torana four-door sedan in a front three-quarter view, with external windscreen sun visor, chrome trim, red interior and a tree-lined parking bay"></a><figcaption>LC Torana · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/lc-torana-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/lc-torana-front.webp" width="520" height="510" loading="lazy" decoding="async" alt="Front view of a white Holden LC Torana with pointed bonnet, central Holden grille badge, external windscreen sun visor, chrome bumper and blurred registration plate"></a><figcaption>LC Torana · front view</figcaption></figure></div>)";
    if (is_discovery) o << R"(<div class="vehicle-photo-pair" aria-label="Supplied views of the Land Rover Discovery II"><figure><a class="garage-photo-link" href="assets/photos/discovery-2-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/discovery-2-three-quarter.webp" width="642" height="348" loading="lazy" decoding="async" alt="Dark Land Rover Discovery II in a front three-quarter view with roof rails, black grille and silver alloy wheels"></a><figcaption>Discovery 2 · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/discovery-2-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/discovery-2-front.webp" width="642" height="387" loading="lazy" decoding="async" alt="Front view of a dark Land Rover Discovery II with rectangular headlights, roof rails and black bumper"></a><figcaption>Discovery 2 · front view</figcaption></figure></div>)";
    if (is_peugeot) o << R"(<div class="vehicle-photo-pair" aria-label="Supplied views of the white Peugeot 405 GLD"><figure><a class="garage-photo-link" href="assets/photos/peugeot-405-gld-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/peugeot-405-gld-three-quarter.webp" width="642" height="348" loading="lazy" decoding="async" alt="White Peugeot 405 GLD four-door saloon in a front three-quarter view, with black side mouldings and grey wheel covers"></a><figcaption>Peugeot 405 GLD · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/peugeot-405-gld-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/peugeot-405-gld-front.webp" width="642" height="387" loading="lazy" decoding="async" alt="Front view of a white Peugeot 405 GLD with rectangular headlamps, black bumper and the Peugeot lion badge"></a><figcaption>Peugeot 405 GLD · front view</figcaption></figure></div>)";
    o << "</article>";
    return o.str();
}

static vector<Car> historical_cars() {
    vector<Car> cars{
        {"2004 · Ford · BA · LTD", "2004 LTD", "Ford", "LTD", "BA", "5.4 L V8 · Petrol / LPG", "A long-wheelbase BA LTD with the 5.4-litre V8 and dual-fuel petrol and LPG. The big, comfortable end of my Australian Ford history.", "ba-ltd", "2004 · recorded year", 2004},
        {"2003 · Ford · BA · Fairmont", "2003 Fairmont", "Ford", "Fairmont", "BA", "5.4 L V8 · Petrol / LPG", "The 2003 BA Fairmont paired a 5.4-litre V8 with petrol and LPG. Another V8 Ford, but in the more familiar Fairmont body.", "ba-fairmont", "2003 · recorded year", 2003},
        {"2002 · Land Rover · Discovery II", "Discovery 2 TDI", "Land Rover", "Discovery 2", "Discovery II", "TDI diesel", "A diesel four-wheel drive among the saloons and coupes. The Discovery II brought a boxy, practical Land Rover into the garage.", "discovery-2", "2002 · recorded year", 2002},
        {"2002 · Ford · AU Series II · Fairmont", "Fairmont", "Ford", "Fairmont", "AU Series II", "4.0 L · Petrol / LPG", "An AU Series II Fairmont with the 4.0-litre six-cylinder engine and dual-fuel petrol and LPG. A different kind of Falcon from the later V8s.", "au-fairmont", "2002 · recorded year", 2002},
        {"Ford · EL · Tickford LPG", "EL Falcon", "Ford", "EL Falcon", "EL", "Petrol / LPG · Tickford", "Victorian taxi yellow paint, but not a taxi: this was my privately owned EL Falcon with Tickford-installed LPG. Only the colour came from the cab rank.", "el-falcon", "1996–1998 · model era", 1997},
        {"Peugeot · 405 · GLD", "405 GLD", "Peugeot", "405 GLD", "405", "Diesel · waste motor oil", "A white Peugeot 405 GLD with black interior. Its most memorable mechanical detail was running the diesel engine on waste motor oil.", "peugeot-405-gld", "Late 1980s–1990s · model era", 1992},
        {"1985 · Mercedes-Benz · C126 · AMG", "500SEC AMG", "Mercedes-Benz", "500SEC", "126.044 · C126", "4,973 cc M117.963 V8", "Smoke Silver over beige leather, right-hand drive and UK delivered. Built at Sindelfingen in 1985 before its AMG conversion, this C126 was a particularly distinctive Mercedes.", "500-sec", "1985 · factory record", 1985},
        {"1985 · Ford · XF · S-Pack", "XF Falcon S-Pack", "Ford", "XF Falcon S-Pack", "XF", "Petrol", "My beige 1985 XF Falcon S-Pack. Square-edged Australian Ford styling and the straightforward character of the XF era.", "xf-s-pack", "1985 · recorded year", 1985},
        {"Mercedes-Benz · W116 · 6.9", "450SEL 6.9", "Mercedes-Benz", "450SEL 6.9", "116.036 · W116", "6,834 cc M100.985 V8", "The fully optioned, yellow-beige Australian 450SEL 6.9 with the M100 V8. This was the car behind the original Infiltrator website, which I began in 1993.", "450sel-69", "1975–1980 · model era", 1978},
        {"Ford · XC · blue ute", "XC Falcon ute (1 of 2)", "Ford", "Falcon ute", "XC", "Six-cylinder", "One of two blue, six-cylinder XC Falcon utes I owned. A utility-bodied Ford alongside the Falcon sedans in my garage history.", "xc-falcon-ute-1", "1976–1979 · model era", 1978},
        {"Ford · XC · blue ute", "XC Falcon ute (2 of 2)", "Ford", "Falcon ute", "XC", "Six-cylinder", "The other blue XC Falcon ute I owned, also with a six-cylinder engine. These were two separate vehicles; their individual build years are not recorded.", "xc-falcon-ute-2", "1976–1979 · model era", 1978},
        {"Mercedes-Benz · W116 · 280SE", "280SE", "Mercedes-Benz", "280SE", "116.024 · W116", "2,746 cc M110 I6", "My white, six-cylinder W116 280SE with traditional hubcaps. A more understated companion to the 450SEL 6.9 in my Mercedes history.", "280-se", "1972–1980 · model era", 1977},
        {"Holden · LC · 1.1 litre", "LC Torana", "Holden", "Torana", "LC", "1.1 L I4", "The little 1.1-litre four-cylinder LC Torana I owned. A compact Holden at the opposite end of the scale from the big V8 sedans.", "lc-torana", "1969–1972 · model era", 1970},
    };
    // Confirmed vehicle years are used where known. Unknown individual build years
    // are positioned by their series period and labelled "model era" in the UI.
    std::stable_sort(cars.begin(), cars.end(),
                     [](const Car& a, const Car& b) { return a.sort_year > b.sort_year; });
    return cars;
}

static string render_home() {
    return page_start("Shannon Smith", "Shannon Smith's personal website: Infiltrator software, a Mercedes-Benz E-Class and a garage history stretching back to the 450SEL 6.9.", "Home", "home-page") + R"site(
<section class="landing-hero" aria-labelledby="home-title">
  <div class="landing-hero-copy">
    <p class="eyebrow">Shannon Smith · Mooroopna, Victoria</p>
    <h1 id="home-title">Projects, cars,<br>software, and<br><span>machines.</span></h1>
    <p class="landing-intro">A personal corner of the internet for building software, driving interesting machinery and keeping the old technology working.</p>
    <div class="landing-actions"><a class="ui-action ui-action-primary" href="workbench.html">Explore the Workbench <span aria-hidden="true">↗</span></a><a class="ui-action ui-action-secondary" href="garage.html">Visit the Garage <span aria-hidden="true">→</span></a></div>
  </div>
  <div class="hero-accent" aria-hidden="true"><span class="hero-wave"></span><span class="hero-wave-copy"></span></div>
</section>
<section class="landing-hubs" aria-label="Explore the site">
  <a class="hub-tile hub-tile-primary" href="workbench.html"><img class="hub-symbol" src="assets/graphics/ui-workbench.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>Workbench</strong><span>Filesystems, native applications, diagnostics and analogue hardware.</span></span><b aria-hidden="true">↗</b></a>
  <a class="hub-tile" href="garage.html"><img class="hub-symbol" src="assets/graphics/ui-garage.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>Garage</strong><span>My current Mercedes, former cars and their stories.</span></span><b aria-hidden="true">↗</b></a>
  <a class="hub-tile" href="https://infiltrator-projects.github.io/Infiltrator-Repository/"><img class="hub-symbol" src="assets/graphics/ui-code.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>Software Centre</strong><span>Downloadable tools, applications and packages.</span></span><b aria-hidden="true">↗</b></a>
  <a class="hub-tile" href="about.html"><img class="hub-symbol" src="assets/graphics/ui-about.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>About</strong><span>The site, its history and the interests behind it.</span></span><b aria-hidden="true">↗</b></a>
</section>
<section class="landing-showcase" aria-label="Featured collections">
  <article class="home-revox">
    <div class="showcase-description">
      <p class="eyebrow">Featured hardware · Open-reel audio</p>
      <h2>ReVox B77 Mk II <span>+ dbx 224</span></h2>
      <p>Magnetic tape, illuminated meters and carefully made mechanics. My B77 Mk II sits with a dbx 224 Type II noise-reduction unit — a very different sort of machine from the ones on my software bench.</p>
      <a class="ui-action ui-action-outline" href="workbench.html#revox-b77">Explore the tape setup <span aria-hidden="true">↗</span></a>
    </div>
    <figure class="revox-preview">
      <img src="assets/photos/revox-b77-mkii-dbx224.webp" width="1200" height="900" decoding="async" alt="Clean illustration of my ReVox B77 Mk II reel-to-reel recorder above a dbx Model 224, with two BASF reels and illuminated VU meters">
      <figcaption>ReVox B77 Mk II and dbx 224 · Redrawn from my own equipment.</figcaption>
    </figure>
  </article>
  <aside class="home-garage-panel" aria-labelledby="home-garage-title">
    <div class="panel-heading"><p class="eyebrow">Garage · Fourteen cars</p><h2 id="home-garage-title">The garage.</h2></div>
    <a class="vehicle-teaser" href="garage.html#e250-cdi"><img src="assets/photos/e250-cdi-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Black Mercedes-Benz E 250 CDI Coupé"><span><strong>2011 E 250 CDI</strong><small>Current car · C207 · OM651</small></span><b aria-hidden="true">→</b></a>
    <a class="vehicle-teaser" href="garage.html#discovery-2"><img src="assets/photos/discovery-2-three-quarter.webp" width="642" height="348" loading="lazy" decoding="async" alt="Dark 2002 Land Rover Discovery II"><span><strong>2002 Discovery II</strong><small>Former car · Diesel 4WD</small></span><b aria-hidden="true">→</b></a>
    <a class="vehicle-teaser" href="garage.html#450sel-69"><img src="assets/photos/450sel-69-side-view.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Yellow-beige Mercedes-Benz 450SEL 6.9 reconstruction"><span><strong>450SEL 6.9</strong><small>The car that started the site</small></span><b aria-hidden="true">→</b></a>
    <a class="panel-link" href="garage.html#vehicle-history">Explore all fourteen cars <span aria-hidden="true">→</span></a>
  </aside>
</section>
<section class="landing-lower" aria-label="Software and site history">
  <article class="home-focus">
    <p class="eyebrow">From the workbench</p>
    <h2>Things I build and keep running.</h2>
    <div class="bench-register">
      <a href="workbench.html#infiltratorfs"><span class="bench-number">01</span><span class="bench-copy"><strong>InfiltratorFS</strong><small>Copy-on-write generations, checked data and recovery work.</small></span><b aria-hidden="true">↗</b></a>
      <a href="workbench.html#link"><span class="bench-number">02</span><span class="bench-copy"><strong>The Link diagnostics family</strong><small>Native vehicle diagnostics developed with my Mercedes.</small></span><b aria-hidden="true">↗</b></a>
      <a href="workbench.html#software"><span class="bench-number">03</span><span class="bench-copy"><strong>Desktop software</strong><small>Native system tools, calendars and interfaces.</small></span><b aria-hidden="true">↗</b></a>
    </div>
    <a class="panel-link" href="workbench.html">Explore the full Workbench <span aria-hidden="true">→</span></a>
  </article>
  <article class="home-history">
    <p class="eyebrow">At a glance</p><h2>Cars, code and history.</h2>
    <p>A personal website I began in 1993 with my Mercedes-Benz 450SEL 6.9, which grew to include software, machinery and older technology.</p>
    <div class="history-facts">
      <div><strong>14</strong><span>Vehicles in the garage history</span></div>
      <div><strong>1993</strong><span>My website began</span></div>
      <div><strong>6.9</strong><span>The original Mercedes-Benz story</span></div>
      <div><strong>C++17</strong><span>Native static website generator</span></div>
    </div>
    <a class="panel-link" href="about.html#the-site">The story of this site <span aria-hidden="true">→</span></a>
  </article>
</section>
)site" + page_end("A personal corner of the internet, since 1993.");
}

static string render_workbench() {
    return page_start("Workbench", "InfiltratorFS, native desktop applications and the Link vehicle-diagnostics family. Software projects by Shannon Smith.", "Workbench", "workbench-page") + R"(
<section class="page-intro"><p class="eyebrow">Workbench</p><h1>Software, machines,<br> first principles.</h1><p class="lead">Filesystems, native applications, vehicle diagnostics and a place for analogue hardware. Personal projects, still being built.</p><nav class="workbench-directory" aria-label="Workbench sections"><a href="#infiltratorfs"><small>01 · Storage</small><strong>InfiltratorFS</strong><span>Filesystem, recovery and integrity</span></a><a href="#software"><small>02 · Applications</small><strong>Desktop software</strong><span>Tools and native interfaces</span></a><a href="#link"><small>03 · Vehicles</small><strong>Link diagnostics</strong><span>Cross-make diagnostic software</span></a><a href="#revox-b77"><small>04 · Audio</small><strong>ReVox B77 Mk II</strong><span>Open-reel analogue machines</span></a></nav></section>
<section id="infiltratorfs" class="project-section"><header class="section-heading"><p class="eyebrow">Storage</p><h2>InfiltratorFS</h2><p class="section-deck">A filesystem with its on-disk format and recovery behaviour at the centre of the design.</p></header>
<div class="project-content"><p>The Linux path is a native kernel driver. The project also includes formatters, inspection and scrub tools, and Windows access through a driverless Explorer bridge. A native Windows filesystem driver remains future work.</p><dl class="inline-facts"><div><dt>Format</dt><dd>0.18</dd></div><div><dt>Writes</dt><dd>Copy-on-write</dd></div><div><dt>Integrity</dt><dd>Checksummed</dd></div></dl><div class="resource-links"><a href="https://github.com/Infiltrator-Projects/InfiltratorFS">Source code ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/QUALIFICATION.md">Test evidence ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/ROADMAP.md">Feature status ↗</a></div>
<details class="technical-detail"><summary>Inside the filesystem <span aria-hidden="true">+</span></summary><div class="engineering-notes"><article><h3>Retain earlier generations</h3><p>Copy-on-write transactions create new state while retaining earlier generations. Checksummed checkpoints provide a way to select a recoverable generation after interruption.</p></article><article><h3>Share data deliberately</h3><p>Snapshots and reflinks share file data. Sparse files and paged extent metadata describe where that data lives without requiring every logical byte to occupy storage.</p></article><article><h3>Verify what comes back</h3><p>CRC64 protects metadata and SHA-256 checks logical file data. Inspection and deep-scrub tools make verification an explicit operation.</p></article></div><dl class="fact-list"><div><dt>Object identity</dt><dd>128-bit identifiers</dd></div><div><dt>Names</dt><dd>UTF-8 components up to 1,023 bytes</dd></div><div><dt>Compression</dt><dd>Native IAC1; LZ4 retained as a non-default representation</dd></div><div><dt>Linux integration</dt><dd>Native VFS driver installed through DKMS</dd></div></dl><h3>The work still ahead</h3><p>Booting a real Linux installation brings mount latency, writeback and metadata costs into focus. Those remain active engineering work. Feature support and passing tests are recorded separately from claims about everyday performance.</p></details></div></section>
<section id="software" class="project-section"><header class="section-heading"><p class="eyebrow">Native applications</p><h2>Desktop software.</h2><p class="section-deck">Tools for understanding and working with the machine.</p></header><div class="project-content"><p>System Monitor, Defragmenter, Calendar and Calculator grew alongside InfiltratorFS as after-hours and weekend projects. They share selected Common primitives, while each owns its domain rules and platform responsibilities.</p><div class="family-register"><a href="https://github.com/Infiltrator-Projects/System-Monitor"><strong>System Monitor</strong><span>Native Linux system management through direct OS and hardware interfaces.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Defragmenter"><strong>Defragmenter</strong><span>Filesystem allocation analysis, defragmentation and recovery on Linux.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calendar"><strong>Calendar</strong><span>Alternative clocks, chronology, calendars and astronomy for Cinnamon.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calculator"><strong>Calculator</strong><span>A shared calculation core with native Linux, Windows and iPhone interfaces.</span><b aria-hidden="true">↗</b></a></div></div></section>
<section id="link" class="project-section"><header class="section-heading"><p class="eyebrow">Vehicle diagnostics</p><h2>The Link family.</h2><p class="section-deck">A shared diagnostic engine. Knowledge specific to each manufacturer.</p></header><div class="project-content"><p>Link owns the common transport, OBD and UDS handling, sessions and scheduling. Mercedes, Jaguar, BMW, Audi and Ford applications add their own identities, definitions and manufacturer-specific behaviour.</p><p>MBLink is developed with <a class="inline-link" href="garage.html">my C207 E-Class</a>.</p><div class="family-register"><a href="https://github.com/Infiltrator-Projects/Link"><strong>Link</strong><span>The shared C11 diagnostic and application engine.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/MBLink"><strong>MBLink</strong><span>Mercedes-Benz · C207 / OM651 development vehicle.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/JAGLink"><strong>JAGLink</strong><span>Jaguar · X-Type / X400 knowledge.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/BMWLink"><strong>BMWLink</strong><span>BMW</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/AUDILink"><strong>AUDILink</strong><span>Audi</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/FORDLink"><strong>FORDLink</strong><span>Ford</span><b aria-hidden="true">↗</b></a></div><details class="technical-detail"><summary>Discover &amp; the diagnostic workflow <span aria-hidden="true">+</span></summary><p>Connect the adapter, identify the vehicle and modules, match supported definitions, then select the data to request. Results stay tied to their source.</p><p>Discover is the specialist side of the family: module identification, read-only inventory, captures and evidence export. Shared interrogation mechanics belong in Link; manufacturer-specific addresses and meanings belong with the vehicle application.</p><p>An unknown response stays unknown until documentation or reproducible captures establish its meaning.</p></details></div></section>

<section id="revox-b77" class="project-section analog-section">
  <header class="section-heading"><p class="eyebrow">Analogue audio</p><h2>ReVox B77 Mk II.</h2><p class="section-deck">Open-reel tape, mechanical engineering and the sound of magnetic recording.</p></header>
  <div class="analog-feature-body">
    <figure><img src="assets/photos/revox-b77-mkii-dbx224.webp" width="1200" height="900" loading="lazy" decoding="async" alt="Illustrated ReVox B77 Mk II recorder with BASF reels, paired with dbx 224 noise reduction"><figcaption>Clean illustrative redraw of my actual equipment, rather than an original photograph.</figcaption></figure>
    <div><p class="eyebrow">My tape equipment</p><h3>B77 Mk II + dbx Model 224</h3><p>My ReVox B77 Mk II reel-to-reel deck is paired with a dbx Model 224 simultaneous encode/decode Type II tape noise-reduction system. The original hardware photographs have been redrawn here to show the machine without the surrounding room.</p><dl class="analog-specs"><div><dt>Recorder</dt><dd>ReVox B77 Mk II</dd></div><div><dt>Noise reduction</dt><dd>dbx Model 224 · Type II</dd></div><div><dt>Format</dt><dd>Open-reel magnetic tape</dd></div></dl></div>
  </div>
</section>
<section id="repository" class="destination-strip"><div><p class="eyebrow">Downloads &amp; installation</p><h2>The Software Centre.</h2><p>Browse the Linux applications, inspect package details and add the APT repository to Linux Mint.</p></div><a class="text-link" href="https://infiltrator-projects.github.io/Infiltrator-Repository/">Browse applications ↗</a></section>
<section id="principles" class="closing-note"><h2>Keep the evidence.<br>Question the result.</h2><p>A successful build proves that code compiled. A clean test proves the case it exercised. Neither automatically proves behaviour on a real car or a real desktop workload. That gap is where much of the work happens.</p></section>
)" + page_end("The best or nothing.");
}

static string vehicle_facts(const vector<VehicleFact>& facts, const string& class_name) {
    std::ostringstream o;
    o << "<dl class=\"" << esc(class_name) << "\">";
    for (const auto& fact : facts) {
        o << "<div><dt>" << esc(fact.label) << "</dt><dd>" << esc(fact.value);
        if (!fact.detail.empty()) o << "<span class=\"vehicle-fact-detail\">" << esc(fact.detail) << "</span>";
        o << "</dd></div>";
    }
    return o.str() + "</dl>";
}

static string vehicle_record(const VehicleRecord& car) {
    std::ostringstream o;
    vector<VehicleFact> identity = {{"VIN", car.vin, ""}, {"Original delivery", car.market, ""}};
    identity.insert(identity.end(), car.identity.begin(), car.identity.end());
    o << "<details class=\"vehicle-record technical-detail\"><summary>Full vehicle record <span aria-hidden=\"true\">+</span></summary>"
      << "<div class=\"vehicle-record-body\">" << vehicle_facts(car.specifications, "vehicle-specifications")
      << "<div class=\"vehicle-record-groups\"><section><h3>Factory identity</h3>";
    o << vehicle_facts(identity, "vehicle-record-facts") << "</section>";
    for (const auto& group : {std::pair<string, const vector<VehicleFact>*>{"Dates & delivery", &car.dates},
                              {"Ownership & modifications", &car.history}}) {
        o << "<section><h3>" << esc(group.first) << "</h3>" << vehicle_facts(*group.second, "vehicle-record-facts") << "</section>";
    }
    o << "</div>";
    if (!car.equipment.empty()) {
        o << "<details class=\"vehicle-equipment technical-detail\"><summary>Factory equipment · " << car.equipment.size()
          << " codes <span aria-hidden=\"true\">+</span></summary><table><caption>" << esc(car.title)
          << " — factory equipment</caption><thead><tr><th scope=\"col\">Code</th><th scope=\"col\">Description</th></tr></thead><tbody>";
        for (const auto& option : car.equipment)
            o << "<tr><th scope=\"row\">" << esc(option.first) << "</th><td>" << esc(option.second) << "</td></tr>";
        o << "</tbody></table></details>";
    }
    if (!car.note.empty()) o << "<p class=\"vehicle-record-note\">" << esc(car.note) << "</p>";
    return o.str() + "</div></details>";
}

static VehicleRecord current_mercedes() {
    return {
        "e250-cdi", "E 250 CDI", "BlueEFFICIENCY Coupé · C207", "2011", "WDD2073032F129158", "Australia",
        "Bremen-built, right-hand-drive pre-facelift E-Class coupé.", true,
        {{"Chassis / Baumuster", "C207 · 207.303", "Right-hand drive"},
         {"Engine", "2,143 cc diesel", "OM651"},
         {"Power / torque", "150 kW · 500 Nm", "204 PS · factory specification"},
         {"Transmission", "5-speed automatic", "722.646"},
         {"Paint", "Obsidian Black", "197U"},
         {"Upholstery", "Black / Anthracite", "Leather · 251A"}},
        {{"Make", "Mercedes-Benz", ""}, {"Model", "E 250 CDI Coupé", ""},
         {"Engine number", "651911 30 638336", ""}, {"Transmission number", "722646 08 520376", ""},
         {"Order number", "0 1 901 08524", ""}, {"Production number", "1789426", ""}},
        // Keep the previously recorded production date distinct from the
        // decoder's release/delivery dates; they describe different events.
        {{"Production", "12 May 2011", "Previously recorded"}, {"Release", "13 May 2011", ""},
         {"Delivery", "13 May 2011", ""}, {"Order location", "Australia Association", ""},
         {"Factory", "Bremen", ""}, {"Emissions", "Euro 5", ""}},
        {{"Registration", "YSF597", ""}},
        {"Panoramic roof", "COMAND navigation", "Heated front seats", "Intelligent Light System"},
        {{"051", "AEJ 20/2"}, {"062", "YOM 10/1 Group 2"}, {"108", "Control code marketing"},
         {"197U", "Obsidian Black"}, {"205B", "Owners manual and service record — English"},
         {"213", "Speed-sensitive power steering / Vario steering"}, {"230", "Parking guidance"},
         {"236", "Adaptive rear interior light"}, {"241", "Electrically adjustable left driver seat with memory"},
         {"250A", "Leather"}, {"251A", "Leather — Black / Anthracite"},
         {"275", "Memory package: driver seat, steering column, mirror"}, {"283B", "Airbag label — English"},
         {"291", "Pelvis airbag"}, {"293", "Sidebags in rear left and right"},
         {"305B", "Head unit language — British English"}, {"310", "Dual cup holder"},
         {"335B", "Instrument cluster / head unit language — English"},
         {"413", "Panoramic sliding sunroof / glass sunroof"}, {"423", "5-speed automatic transmission"},
         {"474", "Diesel particulate filter"}, {"477", "Tyre pressure loss warning"},
         {"518", "Universal Communications Interface (UCI)"}, {"51U", "Black fabric roof interior trim"},
         {"527", "COMAND DVD APS with navigation"}, {"580", "Air conditioner"},
         {"600", "Headlamp cleaning equipment"}, {"608", "Automatic high beam switch (IHC)"},
         {"619", "Cornering illumination"}, {"621", "Headlamp with ILS — left-hand traffic"},
         {"625", "Version for Australia"}, {"668", "Freight protection packaging with tie-down hooks"},
         {"690", "Emergency spare tyre (mini spare)"},
         {"700", "Update of injection system series production configuration"},
         {"736", "Black ash wood trim (2A70)"}, {"801", "AEJ 00/1 R 170 AEJ 99/2"},
         {"810", "Premium sound system"}, {"863", "TV tuner digital to analogue"},
         {"873", "Heated left and right front seats"}, {"901L", "Australia Association"},
         {"919", "Aircon cooling power package"}, {"928", "Exhaust gas cleaning with Euro 5 technology"},
         {"9XXL", "Australia / Pacific"}, {"B12", "Wheel replacement tool"}, {"F207", "Model series 207"},
         {"FC", "Coupé"}, {"G646", "Transmission code version 46"}, {"GA", "Automatic transmission"},
         {"HA", "Rear axle"}, {"K10", "Software control for static curve illumination"},
         {"K11", "Adaptive stop light flashing"}, {"K13", "Service interval control code — 25,000 km"},
         {"M014", "Engine increased performance"}, {"M22", "Displacement 2.2 litre"},
         {"M651", "R4 diesel engine M651"}, {"NA1", "Bremen plant control code"},
         {"NB6", "Bremen plant control code"}, {"P35", "Light package (FFO)"}, {"P64", "Front memory package"},
         {"R", "Right-hand steering"}, {"R01", "Summer tyres"}, {"R11", "No description in supplied record"},
         {"U01", "Rear belt status indication"}, {"U18", "Automatic child seat recognition (AKSE)"},
         {"U60", "Pedestrian protection — active engine bonnet"},
         {"U74", "DVD player — regional code 4, Mexico, South America"},
         {"V56", "Speed + load index 94W"}, {"VL", "Front axle half — left"}, {"VR", "Front axle half — right"}},
        "Factory equipment transcribed from the supplied vehicle datacard."
    };
}

static VehicleRecord historic_mercedes() {
    return {
        "500-sec", "500 SEC AMG", "Factory 500 SEC · C126", "1985", "WDB1260442A171593", "United Kingdom",
        "The UK-delivered, pre-facelift C126 before its AMG conversion.", false,
        {{"Chassis / Baumuster", "C126 · 126.044", "Right-hand drive"},
         {"Engine", "4,973 cc V8", "M117.963"},
         {"Power / torque", "170 kW · 405 Nm", "231 PS · before AMG conversion"},
         {"Transmission", "4-speed automatic", "722.311"},
         {"Paint", "Smoke Silver Metallic", "702U"},
         {"Upholstery", "Beige leather", "275 · datacard description"}},
        {{"Make", "Mercedes-Benz", ""}, {"Model", "500 SEC", ""},
         {"Engine number", "117963 22 081711", ""}, {"Transmission number", "722311 02 661254", ""},
         {"Order number", "0 5 537 55057", ""}},
        {{"Production", "Approximately June 1985", ""}, {"Release", "Not recorded", ""},
         {"Delivery", "Not recorded", ""}, {"Order location", "Great Britain", ""},
         {"Factory", "Sindelfingen", ""}},
        {{"Registration", "1ARK884", ""}, {"Acquired", "17 December 2002", ""},
         {"Conversion", "AMG", "After factory production"}},
        {"UK delivered", "AMG conversion", "Pre-facelift C126", "Sindelfingen"}, {},
        "UK delivery confirmed by Shannon. Factory equipment codes are not included in the supplied SEC record."
    };
}

static string render_garage() {
    std::ostringstream o;
    auto start = page_start("Garage", "The current 2011 Mercedes-Benz E 250 CDI Coupé, its full factory datacard, and my complete vehicle history.", "Garage", "garage-page");
    start.insert(start.find("<link rel=\"stylesheet\" href=\"assets/site-redesign.css"), "<link rel=\"stylesheet\" href=\"assets/garage.css?v=garage-20261106-6point9\">");
    o << start;
    const auto current = current_mercedes();
    const auto sec = historic_mercedes();
    o << R"(<section class="garage-current" aria-labelledby="e250-title"><nav class="section-nav garage-nav" aria-label="Garage sections"><a href="#e250-cdi">Current car</a><a href="#vehicle-history">All previous cars</a><a href="#450sel-69">The original 6.9</a></nav><article class="garage-vehicle vehicle-card vehicle-card--hero" id="e250-cdi"><header class="vehicle-heading"><p class="eyebrow">In the garage · Mercedes-Benz · 2011</p><h1 id="e250-title">E 250 CDI</h1><p class="vehicle-subtitle">BlueEFFICIENCY Coupé</p><p class="vehicle-description">A Bremen-built E-Class coupe in Obsidian Black, with black leather and the OM651 diesel. My current Mercedes, and the car behind MBLink.</p></header><div class="vehicle-overview"><dl class="hero-key-facts"><div><dt>Power</dt><dd>150 <span>kW</span></dd></div><div><dt>Torque</dt><dd>500 <span>Nm</span></dd></div><div><dt>Engine</dt><dd>2.143 <span>litre diesel</span></dd></div></dl><div class="vehicle-story"><p>Originally delivered to Australia, this right-hand-drive, pre-facelift C207 brings two interests together: Mercedes engineering and my <a class="inline-link" href="workbench.html#link">MBLink</a> diagnostic software.</p><p class="equipment-preview">Panoramic roof · COMAND navigation · Heated front seats · Intelligent Light System</p></div></div><div class="vehicle-photo-pair" aria-label="Photographs of the 2011 Mercedes-Benz E 250 CDI Coupé"><figure><a class="garage-photo-link" href="assets/photos/e250-cdi-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/e250-cdi-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Obsidian Black E 250 CDI Coupé photographed from the front three-quarter angle, showing its coupé profile and five-spoke wheels"></a><figcaption>E 250 CDI · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/e250-cdi-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/e250-cdi-front.webp" width="520" height="400" loading="lazy" decoding="async" alt="Front of the Obsidian Black 2011 E 250 CDI Coupé with Victorian registration YSF597"></a><figcaption>E 250 CDI · YSF597</figcaption></figure></div>)";
    o << vehicle_record(current) << "</article></section>";
    o << R"(<section class="garage-chapter" id="vehicle-history" aria-labelledby="vehicle-history-title"><header class="chapter-heading"><p class="eyebrow">The earlier cars · newest to oldest</p><h2 id="vehicle-history-title">A garage history, in order.</h2><p>From Australian Fords and diesels to the Mercedes that began this site. Exact build dates are not recorded for every car; where necessary, the position and label reflect the model generation rather than a verified manufacture year.</p></header>)";
    const auto cars = historical_cars();
    o << R"(<nav class="garage-jump" aria-label="Jump to a previous car"><div class="garage-jump-heading"><strong>Find a car</strong><span>Thirteen previous vehicles · newest to oldest</span></div><div class="garage-jump-links">)";
    for (const auto& car : cars) {
        const auto separator = car.year_label.find(" ·");
        const string year = car.year_label.substr(0, separator);
        o << "<a href=\"#" << esc(car.anchor) << "\"><small>" << esc(year)
          << "</small><span>" << esc(car.title) << "</span></a>";
    }
    o << R"(</div></nav><div class="garage-register">)";
    for (const auto& car : cars) {
        if (car.model == "500SEC") {
            o << R"(<article class="garage-entry garage-vehicle vehicle-card vehicle-card--historic" id="500-sec"><div class="vehicle-heading"><div class="tag">1985 · Mercedes-Benz · C126 · AMG</div><h3>500SEC AMG</h3><p class="year-marker">)"
              << esc(car.year_label)
              << R"(</p><p class="vehicle-description">Smoke Silver over beige leather, right-hand drive and UK delivered. Built at Sindelfingen in 1985 before its AMG conversion, this C126 was a particularly distinctive Mercedes.</p><p class="vehicle-origin">UK delivery · Smoke Silver · Beige leather</p></div>)";
            o << vehicle_facts({{"Make", "Mercedes-Benz", ""}, {"Model", "500SEC", ""},
                                {"Chassis / Series", "126.044 · C126", ""},
                                {"Powertrain", "4,973 cc M117.963 V8", ""}}, "car-facts")
              << R"(<div class="vehicle-photo-pair" aria-label="Photographs of the 500SEC AMG"><figure><a class="garage-photo-link" href="assets/photos/500sec-amg-side.jpg" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/500sec-amg-side.jpg" width="1021" height="409" loading="lazy" decoding="async" alt="Smoke Silver 500SEC AMG photographed from the side, showing the AMG bodywork and five-spoke wheels"></a><figcaption>500SEC AMG · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/500sec-amg-front.jpg" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/500sec-amg-front.jpg" width="520" height="400" loading="lazy" decoding="async" alt="Front of the Smoke Silver 500SEC AMG with Western Australian registration 1ARK884"></a><figcaption>500SEC AMG · 1ARK884</figcaption></figure></div>)"
              << vehicle_record(sec) << "</article>";
        } else {
            o << historical_car(car);
        }
    }
    o << R"(</div></section><section class="destination-strip"><div><p class="eyebrow">Where it began</p><h2>The original 6.9 page.</h2><p>The W116 450SEL 6.9 was the subject of the original site, which began in 1993.</p></div><a class="text-link" href="about.html#the-site">The story of the site <span aria-hidden="true">→</span></a></section>)";
    return o.str() + page_end("A personal garage history.");
}

static string render_about() {
    std::ostringstream o;
    o << page_start("About", "About Shannon Smith: software, cars, open-reel tape, watches and a personal website that began with the Infiltrator 450SEL 6.9 page.", "About", "about-page");
    o << R"(<section class="page-intro"><p class="eyebrow">About</p><h1>A place of my own.</h1><p class="lead">I'm Shannon Smith, in Mooroopna, Victoria. Software, cars and an interest in how things work have been part of my website since 1993.</p><nav class="section-nav" aria-label="About sections"><a href="#interests">Interests</a><a href="#the-site">Site history</a><a href="#principles">Principles</a></nav></section>
<section id="interests" class="about-interests"><header class="section-heading"><h2>Beyond the keyboard.</h2><p class="section-deck">The same interest in engineering, on a different scale.</p></header><div class="project-content"><article class="interest-entry"><h3>Open-reel tape.</h3><p>Tape decks and analogue audio sit alongside the music itself. Mechanical engineering is part of the appeal.</p></article><article class="interest-entry"><h3>On the wrist.</h3><p>A Cosmograph Daytona 126500LN in Oystersteel. Another way to enjoy a precisely made machine.</p></article><article class="interest-entry"><h3>Off the clock.</h3><p>Music, films and games have their place here too. The garage has its <a class="inline-link" href="garage.html">own collection of stories</a>.</p></article></div></section>
<section id="the-site" class="about-history"><header class="section-heading"><p class="eyebrow">ssmithnet.net</p><h2>Before the feed,<br>there was a home page.</h2><p class="section-deck">An address that stayed while the interests grew.</p></header><div class="timeline">)";
    const vector<Event> events = {
        {"1993", "The Infiltrator website begins", "I started my website in 1993 with the Mercedes-Benz 450SEL 6.9 as its original subject. The site has grown considerably since then."},
        {"Beta years", "Windows, Solaris & games", "Voluntary beta testing included Microsoft Windows ME, the Solaris beta team and EA game projects. Windows ME testing included a Beta Tester of the Month award."},
        {"Software projects", "The workshop grows", "My projects expanded to include System Monitor, Calendar, Defragmenter, InfiltratorFS, Calculator and vehicle diagnostics."},
        {"Today", "The work continues", "Cars now share the domain with InfiltratorFS, native software and the Link diagnostic family. It remains a personal website."}
    };
    for (const auto& event : events)
        o << "<article class=\"event\"><span class=\"era\">" << esc(event.date) << "</span><div><h3>" << esc(event.title) << "</h3><p>" << esc(event.description) << "</p></div></article>";
    o << R"(</div></section><section id="principles" class="quote-section"><p class="eyebrow">A personal principle</p><blockquote class="quote">“I am free, no matter what rules surround me. If I find them tolerable, I tolerate them; if I find them too obnoxious, I break them. I am free because I know that I alone am morally responsible for everything I do.”<cite>— Robert A. Heinlein</cite></blockquote></section>)";
    return o.str() + page_end("Since 1993. Still here.");
}

static string render_download() {
    const string url = "https://github.com/Infiltrator-Projects/ssmithnet.net/releases/download/model-11-sandy-v1.0.0/Model_11_Sandy_v.1.0.0.zip";
    auto page = page_start("Model 11 Sandy v1.0.0", "Download Model 11 Sandy v1.0.0.", "Download");
    const auto head_end = page.find("</head>");
    page.insert(head_end, "<meta http-equiv=\"refresh\" content=\"2;url=" + esc(url) + "\">");
    return page + "<section class=\"page-intro\"><div class=\"kicker\">Download</div><h1>Model 11 Sandy<br> v1.0.0</h1><p class=\"lead\">Your download should begin automatically.</p><p><a class=\"text-link\" href=\"" + esc(url) + "\">Download the ZIP now <span aria-hidden=\"true\">↗</span></a></p></section>" + page_end("Direct download");
}

int main(int argc, char** argv) {
    try {
        if (argc > 2) throw std::runtime_error("usage: sitegen [output-directory]");
        const fs::path root = argc == 2 ? fs::absolute(argv[1]) : fs::current_path();
        write(root / "index.html", render_home());
        write(root / "workbench.html", render_workbench());
        write(root / "garage.html", render_garage());
        // The old Archive address remains a full, canonicalised alias for bookmarks.
        const auto about = render_about();
        write(root / "about.html", about);
        write(root / "archive.html", about);
        write(root / "downloads/model-11-sandy/index.html", render_download());
        std::cout << "Generated ssmithnet.net pages from C++ source.\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "sitegen: " << ex.what() << '\n';
        return 1;
    }
}
