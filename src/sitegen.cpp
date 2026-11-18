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
    const string filename = active == "Home" ? "" : active == "Workbench" ? "workbench.html" : active == "Projects" ? "projects.html" : active == "Garage" ? "garage.html" : active == "Download" ? "downloads/model-11-sandy/" : "about.html";
    const string prefix = active == "Download" ? "../../" : "";
    // Use existing site imagery for social previews; never claim an invented app screenshot.
    const string social_image = active == "Home" ? "revox-b77-mkii-dbx224-technics-su-z400.webp" :
                                active == "Garage" ? "e250-cdi-three-quarter.webp" : "";
    const string social_meta = social_image.empty() ? "" :
        "<meta property=\"og:image\" content=\"https://ssmithnet.net/assets/photos/" + social_image + "\">" +
        "<meta name=\"twitter:card\" content=\"summary_large_image\">";
    return infiltrator_web::head(title + " — ssmithnet.net", description, "https://ssmithnet.net/" + filename, prefix + "assets/") + social_meta +
        "<link rel=\"icon\" type=\"image/svg+xml\" href=\"" + esc(prefix + "assets/brand-2026.svg") + "\">" +
        "<link rel=\"stylesheet\" href=\"" + esc(prefix + "assets/site-redesign.css?v=20261118-polish6") + "\">" +
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
    const bool is_xc_1 = c.anchor == "xc-falcon-ute-1";
    const bool is_xc_2 = c.anchor == "xc-falcon-ute-2";
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
    if (is_xc_1) o << R"(<p class="vehicle-reconstruction-note">Illustrative reconstructions of my first blue XC Falcon ute, with the grille deliberately debadged. These are not photographs of my original vehicle.</p>)";
    if (is_xc_2) o << R"(<p class="vehicle-reconstruction-note">Illustrative reconstructions of my second blue XC Falcon ute. Unlike ute one, this vehicle retained its Ford oval badge on the grille; these are not original photographs.</p>)";
    o << "</div><dl class=\"car-facts\">"
      << "<div><dt>Make</dt><dd>" << esc(c.make) << "</dd></div>"
      << "<div><dt>Model</dt><dd>" << esc(c.model) << "</dd></div>"
      << "<div><dt>Chassis / Series</dt><dd>" << esc(c.series) << "</dd></div>"
      << "<div><dt>Powertrain</dt><dd>" << esc(c.powertrain) << "</dd></div>"
      << "</dl>";
    if (is_xc_2) o << R"(<div class="vehicle-photo-pair" aria-label="Illustrative views of blue XC Falcon ute number two with Ford grille badge"><figure><a class="garage-photo-link" href="assets/photos/xc-falcon-ute-2-three-quarter.avif" aria-label="Open illustrative three-quarter photograph"><img src="assets/photos/xc-falcon-ute-2-three-quarter.avif" width="640" height="480" loading="lazy" decoding="async" alt="Blue XC Falcon ute from a front passenger-side three-quarter angle, with Ford oval grille badge, bonnet flutes, chrome bumper and blurred number plate"></a><figcaption>XC Falcon ute 2 · three-quarter passenger-side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/xc-falcon-ute-2-front.avif" aria-label="Open illustrative front photograph"><img src="assets/photos/xc-falcon-ute-2-front.avif" width="640" height="640" loading="lazy" decoding="async" alt="Head-on photograph-style reconstruction of blue XC Falcon ute number two, showing the Ford oval on the black grille, four round headlights and blurred number plate"></a><figcaption>XC Falcon ute 2 · front view with Ford badge</figcaption></figure></div>)";
    if (is_xc_1) o << R"(<div class="vehicle-photo-pair" aria-label="Illustrative views of the debadged blue XC Falcon ute number one"><figure><a class="garage-photo-link" href="assets/photos/xc-falcon-ute-1-three-quarter.avif" aria-label="Open illustrative three-quarter photograph"><img src="assets/photos/xc-falcon-ute-1-three-quarter.avif" width="640" height="480" loading="lazy" decoding="async" alt="Metallic-blue XC Falcon ute from the front passenger-side three-quarter angle, with black lower stripe, recessed bonnet flutes, debadged grille and blurred registration plate"></a><figcaption>XC Falcon ute 1 · three-quarter passenger-side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/xc-falcon-ute-1-front.avif" aria-label="Open illustrative front photograph"><img src="assets/photos/xc-falcon-ute-1-front.avif" width="640" height="640" loading="lazy" decoding="async" alt="Head-on view of the blue XC Falcon ute with twin recessed bonnet flutes, four round headlights, completely debadged black grille and blurred number plate"></a><figcaption>XC Falcon ute 1 · front view</figcaption></figure></div>)";
    if (is_69) o << R"(<div class="vehicle-photo-pair" aria-label="Reconstructed photographs of the yellow-beige Mercedes-Benz 450SEL 6.9"><figure><a class="garage-photo-link" href="assets/photos/450sel-69-side-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/450sel-69-side-view.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Reconstruction of a fully optioned right-hand-drive W116 450SEL 6.9 in pale ochre yellow-beige, front three-quarter view with original chrome trim and alloy wheels; no exterior front or quarter-panel 6.9 emblems"></a><figcaption>450SEL 6.9 · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/450sel-69-front-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/450sel-69-front-view.webp" width="520" height="400" loading="lazy" decoding="async" alt="Reconstruction of a pale yellow-beige Australian W116 450SEL 6.9 straight on, with right-hand steering, European headlamps and unbadged grille"></a><figcaption>450SEL 6.9 · front view</figcaption></figure></div>)";
    if (is_280se) o << R"(<div class="vehicle-photo-pair" aria-label="Reconstructed images of the white Mercedes-Benz 280SE"><figure><a class="garage-photo-link" href="assets/photos/280se-side-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/280se-side-view.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Reconstructed white W116 Mercedes-Benz 280SE standard-wheelbase saloon in three-quarter side view, with original-style hubcaps and Western Australian number-plate format"></a><figcaption>280SE · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/280se-front-view.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/280se-front-view.webp" width="660" height="507" loading="lazy" decoding="async" alt="Reconstructed head-on view of the white Mercedes-Benz W116 280SE, showing its chrome grille, European headlights and Western Australian-style registration plate"></a><figcaption>280SE · front view</figcaption></figure></div>)";
    if (is_xf) o << R"(<div class="vehicle-photo-pair" aria-label="Reference images of the beige Ford XF Falcon S-Pack"><figure><a class="garage-photo-link" href="assets/photos/xf-s-pack-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/xf-s-pack-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Beige XF Falcon S-Pack photographed from the front-left three-quarter angle, nose pointing left, showing the body-coloured Ford grille and five-spoke-style alloy wheels"></a><figcaption>XF Falcon S-Pack · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/xf-s-pack-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/xf-s-pack-front.webp" width="520" height="400" loading="lazy" decoding="async" alt="Illustrative head-on view of a beige XF Falcon S-Pack with body-coloured horizontal grille, blue Ford oval, rectangular headlights and black bumper"></a><figcaption>XF Falcon S-Pack · front view</figcaption></figure></div>)";
    if (is_el) o << R"(<div class="vehicle-photo-pair" aria-label="Illustrative views of a Victorian taxi-yellow Ford EL Falcon"><figure><a class="garage-photo-link" href="assets/photos/el-falcon-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/el-falcon-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Illustrative Victorian taxi-yellow EL Falcon four-door sedan from the front-left three-quarter angle, with oval Ford grille and standard wheel covers, without taxi signs"></a><figcaption>EL Falcon · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/el-falcon-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/el-falcon-front.webp" width="520" height="400" loading="lazy" decoding="async" alt="Illustrative head-on view of a Victorian taxi-yellow Ford EL Falcon showing the oval grille, rounded headlights and regular bumper, without taxi markings"></a><figcaption>EL Falcon · front view</figcaption></figure></div>)";
    if (is_torana) o << R"(<div class="vehicle-photo-pair" aria-label="Supplied views of the white Holden LC Torana"><figure><a class="garage-photo-link" href="assets/photos/lc-torana-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/lc-torana-three-quarter.webp" width="1021" height="510" loading="lazy" decoding="async" alt="White Holden LC Torana four-door sedan in a front three-quarter view, with external windscreen sun visor, chrome trim, red interior and a tree-lined parking bay"></a><figcaption>LC Torana · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/lc-torana-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/lc-torana-front.webp" width="520" height="510" loading="lazy" decoding="async" alt="Front view of a white Holden LC Torana with pointed bonnet, central Holden grille badge, external windscreen sun visor, chrome bumper and blurred registration plate"></a><figcaption>LC Torana · front view</figcaption></figure></div>)";
    if (is_discovery) o << R"(<div class="vehicle-photo-pair" aria-label="Supplied views of the Land Rover Discovery II"><figure><a class="garage-photo-link" href="assets/photos/discovery-2-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/discovery-2-three-quarter.webp" width="642" height="348" loading="lazy" decoding="async" alt="Dark Land Rover Discovery II in a front three-quarter view with roof rails, black grille and silver alloy wheels"></a><figcaption>Discovery 2 · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/discovery-2-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/discovery-2-front.webp" width="642" height="387" loading="lazy" decoding="async" alt="Front view of a dark Land Rover Discovery II with rectangular headlights, roof rails and black bumper"></a><figcaption>Discovery 2 · front view</figcaption></figure></div>)";
    if (is_peugeot) o << R"(<div class="vehicle-photo-pair" aria-label="Supplied views of the white Peugeot 405 GLD"><figure><a class="garage-photo-link" href="assets/photos/peugeot-405-gld-three-quarter.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/peugeot-405-gld-three-quarter.webp" width="642" height="348" loading="lazy" decoding="async" alt="White Peugeot 405 GLD four-door saloon in a front three-quarter view, with black side mouldings and grey wheel covers"></a><figcaption>Peugeot 405 GLD · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/peugeot-405-gld-front.webp" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/peugeot-405-gld-front.webp" width="642" height="387" loading="lazy" decoding="async" alt="Front view of a white Peugeot 405 GLD with rectangular headlamps, black bumper and the Peugeot lion badge"></a><figcaption>Peugeot 405 GLD · front view</figcaption></figure></div>)";
    o << R"(<a class="vehicle-index-return" href="#garage-index">↑ Back to the vehicle index</a></article>)";
    return o.str();
}

static vector<Car> historical_cars() {
    vector<Car> cars{
        {"2004 · Ford · BA · LTD", "2004 LTD", "Ford", "LTD", "BA", "5.4 L V8 · Petrol / LPG", "A long-wheelbase BA LTD with the 5.4-litre V8 and dual-fuel petrol/LPG. This was the luxury end of the large Australian Fords I owned, following the earlier BA Fairmont. The combination of a big V8 and LPG is a detail worth recording rather than reducing it to a generic model listing.", "ba-ltd", "2004 · recorded year", 2004},
        {"2003 · Ford · BA · Fairmont", "2003 Fairmont", "Ford", "Fairmont", "BA", "5.4 L V8 · Petrol / LPG", "My 2003 BA Fairmont combined the 5.4-litre V8 with petrol/LPG dual fuel. It belongs to the same broad Ford era as the later LTD, but the two cars were different ownership entries—not one vehicle listed under two trims.", "ba-fairmont", "2003 · recorded year", 2003},
        {"2002 · Land Rover · Discovery II", "Discovery 2 TDI", "Land Rover", "Discovery 2", "Discovery II", "TDI diesel", "The 2002 Land Rover Discovery II was the diesel four-wheel-drive exception to a garage otherwise dominated by saloons, coupés and Australian Fords. It brought a very different purpose and layout to the collection, and is one of the entries for which reference images have been supplied.", "discovery-2", "2002 · recorded year", 2002},
        {"2002 · Ford · AU Series II · Fairmont", "Fairmont", "Ford", "Fairmont", "AU Series II", "4.0 L · Petrol / LPG", "My 2002 AU Series II Fairmont had the 4.0-litre six-cylinder with petrol/LPG dual fuel. Together with the later V8 Fairmont and LTD, it shows how different the same Australian Ford family could be in engine, body and character. It is a separate 2002 car, not the 2003 BA.", "au-fairmont", "2002 · recorded year", 2002},
        {"Ford · EL · Tickford LPG", "EL Falcon", "Ford", "EL Falcon", "EL", "Petrol / LPG · Tickford", "Victorian taxi yellow paint, but not a taxi: this was my privately owned EL Falcon with Tickford-installed LPG. Only the colour came from the cab rank.", "el-falcon", "1996–1998 · model era", 1997},
        {"Peugeot · 405 · GLD", "405 GLD", "Peugeot", "405 GLD", "405", "Diesel · waste motor oil", "This was a white Peugeot 405 GLD diesel with a black interior—not a 504. Its place in the history is memorable for the fuel experiment: I ran it on waste motor oil. That experience makes it a particular mechanical story rather than simply another name among the sedans.", "peugeot-405-gld", "Late 1980s–1990s · model era", 1992},
        {"1985 · Mercedes-Benz · C126 · AMG", "500SEC AMG", "Mercedes-Benz", "500SEC", "126.044 · C126", "4,973 cc M117.963 V8", "Smoke Silver over beige leather, right-hand drive and UK delivered. Built at Sindelfingen in 1985 before its AMG conversion, this C126 was a particularly distinctive Mercedes.", "500-sec", "1985 · factory record", 1985},
        {"1985 · Ford · XF · S-Pack", "XF Falcon S-Pack", "Ford", "XF Falcon S-Pack", "XF", "Petrol", "My beige 1985 XF Falcon S-Pack sits between the larger V8 Mercedes stories and the later Falcons. It represents another side of the Australian car history: a familiar, square-edged Ford whose particular S-Pack identity matters more here than a long list of generic XF specifications.", "xf-s-pack", "1985 · recorded year", 1985},
        {"Mercedes-Benz · W116 · 6.9", "450SEL 6.9", "Mercedes-Benz", "450SEL 6.9", "116.036 · W116", "6,834 cc M100.985 V8", "The fully optioned, yellow-beige Australian 450SEL 6.9 with the M100 V8. This was the car behind the original Infiltrator website, which I began in 1993.", "450sel-69", "1975–1980 · model era", 1978},
        {"Ford · XC · blue ute", "XC Falcon ute (1 of 2)", "Ford", "Falcon ute", "XC", "Six-cylinder", "One of two blue, six-cylinder XC Falcon utes I owned. A utility-bodied Ford alongside the Falcon sedans in my garage history.", "xc-falcon-ute-1", "1976–1979 · model era", 1978},
        {"Ford · XC · blue ute", "XC Falcon ute (2 of 2)", "Ford", "Falcon ute", "XC", "Six-cylinder", "The other blue XC Falcon ute I owned, also with a six-cylinder engine. These were two separate vehicles; their individual build years are not recorded.", "xc-falcon-ute-2", "1976–1979 · model era", 1978},
        {"Mercedes-Benz · W116 · 280SE", "280SE", "Mercedes-Benz", "280SE", "116.024 · W116", "2,746 cc M110 I6", "My white, six-cylinder W116 280SE with traditional hubcaps. A more understated companion to the 450SEL 6.9 in my Mercedes history.", "280-se", "1972–1980 · model era", 1977},
        {"Holden · LC · 1.1 litre", "LC Torana", "Holden", "Torana", "LC", "1.1 L I4", "My LC Torana was the small 1.1-litre four-cylinder car in a history that also includes large V8 Mercedes-Benz and Ford models. That contrast is the reason it belongs in the same record: the smallest and simplest machines can be as individually memorable as the grandest.", "lc-torana", "1969–1972 · model era", 1970},
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
    <p class="landing-intro">A personal website since 1993. I build software from the foundations upwards, document the cars I've owned and keep the mechanical and analogue things that interest me in view.</p>
    <div class="landing-actions"><a class="ui-action ui-action-primary" href="workbench.html">Explore the Workbench <span aria-hidden="true">↗</span></a><a class="ui-action ui-action-secondary" href="garage.html">Visit the Garage <span aria-hidden="true">→</span></a></div>
  </div>
  <div class="hero-accent" aria-hidden="true"><span class="hero-wave"></span><span class="hero-wave-copy"></span></div>
</section>
<section class="landing-hubs" aria-label="Explore the site">
  <a class="hub-tile hub-tile-primary" href="workbench.html"><img class="hub-symbol" src="assets/graphics/ui-workbench.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>Workbench</strong><span>Filesystems, native applications, diagnostics and analogue hardware.</span></span><b aria-hidden="true">↗</b></a>
  <a class="hub-tile" href="garage.html"><img class="hub-symbol" src="assets/graphics/ui-garage.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>Garage</strong><span>My current Mercedes, former cars and their stories.</span></span><b aria-hidden="true">↗</b></a>
  <a class="hub-tile" href="projects.html"><img class="hub-symbol" src="assets/graphics/ui-code.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>Projects</strong><span>The full source catalogue: applications, modules, games and infrastructure.</span></span><b aria-hidden="true">↗</b></a>
  <a class="hub-tile" href="about.html"><img class="hub-symbol" src="assets/graphics/ui-about.svg" alt="" width="48" height="48" loading="lazy" decoding="async"><span class="hub-content"><strong>About</strong><span>The site, its history and the interests behind it.</span></span><b aria-hidden="true">↗</b></a>
</section>

<section class="home-software" aria-labelledby="home-software-title">
  <div class="home-software-copy">
    <p class="eyebrow">Featured engineering · Software foundations</p>
    <h2 id="home-software-title">I build the parts below the interface.</h2>
    <p>InfiltratorFS is my own filesystem design. Libraries supplies common contracts; Boot and Files consume filesystem capabilities in different environments; InfiltratorOS brings the components together. Each project has its own source, tests and qualification limits.</p>
    <p>It's the sort of work I enjoy most: finding out what each layer must do, making the boundaries explicit and testing what actually works rather than relying on a successful build.</p>
    <div class="home-software-links"><a href="workbench.html#platform">How the architecture works ↗</a><a href="workbench.html#infiltratorfs">Inside InfiltratorFS ↗</a><a href="projects.html#featured-work">Explore featured code ↗</a></div>
  </div>
  <figure class="home-software-diagram">
    <picture>
      <source media="(max-width: 650px)" srcset="assets/graphics/software-architecture-mobile.svg">
      <img src="assets/graphics/software-architecture.svg" width="680" height="520" loading="lazy" decoding="async" alt="Architecture schematic: Libraries defines shared contracts, filesystem engines own formats, Boot and Files use those engines, and InfiltratorOS coordinates the components.">
    </picture>
    <figcaption>Component responsibilities, from shared interfaces to integration.</figcaption>
  </figure>
</section>
<section class="landing-showcase" aria-label="Featured collections">
  <article class="home-revox">
    <div class="showcase-description">
      <p class="eyebrow">Featured hardware · Open-reel audio</p>
      <h2>ReVox B77 Mk II <span>+ dbx 224 + Technics SU-Z400</span></h2>
      <p>My ReVox B77 Mk II, dbx 224 and Technics SU-Z400 represent three distinct parts of my analogue setup: tape transport, external Type II noise reduction and stereo amplification. The redrawn artwork includes the BASF tape reel and my silver metal reel. The Workbench has the equipment story.</p>
      <a class="ui-action ui-action-outline" href="workbench.html#revox-b77">Explore the tape setup <span aria-hidden="true">↗</span></a>
    </div>
    <figure class="revox-preview">
      <img src="assets/photos/revox-b77-mkii-dbx224-technics-su-z400.webp" width="1200" height="900" decoding="async" alt="Illustration of my ReVox B77 Mk II with BASF tape reel on the left, silver metal reel on the right and illuminated VU meters; the dbx 224 and Technics SU-Z400 amplifier are below">
      <figcaption>ReVox B77 Mk II · dbx 224 · Technics SU-Z400 · redrawn from my own equipment.</figcaption>
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
<section class="home-editorial" aria-labelledby="home-editorial-title">
  <div class="home-editorial-heading">
    <p class="eyebrow">The story so far</p>
    <h2 id="home-editorial-title">Since 1993.<br>Still curious.</h2>
  </div>
  <div class="home-editorial-copy">
    <p>I began with a page about my 450SEL 6.9. The interests have grown into filesystems, software, cars and analogue machinery, but this is still a place to keep the work and its history together.</p>
    <a class="text-link" href="about.html#the-site">How the website began <span aria-hidden="true">↗</span></a>
  </div>
</section>
)site" + page_end("A personal corner of the internet, since 1993.");
}

static string render_projects() {
    return page_start("Projects", "All 104 publicly visible Infiltrator-Projects GitHub repositories: operating systems, applications, diagnostics, calendar and clock modules, filesystem engines, games and publishing.", "Projects", "projects-page") + R"CATALOG(
<section class="page-intro projects-intro">
  <p class="eyebrow">Source index · Infiltrator-Projects</p><h1>Repository directory.</h1>
  <p class="lead">Browse public source code by repository name, purpose or family. The catalogue includes application projects as well as the libraries and individual modules that support them.</p>
  <p class="projects-reading-hint">A repository may be an application, a shared library or one filesystem engine—not necessarily something ready to install. Each major family links back to its engineering explanation; the entries below link to the actual code.</p>
  <p class="projects-index-meta"><strong>104 public repositories</strong><span>8 families</span><span>Inventory captured 16 November 2026</span></p>
  <nav class="projects-utility-nav" aria-label="Related destinations"><a href="workbench.html">Read the engineering journal ↗</a><a href="https://github.com/Infiltrator-Projects">Open the live GitHub organisation ↗</a></nav>
</section>

<section class="projects-featured" id="featured-work" aria-labelledby="featured-work-title">
  <header class="projects-featured-heading"><div><p class="eyebrow">Four places to start</p><h2 id="featured-work-title">Selected engineering work.</h2><p>These four projects are starting points into the wider codebase. Each links to its engineering notes and source.</p></div></header>
  <div class="projects-featured-list">
    <article><span class="feature-number">01 / Storage</span><h3>InfiltratorFS</h3><p>My clean-sheet filesystem: generation-based copy-on-write, checkpoints, snapshots, and integrity checks. The interesting question is what data and metadata survive an interrupted operation.</p><div class="feature-links"><a href="workbench.html#infiltratorfs">Architecture and limits ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS">Source ↗</a></div></article>
    <article><span class="feature-number">02 / Foundations</span><h3>InfiltratorOS</h3><p>A developing operating environment with independent Boot, Kernel, Files, and filesystem components. I am testing the contracts between them rather than treating one passing subsystem as a finished OS.</p><div class="feature-links"><a href="workbench.html#platform">How the layers fit ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorOS">Source ↗</a></div></article>
    <article><span class="feature-number">03 / Native applications</span><h3>System Monitor</h3><p>A native tool for inspecting the running machine through operating-system interfaces. Its implementation and tests belong in the same conversation as the results shown to users.</p><div class="feature-links"><a href="workbench.html#software">Application approach ↗</a><a href="https://github.com/Infiltrator-Projects/System-Monitor">Source ↗</a></div></article>
    <article><span class="feature-number">04 / Vehicle diagnostics</span><h3>MBLink</h3><p>Mercedes-Benz diagnostics built on the shared Link engine, informed by work with my own C207 E 250 CDI. Identification, capture, and interpretation are different responsibilities.</p><div class="feature-links"><a href="workbench.html#link">Read the diagnostic method ↗</a><a href="https://github.com/Infiltrator-Projects/MBLink">Source ↗</a></div></article>
  </div>
</section>
<section class="repository-browser" id="directory" aria-labelledby="directory-title">
  <div class="repository-browser-header"><div><p class="eyebrow">Search and browse</p><h2 id="directory-title">Find a repository.</h2><p>Choose a family below or filter by project name or purpose. Calendar, clock and filesystem engines are grouped into expandable lists.</p></div>
    <div class="repository-search"><label for="repository-search">Find a repository</label><div class="repository-search-control"><input id="repository-search" type="search" placeholder="Name, purpose or family" autocomplete="off" aria-controls="repo-families" aria-describedby="repository-search-help"><button id="repository-clear" type="button" hidden>Clear</button></div><span class="repository-search-help" id="repository-search-help">Search titles, descriptions or project families.</span><output id="repository-count" role="status" aria-live="polite">104 repositories</output></div>
  </div>
  <nav class="repository-families-nav" aria-label="Jump to project family"><a href="#group-foundations">Operating system &amp; foundations <span>9</span></a><a href="#group-applications">Applications &amp; utilities <span>7</span></a><a href="#group-diagnostics">Vehicle diagnostics <span>6</span></a><a href="#group-creative">Games &amp; creative work <span>3</span></a><a href="#group-publishing">Sites &amp; publishing <span>2</span></a><a href="#group-calendar-modules">Calendar systems <span>31</span></a><a href="#group-clock-modules">Clock &amp; timekeeping modules <span>23</span></a><a href="#group-filesystem-modules">Filesystem engines <span>23</span></a></nav>
  <div id="repo-families" class="repository-families">
<section class="repo-group" id="group-foundations" data-repo-group><header class="repo-group-heading"><div><h3>Operating system &amp; foundations</h3><p>Common libraries, OS integration, the boot environment, file management and InfiltratorFS. <a class="repo-group-context-link" href="workbench.html#platform">How the system layers fit together ↗</a></p></div><span class="repo-group-count">9 repositories</span></header><div class="repo-grid">
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Boot" data-repo-name="Boot"><strong>Boot</strong><span>Firmware-stage boot manager and Common V2 filesystem support.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Files" data-repo-name="Files"><strong>Files</strong><span>Native file-management project.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Filesystems" data-repo-name="Filesystems"><strong>Filesystems</strong><span>Transitional catalogue; engines are owned by the separate filesystem repositories.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/InfiltratorFS" data-repo-name="InfiltratorFS"><strong>InfiltratorFS</strong><span>Original copy-on-write filesystem, checksums and recovery tools.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/InfiltratorOS" data-repo-name="InfiltratorOS"><strong>InfiltratorOS</strong><span>Operating-system integration and orchestration; not yet a completed standalone OS.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Kernel" data-repo-name="Kernel"><strong>Kernel</strong><span>Kernel source and build work; not yet a qualified bootable release.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Libraries" data-repo-name="Libraries"><strong>Libraries</strong><span>C11 shared foundations, host contracts and filesystem ABI definitions.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Software" data-repo-name="Software"><strong>Software</strong><span>Graphical package management, updates and repositories.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/System-Settings" data-repo-name="System-Settings"><strong>System-Settings</strong><span>System settings and configuration project.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></section>
<section class="repo-group" id="group-applications" data-repo-group><header class="repo-group-heading"><div><h3>Applications &amp; utilities</h3><p>Native tools for desktops, system management, calculations, scheduling and development.</p></div><span class="repo-group-count">7 repositories</span></header><div class="repo-grid">
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Calculator" data-repo-name="Calculator"><strong>Calculator</strong><span>Native cross-platform calculator.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Calendar" data-repo-name="Calendar"><strong>Calendar</strong><span>Cinnamon calendar and clock with alternative time systems.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Defragmenter" data-repo-name="Defragmenter"><strong>Defragmenter</strong><span>Offline allocation analysis, recovery and qualified defragmentation.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/PowerShell-HAL" data-repo-name="PowerShell-HAL"><strong>PowerShell-HAL</strong><span>Natural language to editable PowerShell command insertion.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/RunnerScope" data-repo-name="RunnerScope"><strong>RunnerScope</strong><span>Desktop monitor for self-hosted GitHub Actions runners.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Slicer" data-repo-name="Slicer"><strong>Slicer</strong><span>Source repository and project notes.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/System-Monitor" data-repo-name="System-Monitor"><strong>System-Monitor</strong><span>Native Linux system and hardware inspection.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></section>
<section class="repo-group" id="group-diagnostics" data-repo-group><header class="repo-group-heading"><div><h3>Vehicle diagnostics</h3><p>The shared Link engine and separate Mercedes-Benz, Jaguar, BMW, Audi and Ford applications. <a class="repo-group-context-link" href="workbench.html#link">How diagnostic evidence is interpreted ↗</a></p></div><span class="repo-group-count">6 repositories</span></header><div class="repo-grid">
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/AUDILink" data-repo-name="AUDILink"><strong>AUDILink</strong><span>Audi diagnostic application built on Link.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/BMWLink" data-repo-name="BMWLink"><strong>BMWLink</strong><span>BMW diagnostic application built on Link.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/FORDLink" data-repo-name="FORDLink"><strong>FORDLink</strong><span>Ford diagnostic application built on Link.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/JAGLink" data-repo-name="JAGLink"><strong>JAGLink</strong><span>Jaguar diagnostic application built on Link.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Link" data-repo-name="Link"><strong>Link</strong><span>Shared C11 vehicle-diagnostics engine.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/MBLink" data-repo-name="MBLink"><strong>MBLink</strong><span>Mercedes-Benz diagnostic application built on Link.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></section>
<section class="repo-group" id="group-creative" data-repo-group><header class="repo-group-heading"><div><h3>Games &amp; creative work</h3><p>Original vehicle and civilisation simulations alongside character-development software. <a class="repo-group-context-link" href="workbench.html#creative">The models behind the projects ↗</a></p></div><span class="repo-group-count">3 repositories</span></header><div class="repo-grid">
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/backyard-racer" data-repo-name="backyard-racer"><strong>backyard-racer</strong><span>Car building, garage management and street-racing simulation.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Character-Profiler" data-repo-name="Character-Profiler"><strong>Character-Profiler</strong><span>iPhone character-development and story-bible app.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Egypt" data-repo-name="Egypt"><strong>Egypt</strong><span>Original Egyptian city-building and civilisation simulation.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></section>
<section class="repo-group" id="group-publishing" data-repo-group><header class="repo-group-heading"><div><h3>Sites &amp; publishing</h3><p>The website and the independent package publishing and APT repository system.</p></div><span class="repo-group-count">2 repositories</span></header><div class="repo-grid">
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/Infiltrator-Repository" data-repo-name="Infiltrator-Repository"><strong>Infiltrator-Repository</strong><span>Verified APT publishing pipeline and Software Centre.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry" href="https://github.com/Infiltrator-Projects/ssmithnet.net" data-repo-name="ssmithnet.net"><strong>ssmithnet.net</strong><span>This personal site and its native C++ generator.</span><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></section>
<details class="repo-group repo-group--compact" id="group-calendar-modules" data-repo-group><summary><span class="repo-group-text"><strong>Calendar systems</strong><span>31 independently owned calendar modules, not 31 separately released desktop applications.</span></span><span class="repo-group-count">31 repositories</span></summary><div class="repo-grid repo-grid--compact">
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Armenian-Traditional" data-repo-name="Calendar-Armenian-Traditional"><strong>Calendar-Armenian-Traditional</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Bahai" data-repo-name="Calendar-Bahai"><strong>Calendar-Bahai</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Buddhist" data-repo-name="Calendar-Buddhist"><strong>Calendar-Buddhist</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Byzantine" data-repo-name="Calendar-Byzantine"><strong>Calendar-Byzantine</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Chinese" data-repo-name="Calendar-Chinese"><strong>Calendar-Chinese</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Coptic" data-repo-name="Calendar-Coptic"><strong>Calendar-Coptic</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Dangi" data-repo-name="Calendar-Dangi"><strong>Calendar-Dangi</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Egyptian-Nabonassar" data-repo-name="Calendar-Egyptian-Nabonassar"><strong>Calendar-Egyptian-Nabonassar</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Ethiopian" data-repo-name="Calendar-Ethiopian"><strong>Calendar-Ethiopian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Ethiopic-Amete-Alem" data-repo-name="Calendar-Ethiopic-Amete-Alem"><strong>Calendar-Ethiopic-Amete-Alem</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-French-Republican" data-repo-name="Calendar-French-Republican"><strong>Calendar-French-Republican</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Gregorian" data-repo-name="Calendar-Gregorian"><strong>Calendar-Gregorian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Hebrew" data-repo-name="Calendar-Hebrew"><strong>Calendar-Hebrew</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Indian" data-repo-name="Calendar-Indian"><strong>Calendar-Indian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-International-Fixed" data-repo-name="Calendar-International-Fixed"><strong>Calendar-International-Fixed</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Islamic" data-repo-name="Calendar-Islamic"><strong>Calendar-Islamic</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Islamic-Civil" data-repo-name="Calendar-Islamic-Civil"><strong>Calendar-Islamic-Civil</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Islamic-TBLA" data-repo-name="Calendar-Islamic-TBLA"><strong>Calendar-Islamic-TBLA</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Islamic-Umalqura" data-repo-name="Calendar-Islamic-Umalqura"><strong>Calendar-Islamic-Umalqura</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-ISO-Week" data-repo-name="Calendar-ISO-Week"><strong>Calendar-ISO-Week</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Japanese" data-repo-name="Calendar-Japanese"><strong>Calendar-Japanese</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Julian" data-repo-name="Calendar-Julian"><strong>Calendar-Julian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Mayan" data-repo-name="Calendar-Mayan"><strong>Calendar-Mayan</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Minguo" data-repo-name="Calendar-Minguo"><strong>Calendar-Minguo</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Persian" data-repo-name="Calendar-Persian"><strong>Calendar-Persian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Positivist" data-repo-name="Calendar-Positivist"><strong>Calendar-Positivist</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Revised-Julian" data-repo-name="Calendar-Revised-Julian"><strong>Calendar-Revised-Julian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Roman" data-repo-name="Calendar-Roman"><strong>Calendar-Roman</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Swedish-Historical" data-repo-name="Calendar-Swedish-Historical"><strong>Calendar-Swedish-Historical</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-Thai-Buddhist-Solar" data-repo-name="Calendar-Thai-Buddhist-Solar"><strong>Calendar-Thai-Buddhist-Solar</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Calendar-World" data-repo-name="Calendar-World"><strong>Calendar-World</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></details>
<details class="repo-group repo-group--compact" id="group-clock-modules" data-repo-group><summary><span class="repo-group-text"><strong>Clock &amp; timekeeping modules</strong><span>23 modules for alternative, historical and standard systems of time.</span></span><span class="repo-group-count">23 repositories</span></summary><div class="repo-grid repo-grid--compact">
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Babylonian-Ancient" data-repo-name="Clock-Babylonian-Ancient"><strong>Clock-Babylonian-Ancient</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Babylonian-Hours" data-repo-name="Clock-Babylonian-Hours"><strong>Clock-Babylonian-Hours</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Binary" data-repo-name="Clock-Binary"><strong>Clock-Binary</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Chinese-Ke" data-repo-name="Clock-Chinese-Ke"><strong>Clock-Chinese-Ke</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Chinese-Time" data-repo-name="Clock-Chinese-Time"><strong>Clock-Chinese-Time</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Decimal" data-repo-name="Clock-Decimal"><strong>Clock-Decimal</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Hexadecimal" data-repo-name="Clock-Hexadecimal"><strong>Clock-Hexadecimal</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Indian-Ghati" data-repo-name="Clock-Indian-Ghati"><strong>Clock-Indian-Ghati</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Internet" data-repo-name="Clock-Internet"><strong>Clock-Internet</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Italian-Hours" data-repo-name="Clock-Italian-Hours"><strong>Clock-Italian-Hours</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Japanese-Temporal" data-repo-name="Clock-Japanese-Temporal"><strong>Clock-Japanese-Temporal</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Japanese-Temporal-Early" data-repo-name="Clock-Japanese-Temporal-Early"><strong>Clock-Japanese-Temporal-Early</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Julian" data-repo-name="Clock-Julian"><strong>Clock-Julian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Mean-Solar" data-repo-name="Clock-Mean-Solar"><strong>Clock-Mean-Solar</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Modified-Julian" data-repo-name="Clock-Modified-Julian"><strong>Clock-Modified-Julian</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Nuremberg-Hours" data-repo-name="Clock-Nuremberg-Hours"><strong>Clock-Nuremberg-Hours</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Nuremberg-Solar" data-repo-name="Clock-Nuremberg-Solar"><strong>Clock-Nuremberg-Solar</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Roman-Temporal" data-repo-name="Clock-Roman-Temporal"><strong>Clock-Roman-Temporal</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Sidereal" data-repo-name="Clock-Sidereal"><strong>Clock-Sidereal</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Solar" data-repo-name="Clock-Solar"><strong>Clock-Solar</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Standard-12" data-repo-name="Clock-Standard-12"><strong>Clock-Standard-12</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Standard-24" data-repo-name="Clock-Standard-24"><strong>Clock-Standard-24</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Clock-Unix" data-repo-name="Clock-Unix"><strong>Clock-Unix</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></details>
<details class="repo-group repo-group--compact" id="group-filesystem-modules" data-repo-group><summary><span class="repo-group-text"><strong>Filesystem engines</strong><span>23 independently owned filesystem repositories; presence does not mean read/write qualification.</span></span><span class="repo-group-count">23 repositories</span></summary><div class="repo-grid repo-grid--compact">
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-APFS" data-repo-name="Filesystem-APFS"><strong>Filesystem-APFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-Btrfs" data-repo-name="Filesystem-Btrfs"><strong>Filesystem-Btrfs</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-exFAT" data-repo-name="Filesystem-exFAT"><strong>Filesystem-exFAT</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-EXT2" data-repo-name="Filesystem-EXT2"><strong>Filesystem-EXT2</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-EXT3" data-repo-name="Filesystem-EXT3"><strong>Filesystem-EXT3</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-EXT4" data-repo-name="Filesystem-EXT4"><strong>Filesystem-EXT4</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-FAT12" data-repo-name="Filesystem-FAT12"><strong>Filesystem-FAT12</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-FAT16" data-repo-name="Filesystem-FAT16"><strong>Filesystem-FAT16</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-FAT32" data-repo-name="Filesystem-FAT32"><strong>Filesystem-FAT32</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-FFS" data-repo-name="Filesystem-FFS"><strong>Filesystem-FFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-HFS" data-repo-name="Filesystem-HFS"><strong>Filesystem-HFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-HFSPlus" data-repo-name="Filesystem-HFSPlus"><strong>Filesystem-HFSPlus</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-Minix" data-repo-name="Filesystem-Minix"><strong>Filesystem-Minix</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-NTFS" data-repo-name="Filesystem-NTFS"><strong>Filesystem-NTFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-OFS" data-repo-name="Filesystem-OFS"><strong>Filesystem-OFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-PFS3" data-repo-name="Filesystem-PFS3"><strong>Filesystem-PFS3</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-SFS" data-repo-name="Filesystem-SFS"><strong>Filesystem-SFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-SFS2" data-repo-name="Filesystem-SFS2"><strong>Filesystem-SFS2</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-Swap" data-repo-name="Filesystem-Swap"><strong>Filesystem-Swap</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-UFS1" data-repo-name="Filesystem-UFS1"><strong>Filesystem-UFS1</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-UFS2" data-repo-name="Filesystem-UFS2"><strong>Filesystem-UFS2</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-XFS" data-repo-name="Filesystem-XFS"><strong>Filesystem-XFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
<a class="repo-entry repo-entry--compact" href="https://github.com/Infiltrator-Projects/Filesystem-ZFS" data-repo-name="Filesystem-ZFS"><strong>Filesystem-ZFS</strong><span class="repo-entry-arrow" aria-hidden="true">↗</span></a>
</div></details>
  </div>
  <div class="repository-empty" id="repository-empty" role="status" hidden><strong>No matching repositories.</strong><p>Try another term or clear the search to see all 104 projects.</p></div>
  <p class="repository-disclosure">Inventory of publicly visible <a href="https://github.com/Infiltrator-Projects">Infiltrator-Projects</a> repositories captured on 16 November 2026. Different repositories have different levels of development and qualification: appearing here does not imply a release or complete functionality. GitHub remains the live source for additions and current status.</p>
</section>
<section class="projects-endnote"><p>Not every source repository is a finished application. For published, installable Linux packages, use the <a href="https://infiltrator-projects.github.io/Infiltrator-Repository/">Software Centre</a>.</p></section>
<script>
(() => {
  const field = document.getElementById('repository-search');
  const clear = document.getElementById('repository-clear');
  const empty = document.getElementById('repository-empty');
  const jumpNav = document.querySelector('.repository-families-nav');
  const entries = Array.from(document.querySelectorAll('.repo-entry'));
  const groups = Array.from(document.querySelectorAll('[data-repo-group]'));
  const status = document.getElementById('repository-count');
  if (!field || !clear || !empty || !jumpNav || !status) return;
  const index = groups.map(group => ({
    group,
    label: (group.querySelector('.repo-group-heading h3, .repo-group-text strong')?.textContent || '').toLocaleLowerCase(),
    entries: Array.from(group.querySelectorAll('.repo-entry'))
  }));
  function update() {
    const query = field.value.trim().toLocaleLowerCase();
    let matches = 0;
    for (const family of index) {
      const familyMatch = query.length > 0 && family.label.includes(query);
      let visible = 0;
      for (const entry of family.entries) {
        const searchable = entry.textContent.toLocaleLowerCase() + ' ' + entry.dataset.repoName.toLocaleLowerCase();
        const show = !query || familyMatch || searchable.includes(query);
        entry.hidden = !show;
        if (show) { ++matches; ++visible; }
      }
      family.group.hidden = visible === 0;
      if (family.group.tagName === 'DETAILS') family.group.open = Boolean(query && visible);
    }
    clear.hidden = !query;
    empty.hidden = !query || matches !== 0;
    jumpNav.hidden = Boolean(query);
    status.textContent = query ? matches + ' matching repositories' : entries.length + ' repositories';
  }
  field.addEventListener('input', update);
  clear.addEventListener('click', () => { field.value = ''; update(); field.focus(); });
  update();
})();
</script>
)CATALOG" + page_end("104 public repositories · snapshot 16 November 2026.");
}

static string render_workbench() {
    return page_start("Workbench", "How the Infiltrator software, operating-system components, filesystems, diagnostic applications and creative work fit together, with an analogue ReVox workshop.", "Workbench", "workbench-page") + R"WORKBENCH(
<section class="page-intro workbench-intro">
  <p class="eyebrow">Workbench · Engineering journal</p>
  <h1>Inside the engineering.</h1>
  <p class="lead">How should a filesystem recover after a crash? Where does a boot manager's responsibility end? What makes a vehicle response trustworthy? These engineering notes follow the decisions, experiments and evidence behind the work.</p>
  <p class="workbench-reader-note">Start with the question that led to a project, then follow its architecture, limits and evidence. Repository links lead from each explanation to its implementation.</p>
  <nav class="workbench-directory" id="workbench-topics" aria-label="Workbench sections">
    <a href="#platform"><small>01 · Foundation</small><strong>InfiltratorOS</strong><span>Boot, kernel, Libraries and native file handling</span></a>
    <a href="#infiltratorfs"><small>02 · Storage</small><strong>InfiltratorFS</strong><span>Format, recovery and integrity</span></a>
    <a href="#software"><small>03 · Applications</small><strong>Desktop tools</strong><span>Monitor, defragment, calculate and manage</span></a>
    <a href="#link"><small>04 · Vehicles</small><strong>Link diagnostics</strong><span>Protocols and manufacturer knowledge</span></a>
    <a href="#creative"><small>05 · Simulation</small><strong>Games & creative</strong><span>City building, racing and character stories</span></a>
    <a href="#revox-b77"><small>06 · Audio</small><strong>ReVox B77 Mk II</strong><span>Reels, dbx processing and Technics amplification</span></a>
  </nav>
  <p class="workbench-index-link"><a href="projects.html">Looking for source code? Browse Projects <span aria-hidden="true">↗</span></a></p>
</section>

<section class="workbench-progress" id="current-work" aria-labelledby="current-work-title">
  <header><div><p class="eyebrow">Development snapshot · 17 November 2026</p><h2 id="current-work-title">What I'm working on.</h2><p>Recent changes in the actual repositories. Each entry links to a specific commit; it records work undertaken, not a declaration that the whole project is finished.</p></div><a href="https://github.com/orgs/Infiltrator-Projects/repositories">See the repositories ↗</a></header>
  <ol class="workbench-progress-list">
    <li><time datetime="2026-11-17">17 Nov</time><div><strong>Boot · Secure Boot updater</strong><p>Updated the native installer workflow for an existing Secure Boot signing identity.</p><a href="https://github.com/Infiltrator-Projects/Boot/commit/72f423eba57ca6da5132236db065f6b165fd88c8">Inspect commit ↗</a></div></li>
    <li><time datetime="2026-11-14">14 Nov</time><div><strong>InfiltratorFS · Common V2 contract</strong><p>Aligned the filesystem integration with the corrected Common V2 host contract.</p><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/commit/9e17f257d7f5a042dbe8cd64321ed59ca61a138e">Inspect commit ↗</a></div></li>
    <li><time datetime="2026-11-12">12 Nov</time><div><strong>Calendar · Release qualification</strong><p>Connected the native provider checker to Cinnamon release qualification.</p><a href="https://github.com/Infiltrator-Projects/Calendar/commit/50b4cfd105ad73fcfda93a368289a974cedb76fd">Inspect commit ↗</a></div></li>
  </ol>
  <p class="workbench-progress-caveat">Dated snapshot; the linked repositories are the source of current status and newer changes.</p>
</section>
<section id="platform" class="project-section">
  <header class="section-heading"><p class="eyebrow">Foundation · Operating system</p><h2>Building an operating environment, layer by layer.</h2><p class="section-deck">A collection of individually owned components intended to work as one operating environment.</p></header>
  <div class="project-content">
    <p>InfiltratorOS integrates components maintained in separate repositories. Its current deliverable is an amd64 integration package for a Debian/Linux Mint environment. It establishes the layout, shared resources and installation sequence for independently maintained components. A completed standalone operating system and graphical Setup installer are still future milestones.</p>
    <p>The architectural split matters. <strong>Libraries</strong> supplies portable C11 mechanisms and public contracts. <strong>Boot</strong> handles the firmware-stage selection of operating systems and uses filesystem-specific Common V2 shared objects. The separate <strong>Kernel</strong> repository tracks the native-kernel development path, with a qualified QEMU milestone but no claim that the resulting kernel is ready for everyday hardware. <strong>Files</strong> is a native file manager, and <strong>Software</strong> owns application discovery, package operations and update workflows.</p>
    <div class="workbench-context"><strong>One component, one responsibility.</strong><p>Ownership stays explicit: a filesystem repository defines the behaviour of its format, the common layer defines the interface, and the loader or application consumes the capability. The old central Filesystems repository is transitional while the engines move to their independent homes.</p></div>
    <div class="engineering-map" aria-label="The responsibilities within InfiltratorOS">
      <div><span class="engineering-step">01 · Contracts</span><h3>Libraries</h3><p>Portable C11 facilities and the Common V2 interfaces used by format engines and their clients.</p></div>
      <div><span class="engineering-step">02 · System layers</span><h3>Boot, Kernel and InfiltratorOS</h3><p>Separate boot, kernel and installation responsibilities. Passing one component test does not qualify the whole operating environment.</p></div>
      <div><span class="engineering-step">03 · User tools</span><h3>Files and Software</h3><p>File operations, package discovery and updates belong in applications rather than firmware or filesystem code.</p></div>
    </div>
    <p class="engineering-throughline"><strong>Follow one responsibility:</strong> a filesystem engine defines the on-disk rules; a qualified Boot reader needs to understand that format during startup; Files handles file operations in the running environment. Kernel development follows its own qualification path. Sharing an interface does not make those components interchangeable.</p>
    <p class="workbench-source-link"><a href="projects.html#group-foundations">See the operating-system source repositories <span aria-hidden="true">↗</span></a></p>
    <p class="project-boundary">Builds, emulated boots and component tests are evidence of particular milestones, not proof that every subsystem is ready for a general-purpose installation. Current qualification is maintained in each repository.</p>
  </div>
<a class="chapter-backlink" href="#workbench-topics">↑ Back to Workbench topics</a>
</section>
<section id="infiltratorfs" class="project-section">
  <header class="section-heading"><p class="eyebrow">Storage · Original format</p><h2>InfiltratorFS.</h2><p class="section-deck">What should survive a crash, and how can the filesystem prove what survived?</p></header>
  <div class="project-content">
    <p>InfiltratorFS is a clean-sheet filesystem with its own persistent format, transactions, allocation model and recovery rules. The filesystem itself owns these decisions; Linux, Windows and future systems are adapters to its behaviour rather than alternative implementations of the format.</p>
    <p>Copy-on-write transactions publish new generations without immediately destroying earlier state. Checkpoints, snapshots and reflinks provide ways to retain or share data; sparse files and extent metadata make allocation explicit. CRC64 and SHA-256 form separate metadata and logical-data integrity mechanisms. Inspection, checking and scrub tools expose verification rather than assuming that a successful write call settles the question.</p>
    <dl class="inline-facts"><div><dt>Current on-disk format</dt><dd>0.19</dd></div><div><dt>Transactions</dt><dd>Generation-based CoW</dd></div><div><dt>Qualification</dt><dd>Platform-specific evidence</dd></div></dl>
    <p>The Linux path includes a native VFS driver and the project has a driverless Windows Explorer access bridge. The experimental Common V2 module shares the portable format engine with a hosted implementation and a read-only BOOT profile. A production-qualified firmware installation and a native Windows filesystem driver are not yet established.</p>
    <div class="resource-links"><a href="https://github.com/Infiltrator-Projects/InfiltratorFS">Source repository ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/QUALIFICATION.md">Qualification evidence ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/ROADMAP.md">Roadmap ↗</a><a href="projects.html#group-filesystem-modules">Other filesystem engines ↗</a></div>
    <details class="technical-detail"><summary>More on the filesystem's design <span aria-hidden="true">+</span></summary>
      <div class="engineering-notes"><article><h3>Keep identity stable</h3><p>Objects carry 128-bit identifiers and the namespace supports UTF-8 names. The transaction and extent model separates logical identity from where bytes happen to be stored.</p></article><article><h3>Share without guessing</h3><p>Reflinks, snapshots and sparse allocation require explicit rules about ownership, history and future writes. A shared extent is a deliberate state, not an accidental consequence of copying.</p></article><article><h3>Test failures as well as success</h3><p>Disposable-image tests can inject I/O interruptions. Mounted Linux performance, writeback behaviour, recovery and the firmware bridge have distinct qualification boundaries.</p></article></div>
    </details>
  </div>
<a class="chapter-backlink" href="#workbench-topics">↑ Back to Workbench topics</a>
</section>
<section id="software" class="project-section">
  <header class="section-heading"><p class="eyebrow">Applications · Practical tools</p><h2>Software that does a specific job.</h2><p class="section-deck">Not every capability belongs inside the operating system itself.</p></header>
  <div class="project-content">
    <p><strong>System Monitor</strong> examines the running machine through native system interfaces. <strong>Defragmenter</strong> inspects offline filesystem allocation and restricts write operations to explicitly qualified formats. <strong>Calendar</strong> is a Cinnamon panel clock and calendar that can use 31 independently owned calendar systems and 23 clock modules, rather than hard-coding all those rules into one desktop application.</p>
    <p><strong>Calculator</strong>, <strong>Files</strong> and <strong>Software</strong> address different daily tasks: calculation, file operations and package management. <strong>RunnerScope</strong> follows self-hosted GitHub Actions runners; <strong>PowerShell-HAL</strong> inserts generated commands for inspection rather than executing them automatically. They can share neutral facilities from Libraries while retaining their own product behaviour and release evidence.</p>
    <div class="engineering-note"><p class="eyebrow">The important boundary</p><h3>One application should not secretly reimplement another engine.</h3><p>Defragmenter can analyse an allocation map, but a filesystem module must own its actual format and writes. Calendar owns the user interface, while the separate time and calendar modules own their rules. The native tools can share low-level facilities without becoming a single inseparable program.</p><p>That separation is also a testing boundary: a successful interface build is not evidence that an unfamiliar filesystem's write path is qualified.</p></div>
    <p class="workbench-source-link"><a href="projects.html#group-applications">Browse application source <span aria-hidden="true">↗</span></a> · <a href="projects.html#group-calendar-modules">Calendar modules <span aria-hidden="true">↗</span></a> · <a href="projects.html#group-clock-modules">Clock modules <span aria-hidden="true">↗</span></a></p>
    <p class="project-boundary">The project directory lists every module separately, but the technical qualification remains with the application and its engine. Published packages and source repositories are separate destinations.</p>
  </div>
<a class="chapter-backlink" href="#workbench-topics">↑ Back to Workbench topics</a>
</section>
<section id="link" class="project-section">
  <header class="section-heading"><p class="eyebrow">Vehicle diagnostics · Shared engine</p><h2>Link and its manufacturer families.</h2><p class="section-deck">An adapter returns bytes. A diagnostic product must establish what they actually mean.</p></header>
  <div class="project-content">
    <p><strong>Link</strong> owns the common C11 engine for transport, framing, standard OBD-II, UDS sequencing, telemetry and evidence handling. The manufacturer repositories then provide vehicle identity, module definitions and knowledge specific to Mercedes-Benz, Jaguar, BMW, Audi and Ford. This avoids having five incompatible copies of the same underlying diagnostic machinery.</p>
    <p><strong>MBLink</strong> uses <a class="inline-link" href="garage.html#e250-cdi">my C207 E 250 CDI</a> as a real development and evidence vehicle, but the Mercedes-Benz product is not restricted to that model. Discover targets where enabled concentrate on read-only ECU identification, inventory and capture. An unknown value remains unknown until documentation or reproducible evidence supports its interpretation; a newly recognised module does not automatically authorise a new kind of diagnostic request.</p>
    <div class="engineering-process" aria-label="Read-only vehicle diagnostics workflow">
      <div><span class="engineering-step">01</span><h3>Identify</h3><p>Establish the adapter, vehicle identity and the protocols the target actually supports.</p></div>
      <div><span class="engineering-step">02</span><h3>Collect</h3><p>Use supported read-only requests to record ECU identity, inventory and diagnostic responses.</p></div>
      <div><span class="engineering-step">03</span><h3>Interpret</h3><p>Apply manufacturer-specific definitions only where evidence supports them; preserve unknown responses as unknown.</p></div>
    </div>
    <p class="workbench-source-link"><a href="projects.html#group-diagnostics">Explore Link and manufacturer repositories <span aria-hidden="true">↗</span></a></p>
    <details class="technical-detail"><summary>What Discover is for <span aria-hidden="true">+</span></summary><p>The workflow begins with connection and identification, continues through supported module discovery and read-only data requests, and retains captures for interpretation. Reusable interrogation mechanisms belong to Link; Mercedes, Jaguar, BMW, Audi or Ford-specific meanings stay with the corresponding product. This boundary matters whenever a car produces a response the software cannot yet explain.</p></details>
  </div>
<a class="chapter-backlink" href="#workbench-topics">↑ Back to Workbench topics</a>
</section>
<section id="creative" class="project-section">
  <header class="section-heading"><p class="eyebrow">Simulation · Creative tools</p><h2>Games are engineering projects too.</h2><p class="section-deck">Different user experiences, but the same need for coherent models and clear cause and effect.</p></header>
  <div class="project-content">
    <p><strong>Egypt</strong> is an original city-building simulation centred on the Nile, work, movement, production and visibly functioning settlements; its repository documents an early native executable shell rather than a completed game. <strong>Backyard Racer</strong> develops the garage, classifieds, car-building and racing loop as one simulation, with vehicle state and progression owned by the game. <strong>Character Profiler</strong> is a separate iPhone story-bible application that treats characters, relationships and portable archives as durable data.</p>
    <p>These are intentionally separate projects, not hidden modules of InfiltratorOS. Their designs draw on the strengths of older software without claiming to be a reproduction of someone else's program.</p>
    <div class="engineering-note"><p class="eyebrow">The design question</p><h3>What must the simulation remember?</h3><p>In Egypt, work and materials must remain coherent as settlements develop. In Backyard Racer, a vehicle's condition and changes have to survive beyond a single race. In Character Profiler, characters and relationships need a durable, portable archive. These projects have different interfaces, but each depends on consistent state underneath.</p></div>
    <p class="workbench-source-link"><a href="projects.html#group-creative">Find the creative project sources <span aria-hidden="true">↗</span></a></p>
  </div>
<a class="chapter-backlink" href="#workbench-topics">↑ Back to Workbench topics</a>
</section>
<section id="revox-b77" class="project-section analog-section">
  <header class="section-heading"><p class="eyebrow">Analogue · Real hardware</p><h2>ReVox B77 Mk II, dbx 224 and Technics SU-Z400.</h2><p class="section-deck">A different way of seeing an engineering system work.</p></header>
  <div class="analog-feature-body">
    <figure><img src="assets/photos/revox-b77-mkii-dbx224-technics-su-z400.webp" width="1200" height="900" loading="lazy" decoding="async" alt="Redrawn ReVox B77 Mk II with BASF reel and silver metal reel, above a dbx 224 and Technics SU-Z400 integrated amplifier"><figcaption>A clean redraw based on my own recorder, metal reel, dbx unit and Technics amplifier, not an original photograph.</figcaption></figure>
    <div><p class="eyebrow">The equipment</p><h3>Open-reel recording, noise reduction and amplification.</h3>
      <p>The ReVox B77 Mk II is a two-channel open-reel tape recorder. Its moving reels, transport controls and VU meters make the recording process visible in a way that software often conceals. There are separate left and right line inputs and outputs on the back of my machine.</p>
      <p>The dbx Model 224 is an external Type II encode/decode noise-reduction unit. It is part of the recording and playback signal path, not a replacement for the recorder's mechanical transport. The recorder and noise-reduction processor have different jobs, but their settings and connections matter together.</p>
      <p>The Technics SU-Z400 is my stereo integrated amplifier, with front-panel bass, treble, balance and volume sliders. It sits beneath the dbx unit in my setup and represents the listening and amplification side of the equipment.</p>
      <p>The redraw follows my photographs: a BASF reel and the distinctive silver metal reel on the B77, its illuminated meters, the dbx 224 below and the Technics amplifier beneath that. It is a depiction of the hardware, not a claim about the exact current cable routing.</p>
      <dl class="analog-specs"><div><dt>Recorder</dt><dd>ReVox B77 Mk II</dd></div><div><dt>Noise reduction</dt><dd>dbx 224 · Type II</dd></div><div><dt>Amplifier</dt><dd>Technics SU-Z400</dd></div><div><dt>Reels</dt><dd>BASF and silver metal reel</dd></div><div><dt>Deck connections</dt><dd>Two-channel line input and output</dd></div></dl>
    </div>
  </div>
<a class="chapter-backlink" href="#workbench-topics">↑ Back to Workbench topics</a>
</section>
<section id="repository" class="workbench-pathways" aria-labelledby="workbench-pathways-title"><div><p class="eyebrow">From the notes to the code</p><h2 id="workbench-pathways-title">What do you want to inspect?</h2><p>Workbench explains why and how. Projects gives you the source. The Software Centre is for the packages actually published for installation.</p></div><div class="workbench-pathway-links"><a href="projects.html">Find GitHub repositories <span aria-hidden="true">↗</span></a><a href="https://infiltrator-projects.github.io/Infiltrator-Repository/">Find installable packages <span aria-hidden="true">↗</span></a></div></section>
<section id="principles" class="closing-note"><h2>Verify the work,<br>not just the build.</h2><p>A successful compilation proves a particular build. A deterministic unit test proves the cases it exercised. A boot in QEMU, a mount on test media and a real vehicle capture each establish different evidence. The repositories record those boundaries, and the unfinished work is part of the story.</p></section>
)WORKBENCH" + page_end("The best or nothing.");
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
    o << R"(<section class="garage-chapter" id="vehicle-history" aria-labelledby="vehicle-history-title"><header class="chapter-heading"><p class="eyebrow">The earlier cars · newest to oldest</p><h2 id="vehicle-history-title">A garage history, in order.</h2><p>This is a record of vehicles I owned, not a timeline of when I bought them. They are ordered by known manufacture year where recorded; otherwise the model-era label explains the position. The photos are identified individually as original, supplied reference material or illustrative reconstructions.</p></header>)";
    const auto cars = historical_cars();
    o << R"(<nav class="garage-jump" id="garage-index" aria-label="Jump to a previous car"><div class="garage-jump-heading"><strong>Find a car</strong><span>Thirteen previous vehicles · newest to oldest</span></div><div class="garage-jump-links">)";
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
              << R"(<div class="vehicle-photo-pair" aria-label="Photographs of the 500SEC AMG"><figure><a class="garage-photo-link" href="assets/photos/500sec-amg-side.jpg" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/500sec-amg-three-quarter.webp" width="1021" height="409" loading="lazy" decoding="async" alt="Smoke Silver 500SEC AMG photographed from the side, showing the AMG bodywork and five-spoke wheels"></a><figcaption>500SEC AMG · three-quarter side view</figcaption></figure><figure><a class="garage-photo-link" href="assets/photos/500sec-amg-front.jpg" aria-label="Open full-sized vehicle photograph"><img src="assets/photos/500sec-amg-front.webp" width="520" height="400" loading="lazy" decoding="async" alt="Front of the Smoke Silver 500SEC AMG with Western Australian registration 1ARK884"></a><figcaption>500SEC AMG · 1ARK884</figcaption></figure></div>)"
              << vehicle_record(sec) << R"(<a class="vehicle-index-return" href="#garage-index">↑ Back to the vehicle index</a></article>)";
        } else {
            o << historical_car(car);
        }
    }
    o << R"(</div></section><section class="destination-strip"><div><p class="eyebrow">The vehicle and the code</p><h2>From driving to diagnostics.</h2><p>The current E 250 CDI is also my MBLink development and evidence vehicle. The diagnostic tools are documented in the Workbench; the Garage preserves the cars themselves and what is known about them.</p></div><a class="text-link" href="workbench.html#link">How MBLink fits together <span aria-hidden="true">→</span></a></section>)";
    return o.str() + page_end("A personal garage history.");
}

static string render_about() {
    return page_start("About", "The history of Shannon Smith and a personal website begun in 1993, with the Mercedes-Benz 450SEL 6.9, software work and other interests.", "About", "about-page") + R"ABOUT(
<section class="page-intro">
  <p class="eyebrow">About · Online since 1993</p>
  <h1>A place of my own.</h1>
  <p class="lead">I'm Shannon Smith, in Mooroopna, Victoria. This has been my personal corner of the internet since 1993—a place for the cars I've owned, software I've built, open-reel audio, watches and the machinery I enjoy understanding.</p>
  <nav class="section-nav" aria-label="About sections"><a href="#the-site">The site's history</a><a href="#professional-background">ICT career</a><a href="#interests">Other interests</a><a href="#daytona">The Daytona</a><a href="#how-it-fits">The common thread</a><a href="#contact">Get in touch</a><a href="#principles">A principle</a></nav>
</section>
<section id="the-site" class="about-history">
  <header class="section-heading"><p class="eyebrow">The original site</p><h2>From one Mercedes<br>to a lifetime of projects.</h2><p class="section-deck">An ongoing personal record, rather than a succession of unrelated websites.</p></header>
  <p class="about-section-intro">The site began with one particular car and grew to record other interests over time. Its history is the story of that gradual expansion, not a claim that every project dates back to the beginning.</p>
  <div class="timeline">
    <article class="event"><span class="era">1993</span><div><h3>The 450SEL 6.9 and the Infiltrator website.</h3><p>The original subject was my Mercedes-Benz W116 450SEL 6.9. Its engineering, specification and character gave the site something worth documenting. That beginning matters: the web page was personal first, and it still is. The <a href="garage.html#450sel-69">Garage</a> retains that vehicle alongside the others I have owned.</p></div></article>
    <article class="event"><span class="era">Beta-testing years</span><div><h3>Windows, Solaris and games.</h3><p>I was involved in voluntary beta testing of Microsoft Windows ME, Solaris and EA game projects. The Windows ME work included a Beta Tester of the Month award. Testing meant spending time with unfinished software and reporting what actually happened—not merely judging its intended feature list.</p></div></article>
    <article class="event"><span class="era">The software workshop</span><div><h3>From applications to their foundations.</h3><p>The software work expanded to include System Monitor, Defragmenter, Calendar, Calculator, vehicle diagnostics and InfiltratorFS. More recently the structure has grown into separately owned operating-system components, shared libraries, filesystem engines and specialist modules. These are different layers of work, not dozens of names for the same application. <a href="workbench.html#platform">The Workbench explains the architectural boundaries</a>, while <a href="projects.html#group-foundations">Projects lists the separate source repositories</a>.</p></div></article>
    <article class="event"><span class="era">Today</span><div><h3>One website, several kinds of evidence.</h3><p>The Mercedes E 250 CDI is both a car I drive and an evidence vehicle for MBLink development. My ReVox B77 Mk II, dbx 224 and Technics SU-Z400 are another kind of engineering interest: hands-on equipment that does not need to become a software project to deserve attention.</p></div></article>
  </div>
</section>
<section id="professional-background" class="about-professional" aria-labelledby="professional-background-title">
  <header class="section-heading"><p class="eyebrow">Professional background · ICT</p><h2 id="professional-background-title">Keeping systems working<br>for real people.</h2><p class="section-deck">More than two decades of ICT management, infrastructure, support and technical delivery, alongside the software I develop independently.</p></header>
  <p class="about-professional-intro">My work has taken me through corporate contact centres, health, community services and education. I've led ICT teams, administered infrastructure and supported people who simply need their technology to work. It's a different kind of engineering from designing a filesystem, but the same attention to reliability, practical outcomes and understanding the system applies.</p>
  <div class="professional-history" aria-label="Selected professional ICT roles">
    <article class="professional-role"><span class="professional-period">Current</span><div><h3>ICT Systems Administrator</h3><p class="professional-organisation">St Augustine's College · Kyabram</p><p>Supporting school ICT infrastructure, systems and everyday technology for teaching and administration.</p></div></article>
    <article class="professional-role"><span class="professional-period">Previously</span><div><h3>ICT Manager</h3><p class="professional-organisation">Rumbalara Aboriginal Co-operative · Mooroopna</p><p>Led day-to-day ICT operations and improvement work for a community-controlled organisation, combining management, infrastructure, suppliers, policy and hands-on support.</p></div></article>
    <article class="professional-role"><span class="professional-period">Earlier</span><div><h3>Senior Technical Support Officer</h3><p class="professional-organisation">Goulburn Valley Health · Shepparton</p><p>Worked within a large health ICT team on servers, networks, business systems and service continuity where access, reliability and confidentiality mattered.</p></div></article>
    <article class="professional-role"><span class="professional-period">Earlier</span><div><h3>Technical Services Manager / Regional ICT Manager</h3><p class="professional-organisation">Stellar Call Centres · Melbourne</p><p>Managed multi-site ICT operations, technical teams and infrastructure projects, from day-to-day support to new deployments and system standards.</p></div></article>
    <article class="professional-role"><span class="professional-period">Foundations</span><div><h3>Network engineering and teaching</h3><p class="professional-organisation">Millennium Security · Equinox Global · Curtin University</p><p>Worked on networking and technical solutions for business and education clients in Perth, and taught international students in Curtin's Business IT stream under contract.</p></div></article>
  </div>
  <div class="professional-education"><h3>Training and qualifications.</h3><p>Diploma of Information Technology (Advanced Stream), Certificates IV and III in Information Technology, Microsoft MCSE + Internet and ITIL V3. The qualifications sit alongside years spent administering systems, managing services and working directly with users.</p></div>
</section>
<section id="interests" class="about-interests">
  <header class="section-heading"><p class="eyebrow">Away from the code</p><h2>Machines, music<br>and other interests.</h2><p class="section-deck">The website is wider than the software projects.</p></header>
  <div class="project-content">
    <article class="interest-entry"><h3>Open-reel tape.</h3><p>My ReVox B77 Mk II, dbx 224 and Technics SU-Z400 bring together tape transport, analogue recording, Type II noise reduction and stereo amplification. The silver metal reel is part of the equipment I enjoy as much as the sound and the engineering behind it. Their own equipment story belongs in the <a href="workbench.html#revox-b77">Workbench</a>.</p></article>
    <article class="interest-entry"><h3>Films, games and cars.</h3><p>Music, films and games are part of the picture alongside the cars. Some ideas have also led to my own simulation projects, but enjoying those subjects came first. The <a href="garage.html">Garage</a> contains the record of the vehicles I have owned.</p></article>
  </div>
  <article class="watch-feature" id="daytona" aria-labelledby="daytona-title">
    <div class="watch-feature-copy">
      <p class="eyebrow">Mechanical timekeeping</p>
      <h3 id="daytona-title">My reverse-panda Daytona.</h3>
      <p>The Cosmograph Daytona 126500LN in Oystersteel has a black dial with contrasting pale chronograph registers and a black tachymeter bezel. The reverse-panda layout is the version I wanted to show here.</p>
      <p>The attraction is familiar: mechanical precision, carefully finished metal and an instrument whose design reflects its purpose. It's another form of engineering that I enjoy living with, rather than just looking at on a screen.</p>
    </div>
    <figure>
      <img src="assets/photos/daytona-reverse-panda.webp" width="220" height="220" loading="lazy" decoding="async" alt="Close-up of an Oystersteel Daytona with black dial, silver chronograph subdials, black tachymeter bezel and stainless-steel bracelet">
      <figcaption>My reverse-panda Daytona · Image supplied for this website.</figcaption>
    </figure>
  </article>
</section>
<section id="how-it-fits" class="about-thread">
  <header class="section-heading"><p class="eyebrow">The common thread</p><h2>Different machines.<br>Same curiosity.</h2><p class="section-deck">They don't have to be the same kind of project to belong here.</p></header>
  <div class="about-thread-body">
    <p>A filesystem raises questions about what can survive a failure. Vehicle diagnostics asks what a machine is actually reporting. An open-reel deck makes the signal path visible, and a mechanical watch is precision engineering small enough to wear. The interesting part is understanding how each behaves, not just knowing its specifications.</p>
    <p>Some interests become code; others remain cars, music or physical equipment. I have kept them together on this site because one pursuit need not replace another. The original 450SEL 6.9 story still belongs here alongside the software and everything that followed.</p>
  </div>
</section>

<section class="about-contact" id="contact" aria-labelledby="about-contact-title">
  <div><p class="eyebrow">Talk about the work</p><h2 id="about-contact-title">Questions, code, and collaboration.</h2></div>
  <div><p>Much of the work here is public because I want it to be inspectable. If you're interested in a particular project, the relevant GitHub repository is the best place for technical questions, issues and discussions. My public GitHub profile is another way to find what I'm working on.</p>
    <div class="about-contact-links"><a href="https://github.com/The-First-Infiltrator">My GitHub profile ↗</a><a href="projects.html#directory">Find the right repository ↗</a></div>
  </div>
</section>
<section id="principles" class="quote-section"><p class="eyebrow">A personal principle</p><blockquote class="quote">“I am free, no matter what rules surround me. If I find them tolerable, I tolerate them; if I find them too obnoxious, I break them. I am free because I know that I alone am morally responsible for everything I do.”<cite>— Robert A. Heinlein</cite></blockquote></section>
)ABOUT" + page_end("Since 1993. Still here.");
}

static string render_download() {
    const string url = "https://github.com/Infiltrator-Projects/ssmithnet.net/releases/download/model-11-sandy-v1.0.0/Model_11_Sandy_v.1.0.0.zip";
    auto page = page_start("Model 11 Sandy v1.0.0", "Download Model 11 Sandy v1.0.0.", "Download");
    const auto head_end = page.find("</head>");
    page.insert(head_end, "<meta name=\"robots\" content=\"noindex,follow\">");
    return page + "<section class=\"page-intro\"><div class=\"kicker\">Download</div><h1>Model 11 Sandy<br> v1.0.0</h1><p class=\"lead\">A 365 MB ZIP archive. Select the link below to start the download.</p><p><a class=\"text-link\" href=\"" + esc(url) + "\">Download the ZIP now <span aria-hidden=\"true\">↗</span></a></p></section>" + page_end("Direct download");
}

int main(int argc, char** argv) {
    try {
        if (argc > 2) throw std::runtime_error("usage: sitegen [output-directory]");
        const fs::path root = argc == 2 ? fs::absolute(argv[1]) : fs::current_path();
        write(root / "index.html", render_home());
        write(root / "workbench.html", render_workbench());
        write(root / "projects.html", render_projects());
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
