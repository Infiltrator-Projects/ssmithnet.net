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
    string tag, title, make, model, series, powertrain, description;
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
        "<link rel=\"icon\" type=\"image/svg+xml\" href=\"" + esc(prefix + "assets/favicon.svg") + "\">"
        "</head><body class=\"" + esc(body_class) + "\">" + infiltrator_web::navigation(active, prefix) + "<main id=\"main\">";
}

static string page_end(const string& right) {
    return "</main>" + infiltrator_web::footer(right);
}

static string historical_car(const Car& c) {
    std::ostringstream o;
    o << "<article class=\"garage-entry garage-vehicle\"><div class=\"vehicle-heading\"><div class=\"tag\">" << esc(c.tag)
      << "</div><h3>" << esc(c.title) << "</h3><p class=\"vehicle-description\">" << esc(c.description)
      << "</p></div><dl class=\"car-facts\">"
      << "<div><dt>Make</dt><dd>" << esc(c.make) << "</dd></div>"
      << "<div><dt>Model</dt><dd>" << esc(c.model) << "</dd></div>"
      << "<div><dt>Chassis / Series</dt><dd>" << esc(c.series) << "</dd></div>"
      << "<div><dt>Powertrain</dt><dd>" << esc(c.powertrain) << "</dd></div>"
      << "</dl></article>";
    return o.str();
}

static vector<Car> historical_cars() {
    return {
        {"W116","450SEL 6.9","Mercedes-Benz","450SEL 6.9","116.036","M100 · 6.8 L V8","The car behind the original Infiltrator 6.9 page and the earliest chapter of the domain."},
        {"W116","280SE","Mercedes-Benz","280SE","116.024","M110 · 2.8 L I6","The six-cylinder W116 in my garage history, alongside the 450SEL 6.9."},
        {"Ford · Tickford LPG","EL Falcon","Ford","EL Falcon","EL","Petrol / LPG · Tickford","An earlier Falcon in the Australian chapter, with Tickford-installed LPG."},
        {"Ford","XF Falcon","Ford","XF Falcon","XF","Petrol","An XF Falcon among the Australian Fords I have owned."},
        {"Ford · BA · V8 · LPG","2003 Fairmont","Ford","Fairmont","BA","5.4 L V8 · Petrol / LPG","A BA Fairmont with the 5.4-litre V8 and dual-fuel LPG."},
        {"Holden","LC Torana","Holden","Torana","LC","1.1 L I4","The little 1.1-litre four-cylinder version I had."},
        {"Ford · BA · V8 · LPG","2004 LTD","Ford","LTD","BA","5.4 L V8 · Petrol / LPG","The long-wheelbase BA Ford: a 2004 LTD with the 5.4-litre V8 and dual-fuel LPG."},
        {"Ford · AU Series II · LPG","Fairmont","Ford","Fairmont","AU Series II","4.0 L · Petrol / LPG","An AU Series II Fairmont with the 4.0-litre engine and dual-fuel LPG."},
        {"Land Rover","Discovery 2 TDI","Land Rover","Discovery 2","Discovery II","TDI diesel","The Discovery II brought a four-wheel-drive diesel into a history otherwise dominated by saloons and coupés."},
        {"Peugeot","504 Diesel","Peugeot","504","504","Diesel · waste motor oil","An old-school diesel that I ran on waste motor oil. The exact variant remains deliberately unspecified."}
    };
}

static string render_home() {
    return page_start("Shannon Smith", "Shannon Smith's personal website: Infiltrator software, a Mercedes-Benz E-Class and a garage history stretching back to the 450SEL 6.9.", "Home", "home-page") + R"(
<section class="home-intro" aria-labelledby="home-title"><p class="eyebrow">Mooroopna, Victoria · A personal website</p><h1 id="home-title">Shannon Smith.</h1><div class="home-intro-bottom"><p class="lead">I build software, enjoy machinery, and like to understand how things work.</p><a class="text-link" href="about.html">A little about me <span aria-hidden="true">→</span></a></div></section>
<section class="home-feature" aria-labelledby="home-workbench"><div class="feature-heading"><p class="eyebrow">On the workbench</p><h2 id="home-workbench"><a href="workbench.html#infiltratorfs">InfiltratorFS</a></h2><p class="feature-subtitle">A filesystem, built from first principles.</p></div><div class="feature-content"><p>What survives a crash? How should old versions be retained? InfiltratorFS follows those questions into the on-disk format, recovery tools and a native Linux driver.</p><ul class="feature-notes"><li>Copy-on-write generations</li><li>Checksummed data &amp; metadata</li><li>Native Linux integration</li></ul><a class="text-link" href="workbench.html">Inside the Workbench <span aria-hidden="true">→</span></a></div></section>
<section class="home-feature" aria-labelledby="home-garage"><div class="feature-heading"><p class="eyebrow">In the garage · 2011</p><h2 id="home-garage"><a href="garage.html">E 250 CDI</a></h2><p class="feature-subtitle">Mercedes-Benz BlueEFFICIENCY Coupé.</p></div><div class="feature-content"><p>The C207 in the garage today, finished in Obsidian Black. It is also the development vehicle for MBLink, where the software and automotive interests meet.</p><dl class="inline-facts"><div><dt>Series</dt><dd>C207</dd></div><div><dt>Engine</dt><dd>OM651 diesel</dd></div><div><dt>Original delivery</dt><dd>Australia</dd></div></dl><a class="text-link" href="garage.html">The car &amp; the garage history <span aria-hidden="true">→</span></a></div></section>
<section class="home-closing"><p>This address began with a Mercedes-Benz 450SEL 6.9 in the 1990s. Cars, code, open-reel tape and watches have all found a place here since.</p><a class="text-link" href="about.html#the-site">The story of ssmithnet.net <span aria-hidden="true">→</span></a></section>
)" + page_end("A personal corner of the internet, since the 1990s.");
}

static string render_workbench() {
    return page_start("Workbench", "InfiltratorFS, native desktop applications and the Link vehicle-diagnostics family. Software projects by Shannon Smith.", "Workbench", "workbench-page") + R"(
<section class="page-intro"><p class="eyebrow">Workbench</p><h1>Software, from<br> first principles.</h1><p class="lead">Filesystems, native applications and vehicle diagnostics. Personal projects, begun in 2016 and still being built.</p><nav class="section-nav" aria-label="Workbench sections"><a href="#infiltratorfs">Storage</a><a href="#software">Desktop software</a><a href="#link">Vehicle diagnostics</a></nav></section>
<section id="infiltratorfs" class="project-section"><header class="section-heading"><p class="eyebrow">Storage</p><h2>InfiltratorFS</h2><p class="section-deck">A filesystem with its on-disk format and recovery behaviour at the centre of the design.</p></header>
<div class="project-content"><p>The Linux path is a native kernel driver. The project also includes formatters, inspection and scrub tools, and Windows access through a driverless Explorer bridge. A native Windows filesystem driver remains future work.</p><dl class="inline-facts"><div><dt>Format</dt><dd>0.18</dd></div><div><dt>Writes</dt><dd>Copy-on-write</dd></div><div><dt>Integrity</dt><dd>Checksummed</dd></div></dl><div class="resource-links"><a href="https://github.com/Infiltrator-Projects/InfiltratorFS">Source code ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/QUALIFICATION.md">Test evidence ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/ROADMAP.md">Feature status ↗</a></div>
<details class="technical-detail"><summary>Inside the filesystem <span aria-hidden="true">+</span></summary><div class="engineering-notes"><article><h3>Retain earlier generations</h3><p>Copy-on-write transactions create new state while retaining earlier generations. Checksummed checkpoints provide a way to select a recoverable generation after interruption.</p></article><article><h3>Share data deliberately</h3><p>Snapshots and reflinks share file data. Sparse files and paged extent metadata describe where that data lives without requiring every logical byte to occupy storage.</p></article><article><h3>Verify what comes back</h3><p>CRC64 protects metadata and SHA-256 checks logical file data. Inspection and deep-scrub tools make verification an explicit operation.</p></article></div><dl class="fact-list"><div><dt>Object identity</dt><dd>128-bit identifiers</dd></div><div><dt>Names</dt><dd>UTF-8 components up to 1,023 bytes</dd></div><div><dt>Compression</dt><dd>Native IAC1; LZ4 retained as a non-default representation</dd></div><div><dt>Linux integration</dt><dd>Native VFS driver installed through DKMS</dd></div></dl><h3>The work still ahead</h3><p>Booting a real Linux installation brings mount latency, writeback and metadata costs into focus. Those remain active engineering work. Feature support and passing tests are recorded separately from claims about everyday performance.</p></details></div></section>
<section id="software" class="project-section"><header class="section-heading"><p class="eyebrow">Native applications</p><h2>Desktop software.</h2><p class="section-deck">Tools for understanding and working with the machine.</p></header><div class="project-content"><p>System Monitor, Defragmenter, Calendar and Calculator grew alongside InfiltratorFS as after-hours and weekend projects. They share selected Common primitives, while each owns its domain rules and platform responsibilities.</p><div class="family-register"><a href="https://github.com/Infiltrator-Projects/System-Monitor"><strong>System Monitor</strong><span>Native Linux system management through direct OS and hardware interfaces.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Defragmenter"><strong>Defragmenter</strong><span>Filesystem allocation analysis, defragmentation and recovery on Linux.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calendar"><strong>Calendar</strong><span>Alternative clocks, chronology, calendars and astronomy for Cinnamon.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calculator"><strong>Calculator</strong><span>A shared calculation core with native Linux, Windows and iPhone interfaces.</span><b aria-hidden="true">↗</b></a></div></div></section>
<section id="link" class="project-section"><header class="section-heading"><p class="eyebrow">Vehicle diagnostics</p><h2>The Link family.</h2><p class="section-deck">A shared diagnostic engine. Knowledge specific to each manufacturer.</p></header><div class="project-content"><p>Link owns the common transport, OBD and UDS handling, sessions and scheduling. Mercedes, Jaguar, BMW, Audi and Ford applications add their own identities, definitions and manufacturer-specific behaviour.</p><p>MBLink is developed with <a class="inline-link" href="garage.html">my C207 E-Class</a>.</p><div class="family-register"><a href="https://github.com/Infiltrator-Projects/Link"><strong>Link</strong><span>The shared C11 diagnostic and application engine.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/MBLink"><strong>MBLink</strong><span>Mercedes-Benz · C207 / OM651 development vehicle.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/JAGLink"><strong>JAGLink</strong><span>Jaguar · X-Type / X400 knowledge.</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/BMWLink"><strong>BMWLink</strong><span>BMW</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/AUDILink"><strong>AUDILink</strong><span>Audi</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/FORDLink"><strong>FORDLink</strong><span>Ford</span><b aria-hidden="true">↗</b></a></div><details class="technical-detail"><summary>Discover &amp; the diagnostic workflow <span aria-hidden="true">+</span></summary><p>Connect the adapter, identify the vehicle and modules, match supported definitions, then select the data to request. Results stay tied to their source.</p><p>Discover is the specialist side of the family: module identification, read-only inventory, captures and evidence export. Shared interrogation mechanics belong in Link; manufacturer-specific addresses and meanings belong with the vehicle application.</p><p>An unknown response stays unknown until documentation or reproducible captures establish its meaning.</p></details></div></section>
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
    start.insert(start.find("</head>"), "<link rel=\"stylesheet\" href=\"assets/garage.css?v=garage-20261016-editorial\">");
    o << start;
    const auto current = current_mercedes();
    const auto sec = historic_mercedes();
    o << R"(<section class="garage-current" aria-labelledby="e250-title"><nav class="section-nav garage-nav" aria-label="Garage sections"><a href="#e250-cdi">Current car</a><a href="#mercedes-history">Mercedes-Benz history</a><a href="#australian-history">Fords &amp; further afield</a></nav><article class="garage-vehicle vehicle-card vehicle-card--hero" id="e250-cdi"><header class="vehicle-heading"><p class="eyebrow">In the garage · Mercedes-Benz · 2011</p><h1 id="e250-title">E 250 CDI</h1><p class="vehicle-subtitle">BlueEFFICIENCY Coupé</p><p class="vehicle-description">Obsidian Black. Black leather. A Bremen-built C207, and the Mercedes in the garage today.</p></header><div class="vehicle-overview"><dl class="hero-key-facts"><div><dt>Power</dt><dd>150 <span>kW</span></dd></div><div><dt>Torque</dt><dd>500 <span>Nm</span></dd></div><div><dt>Engine</dt><dd>2.143 <span>litre diesel</span></dd></div></dl><div class="vehicle-story"><p>A right-hand-drive, pre-facelift E-Class coupé, originally delivered to Australia. It is also the development vehicle for <a class="inline-link" href="workbench.html#link">MBLink</a>.</p><p class="equipment-preview">Panoramic roof · COMAND navigation · Heated front seats · Intelligent Light System</p></div></div>)";
    o << vehicle_record(current) << "</article></section>";
    o << R"(<section class="garage-chapter" aria-labelledby="mercedes-history"><header class="chapter-heading"><p class="eyebrow">The garage history</p><h2 id="mercedes-history">Mercedes-Benz.</h2><p>The C126, the W116s, and the 450SEL 6.9 that started this website.</p></header><div class="garage-register"><article class="garage-entry garage-vehicle vehicle-card vehicle-card--historic" id="500-sec"><div class="vehicle-heading"><div class="tag">1985 · C126 · AMG</div><h3>500SEC AMG</h3><p class="vehicle-description">A right-hand-drive 500SEC, built at Sindelfingen before its AMG conversion.</p><p class="vehicle-origin">UK delivery · Smoke Silver · Beige leather</p></div>)";
    o << vehicle_facts({{"Make", "Mercedes-Benz", ""}, {"Model", "500SEC", ""},
                        {"Chassis / Series", "126.044 · C126", ""}, {"Powertrain", "4,973 cc M117.963 V8", ""}}, "car-facts")
      << vehicle_record(sec) << "</article>";
    const auto cars = historical_cars();
    for (std::size_t i = 0; i < 2; ++i) o << historical_car(cars[i]);
    o << R"(</div></section><section class="garage-chapter" aria-labelledby="australian-history"><header class="chapter-heading"><h2 id="australian-history">Fords &amp; further afield.</h2><p>LPG Falcons and V8s, a little Torana, and two very different diesels.</p></header><div class="garage-register">)";
    for (std::size_t i = 2; i < cars.size(); ++i) o << historical_car(cars[i]);
    o << R"(</div></section><section class="destination-strip"><div><p class="eyebrow">Where it began</p><h2>The original 6.9 page.</h2><p>The W116 was the first subject of ssmithnet.net in the 1990s.</p></div><a class="text-link" href="about.html#the-site">The story of the site <span aria-hidden="true">→</span></a></section>)";
    return o.str() + page_end("A personal garage history.");
}

static string render_about() {
    std::ostringstream o;
    o << page_start("About", "About Shannon Smith: software, cars, open-reel tape, watches and a personal website that began with the Infiltrator 450SEL 6.9 page.", "About", "about-page");
    o << R"(<section class="page-intro"><p class="eyebrow">About</p><h1>A place of my own.</h1><p class="lead">I'm Shannon Smith, in Mooroopna, Victoria. Software, cars and an interest in how things work have kept this corner of the internet busy since the 1990s.</p></section>
<section class="about-interests"><header class="section-heading"><h2>Beyond the keyboard.</h2><p class="section-deck">The same interest in engineering, on a different scale.</p></header><div class="project-content"><article class="interest-entry"><h3>Open-reel tape.</h3><p>Tape decks and analogue audio sit alongside the music itself. Mechanical engineering is part of the appeal.</p></article><article class="interest-entry"><h3>On the wrist.</h3><p>A Cosmograph Daytona 126500LN in Oystersteel. Another way to enjoy a precisely made machine.</p></article><article class="interest-entry"><h3>Off the clock.</h3><p>Music, films and games have their place here too. The garage has its <a class="inline-link" href="garage.html">own collection of stories</a>.</p></article></div></section>
<section id="the-site" class="about-history"><header class="section-heading"><p class="eyebrow">ssmithnet.net</p><h2>Before the feed,<br>there was a home page.</h2><p class="section-deck">An address that stayed while the interests grew.</p></header><div class="timeline">)";
    const vector<Event> events = {
        {"c. 1996", "The Infiltrator 6.9 page", "A Mercedes-Benz 450SEL 6.9 was the original subject. The page became the seed of ssmithnet.net and its Mercedes-Benz history."},
        {"Beta years", "Windows, Solaris & games", "Voluntary beta testing included Microsoft Windows ME, the Solaris beta team and EA game projects. Windows ME testing included a Beta Tester of the Month award."},
        {"2016", "The software workshop", "System Monitor, Calendar, Defragmenter, InfiltratorFS and Calculator began as personal after-hours and weekend projects."},
        {"Today", "The work continues", "Cars now share the domain with InfiltratorFS, native software and the Link diagnostic family. It remains a personal website."}
    };
    for (const auto& event : events)
        o << "<article class=\"event\"><span class=\"era\">" << esc(event.date) << "</span><div><h3>" << esc(event.title) << "</h3><p>" << esc(event.description) << "</p></div></article>";
    o << R"(</div></section><section class="quote-section"><p class="eyebrow">A personal principle</p><blockquote class="quote">“I am free, no matter what rules surround me. If I find them tolerable, I tolerate them; if I find them too obnoxious, I break them. I am free because I know that I alone am morally responsible for everything I do.”<cite>— Robert A. Heinlein</cite></blockquote></section>)";
    return o.str() + page_end("Since the 1990s. Still here.");
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
