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
    const string filename = active == "Home" ? "" : active == "Workbench" ? "workbench.html" : active == "Garage" ? "garage.html" : active == "Download" ? "downloads/model-11-sandy/" : "archive.html";
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
    o << "<article class=\"garage-entry\"><div class=\"tag\">" << esc(c.tag) << "</div><h3>" << esc(c.title)
      << "</h3><p>" << esc(c.description) << "</p><dl class=\"car-facts\">"
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
    return page_start("Shannon Smith", "Cars, code and a personal corner of the internet since the 1990s. Shannon Smith's projects, garage and archive.", "Home", "home-page") + R"(
<section class="intro-hero" aria-labelledby="home-title">
  <div class="intro-copy"><div class="kicker">Shannon Smith · Mooroopna, Australia</div>
    <h1 id="home-title">Cars. Code.<br>And a lifelong<br><em>need to understand.</em></h1>
    <p class="lead">This domain began with a Mercedes-Benz 450SEL 6.9. These days the workshop includes a filesystem built from scratch, vehicle diagnostics, and the machines I keep coming back to.</p>
    <a class="text-link" href="workbench.html">Inside the Workbench <span aria-hidden="true">→</span></a>
  </div>
  <aside class="home-index" aria-label="Explore the site"><div class="index-caption">A personal collection</div>
    <a href="workbench.html"><span class="index-no">01</span><span><strong>Workbench</strong><small>Software, filesystems &amp; vehicle diagnostics</small></span><span aria-hidden="true">→</span></a>
    <a href="garage.html"><span class="index-no">02</span><span><strong>Garage</strong><small>Mercedes, Falcons &amp; everything between</small></span><span aria-hidden="true">→</span></a>
    <a href="archive.html"><span class="index-no">03</span><span><strong>Archive</strong><small>The 6.9 page, beta years &amp; the open web</small></span><span aria-hidden="true">→</span></a>
    <div class="index-foot">Personal domain. Since the 1990s.</div>
  </aside>
</section>
<section class="feature-row" aria-labelledby="current-title"><div><div class="kicker">On the bench</div><h2 id="current-title">What happens when you build<br>a filesystem from first principles?</h2></div><div><p>InfiltratorFS is where questions about storage become code: what survives a crash, how old versions are retained, and what it takes to boot and run a real Linux installation.</p><a class="text-link" href="workbench.html#infiltratorfs">The engineering behind InfiltratorFS <span aria-hidden="true">→</span></a><p><a class="text-link" href="https://infiltrator-projects.github.io/Infiltrator-Repository/">Browse the Software Centre ↗</a></p></div></section>
<section class="interests" aria-labelledby="interests-title"><div class="section-head"><div><div class="kicker">Away from the keyboard</div><h2 id="interests-title">Mechanical. Analogue. Personal.</h2></div><p>Open-reel tape, watches, music, films and games have their place here too.</p></div><div class="interest-columns"><div><h3>Four wheels</h3><p>A C207 Mercedes-Benz today; a garage history stretching through W116s, Australian Fords, a Torana and a Peugeot diesel.</p><a class="text-link" href="garage.html">Explore the garage →</a></div><div><h3>Two reels</h3><p>Open-reel tape decks and analogue audio: another part of my interest in machinery, alongside the music itself.</p></div><div><h3>On the wrist</h3><p>A Cosmograph Daytona 126500LN in Oystersteel. Different scale, same fascination with mechanical engineering.</p></div></div></section>
<section class="origin-strip"><span class="kicker">The beginning</span><h2>Before the feed,<br>there was a home page.</h2><div><p>The 450SEL 6.9 was the original subject. Cars, beta testing and software followed. The address remained a place of my own.</p><a class="text-link" href="archive.html">Back to the beginning →</a></div></section>
<section class="quote-home"><blockquote class="quote">“I am free, no matter what rules surround me. If I find them tolerable, I tolerate them; if I find them too obnoxious, I break them. I am free because I know that I alone am morally responsible for everything I do.”<small>— Robert A. Heinlein</small></blockquote></section>
)" + page_end("Since the 1990s. Still here.");
}

static string render_workbench() {
    return page_start("Workbench", "Inside InfiltratorFS, native software and the LINK vehicle-diagnostics family: storage design, desktop tools and work in progress.", "Workbench") + R"(
<section class="page-intro"><div class="kicker">Workbench</div><h1>From the question<br>to the working machine.</h1><p class="lead">Storage, native software and vehicle diagnostics share the bench here. Each section follows the machine from design intent to working evidence.</p><nav class="section-nav" aria-label="Workbench sections"><a href="#infiltratorfs">InfiltratorFS</a><a href="#software">Software</a><a href="#link">LINK family</a><a href="#repository">Package repository</a><a href="#principles">Working principles</a></nav></section>
<section id="infiltratorfs" class="project-section"><div class="project-heading"><div><span class="project-label">01 / Storage</span><h2>InfiltratorFS</h2></div><span class="status-note">Active development · Format 0.18</span></div>
<div class="project-intro"><p class="large-copy">A filesystem built from scratch, with the on-disk format and recovery behaviour at the centre of the design.</p><p>The Linux path is a native kernel driver. The project also includes formatters, inspection and scrub tools, and Windows access through a driverless Explorer bridge. A native Windows filesystem driver remains future work.</p></div>
<div class="engineering-notes"><article><span class="note-no">01</span><h3>Publish a new generation</h3><p>Copy-on-write transactions create new state while retaining earlier generations. Checksummed checkpoints provide a way to select a recoverable generation after interruption.</p></article><article><span class="note-no">02</span><h3>Share data deliberately</h3><p>Snapshots and reflinks share file data. Sparse files and paged extent metadata describe where that data lives without requiring every logical byte to occupy storage.</p></article><article><span class="note-no">03</span><h3>Verify what comes back</h3><p>CRC64 protects metadata and SHA-256 checks logical file data. Inspection and deep-scrub tools make verification an explicit operation.</p></article></div>
<details class="technical-detail"><summary>Inside the format <span aria-hidden="true">+</span></summary><dl class="fact-list"><div><dt>Identity</dt><dd>128-bit object identifiers</dd></div><div><dt>Names</dt><dd>UTF-8 components up to 1,023 bytes</dd></div><div><dt>Compression</dt><dd>Native IAC1; LZ4 retained as a non-default representation</dd></div><div><dt>Linux integration</dt><dd>Native VFS driver installed through DKMS</dd></div></dl></details>
<div class="work-note"><h3>The work still ahead</h3><p>Booting a real Linux installation brings mount latency, writeback and metadata costs into focus. Those remain active engineering work. Feature support and passing tests are recorded separately from claims about everyday performance.</p></div>
<div class="resource-links"><a href="https://github.com/Infiltrator-Projects/InfiltratorFS">Source code ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/QUALIFICATION.md">Test evidence ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/ROADMAP.md">Feature status ↗</a></div></section>
<section id="software" class="project-section"><div class="project-heading"><div><span class="project-label">02 / Native software</span><h2>The software bench.</h2></div><span class="status-note">Personal projects · Begun 2016</span></div>
<div class="project-intro"><p class="large-copy">Four different tools, built for the same reason: understand the problem properly and own the important behaviour.</p><p>System Monitor, Defragmenter, Calendar and Calculator grew alongside InfiltratorFS as after-hours and weekend projects. They share engineering ideas and selected Common primitives, but each keeps its own domain rules and platform responsibilities.</p></div>
<div class="family-register"><a href="https://github.com/Infiltrator-Projects/System-Monitor"><strong>System Monitor</strong><span>Linux · native system management through direct OS and hardware interfaces</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Defragmenter"><strong>Defragmenter</strong><span>Linux · offline filesystem allocation analysis, defragmentation and recovery</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calendar"><strong>Calendar</strong><span>Cinnamon · alternative clocks, chronology, calendars and astronomy</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calculator"><strong>Calculator</strong><span>Linux, Windows &amp; iPhone · shared calculation core with native interfaces</span><b aria-hidden="true">↗</b></a></div></section>
<section id="link" class="project-section"><div class="project-heading"><div><span class="project-label">03 / Automotive</span><h2>The LINK family.</h2></div><span class="status-note">Shared engine · Manufacturer-specific knowledge</span></div>
<div class="project-intro"><p class="large-copy">One diagnostic engine, with each vehicle's knowledge in the right place.</p><p>LINK owns the common transport, OBD and UDS handling, sessions and scheduling. Mercedes, Jaguar, BMW, Audi and Ford applications add their own identities, definitions and manufacturer-specific behaviour.</p></div>
<ol class="diagnostic-flow" aria-label="Diagnostic workflow"><li><span>01</span><strong>Connect</strong><p>Select the adapter.</p></li><li><span>02</span><strong>Identify</strong><p>Read vehicle and module identity.</p></li><li><span>03</span><strong>Match</strong><p>Use supported definitions.</p></li><li><span>04</span><strong>Select</strong><p>Choose the data to request.</p></li><li><span>05</span><strong>Observe</strong><p>Keep results tied to their source.</p></li></ol>
<div class="family-register"><a href="https://github.com/Infiltrator-Projects/LINK"><strong>LINK</strong><span>The shared C11 diagnostic and application engine</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/MBLINK"><strong>MBLINK</strong><span>Mercedes-Benz · developed with the C207 / OM651</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Jaglink"><strong>JAGLINK</strong><span>Jaguar · X-Type / X400 knowledge</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/BMWLINK"><strong>BMWLINK</strong><span>BMW</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/AUDILINK"><strong>AUDILINK</strong><span>Audi</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/FORDLINK"><strong>FORDLINK</strong><span>Ford</span><b aria-hidden="true">↗</b></a></div>
<details class="technical-detail"><summary>What Discover is for <span aria-hidden="true">+</span></summary><p>Discover is the specialist side of the family: module identification, read-only inventory, captures and evidence export. Shared interrogation mechanics belong in LINK; manufacturer-specific addresses and meanings belong with the vehicle application.</p><p>An unknown response stays unknown until documentation or reproducible captures establish its meaning.</p></details></section>
<section id="repository" class="project-section"><div class="project-heading"><div><span class="project-label">04 / Distribution</span><h2>Package Repository.</h2></div><span class="status-note">Linux Mint &amp; Debian-family desktops · Beta</span></div><div class="project-intro"><p class="large-copy">One place to find the software.<br>One APT source to keep it updated.</p><p>The software centre brings the Linux applications together, including InfiltratorFS, System Monitor, Defragmenter, Calendar, Calculator and the LINK applications. Each project releases independently; the repository checks release package hashes and builds the package catalogue and APT metadata from those packages.</p></div><div class="resource-links"><a href="https://infiltrator-projects.github.io/Infiltrator-Repository/">Browse software &amp; setup instructions ↗</a><a href="https://github.com/Infiltrator-Projects/Infiltrator-Repository">Repository source ↗</a></div></section>
<section id="principles" class="feature-row"><div><div class="kicker">Working principles</div><h2>Keep the evidence.<br>Question the result.</h2></div><div><p>A successful build proves that code compiled. A clean test proves the case it exercised. Neither automatically proves that an application behaves correctly on a real car or a filesystem handles a real desktop workload.</p><p>That gap is where much of the work happens.</p><a class="text-link" href="https://github.com/Infiltrator-Projects">The project repositories ↗</a></div></section>
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

static string vehicle_card(const VehicleRecord& car) {
    std::ostringstream o;
    o << "<article class=\"vehicle-card vehicle-card--" << (car.current ? "hero" : "historic")
      << "\" id=\"" << esc(car.id) << "\" aria-labelledby=\"" << esc(car.id) << "-title\">"
      << "<div class=\"vehicle-overview\"><header class=\"vehicle-heading\">"
      << "<div class=\"vehicle-status\"><span aria-hidden=\"true\"></span>"
      << (car.current ? "Current" : "Historic") << " · VIN verified</div>"
      << "<div class=\"vehicle-title-row\"><h2 id=\"" << esc(car.id) << "-title\">" << esc(car.title)
      << "</h2><span class=\"vehicle-year\">" << esc(car.year) << "</span></div>"
      << "<p class=\"vehicle-subtitle\">" << esc(car.subtitle) << "</p>"
      << "<dl class=\"vehicle-vin\"><dt>VIN</dt><dd>" << esc(car.vin) << "</dd></dl>"
      << "<dl class=\"vehicle-delivery\"><dt>Original delivery</dt><dd>" << esc(car.market) << "</dd></dl>"
      << "</header>" << vehicle_facts(car.specifications, "vehicle-specifications") << "</div>"
      << "<div class=\"vehicle-highlights\"><span class=\"vehicle-section-label\">At a glance</span><ul>";
    for (const auto& item : car.highlights) o << "<li>" << esc(item) << "</li>";
    o << "</ul></div><details class=\"vehicle-record\"><summary>Full vehicle record<span aria-hidden=\"true\"></span></summary>"
      << "<div class=\"vehicle-record-body\"><p class=\"vehicle-description\">" << esc(car.description)
      << "</p><div class=\"vehicle-record-groups\">";
    for (const auto& group : {std::pair<string, const vector<VehicleFact>*>{"Factory identity", &car.identity},
                              {"Dates & delivery", &car.dates}, {"Ownership & modifications", &car.history}}) {
        o << "<section><h3>" << esc(group.first) << "</h3>" << vehicle_facts(*group.second, "vehicle-record-facts") << "</section>";
    }
    o << "</div>";
    if (!car.equipment.empty()) {
        o << "<details class=\"vehicle-equipment\"><summary>Factory equipment · " << car.equipment.size()
          << " codes<span aria-hidden=\"true\"></span></summary><table><caption>" << esc(car.title)
          << " — factory equipment</caption><thead><tr><th scope=\"col\">Code</th><th scope=\"col\">Description</th></tr></thead><tbody>";
        for (const auto& option : car.equipment)
            o << "<tr><th scope=\"row\">" << esc(option.first) << "</th><td>" << esc(option.second) << "</td></tr>";
        o << "</tbody></table></details>";
    }
    if (!car.note.empty()) o << "<p class=\"vehicle-record-note\">" << esc(car.note) << "</p>";
    return o.str() + "</div></details></article>";
}

static VehicleRecord current_mercedes() {
    return {
        "e250-cdi", "E 250 CDI", "BlueEFFICIENCY Coupé · C207", "2011", "WDD2073032F129158", "Australia",
        "The Mercedes in the garage today. Built at Bremen as a right-hand-drive, pre-facelift E-Class coupé.", true,
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
        "The UK-delivered, pre-facelift C126. Factory specifications below describe the car before its AMG conversion.", false,
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
    auto start = page_start("Garage","My 2011 Mercedes-Benz E 250 CDI Coupé and the cars I have owned: the UK-delivered 500 SEC AMG, W116s, Falcons and more.","Garage","garage-page");
    start.insert(start.find("</head>"), "<link rel=\"stylesheet\" href=\"assets/garage.css?v=datacards-20261016-compact\">");
    o << start;
    o << R"(<section class="garage-feature-section" aria-labelledby="garage-title"><div class="garage-introduction"><div class="kicker">Garage</div><h1 id="garage-title">The machines behind the stories.</h1><p>From the W116 that started the website to the 2011 E-Class in the garage today.</p></div><div class="garage-featured" aria-label="Featured Mercedes-Benz vehicles">)";
    o << vehicle_card(current_mercedes()) << vehicle_card(historic_mercedes()) << "</div></section>";
    o << R"(<section class="garage-history" aria-labelledby="mercedes-history-title"><div class="section-head"><div><div class="kicker">Mercedes-Benz</div><h2 id="mercedes-history-title">The cars that started it.</h2></div><p>The original subject of ssmithnet.net.</p></div><div class="garage-register mercedes">)";
    auto cars = historical_cars();
    for (std::size_t i=0;i<2;i++) o << historical_car(cars[i]);
    o << R"(</div></section><section class="garage-history" aria-labelledby="ford-history-title"><div class="section-head"><div><div class="kicker">Australian Fords &amp; further afield</div><h2 id="ford-history-title">Beyond the three-pointed star.</h2></div><p>Falcons, LPG V8s, a little Torana and two very different diesels.</p></div><div class="garage-register">)";
    for (std::size_t i=2;i<cars.size();i++) o << historical_car(cars[i]);
    o << R"(</div></section><section class="feature-row garage-connections"><div><div class="kicker">A recurring theme</div><h2>Different answers<br>to the same fascination.</h2></div><div><p>The 6.9 and the 1.1-litre Torana sit at very different ends of this collection. Both belong in it. So do the LPG Fords and the Peugeot diesel I ran on waste motor oil.</p><p>The current Mercedes connects the garage to the Workbench: the C207 and its OM651 diesel are the physical development vehicle for MBLINK.</p><a class="text-link" href="workbench.html#link">Follow that connection →</a></div></section><div class="center"><a class="back" href="index.html">← Back to ssmithnet.net</a></div>)";
    return o.str() + page_end("Mercedes-Benz · Australian cars · Personal history");
}

static string render_archive() {
    std::ostringstream o;
    o << page_start("Archive","The origins of ssmithnet.net: the Infiltrator 450SEL 6.9 page, Windows ME and Solaris beta testing, and a personal place on the web.","Archive");
    vector<Event> e = {
        {"c. 1996","The 6.9 page","The Infiltrator 450SEL 6.9 page becomes the seed of ssmithnet.net and its Mercedes-Benz history."},
        {"1990s–2000s","Cars, web pages, experiments.","The domain remains a personal place for cars and whatever else is worth putting online."},
        {"Beta years","Windows and Solaris","Voluntary participation in Microsoft Windows ME beta testing and the Solaris beta team, along with game beta testing for EA projects."},
        {"2016","The software workshop begins.","System Monitor, Calendar, Defragmenter, InfiltratorFS and Calculator begin as personal after-hours and weekend projects."},
        {"Later","The hobby became the profession.","The web page stayed personal while computing and systems work became increasingly serious."},
        {"Now","The work continues.","Cars now share the domain with InfiltratorFS, the wider software projects and the LINK diagnostic family. The tools have changed; it is still a personal website."}
    };
    o << R"(<section><div class="section-head"><div><div class="kicker">Archive</div><h1>The old internet is part of the story.</h1></div><p>The Infiltrator 6.9 page, the beta-testing years, and an address of my own.</p></div><p class="section-intro">ssmithnet.net dates from the 1990s, when personal sites were something you built rather than something a platform gave you. This page keeps a few pieces of that history visible.</p><div class="panel"><div class="timeline">)";
    for (const auto& x : e) o << "<article class=\"event\"><span class=\"era\">" << esc(x.date) << "</span><div><h3>" << esc(x.title) << "</h3><p>" << esc(x.description) << "</p></div></article>";
    o << R"(</div></div></section><section><div class="section-head"><div><div class="kicker">Memory</div><h2>The beta-testing years.</h2></div><p>Windows ME, Solaris and EA game projects were part of that period.</p></div><div class="grid two"><article class="card"><div class="tag">Testing</div><h3>Beta tester</h3><p>Windows ME beta testing included a Beta Tester of the Month award. Other beta work included Solaris and EA game projects.</p></article><article class="card"><div class="tag">Web</div><h3>An address of my own.</h3><p>A domain gives this collection a stable address across changing interests: from a Mercedes page to software projects, without turning every part of life into a social profile.</p></article></div></section><div class="center"><a class="back" href="index.html">← Back to ssmithnet.net</a></div>)";
    return o.str() + page_end("Since the 1990s. Still here.");
}

static string render_download() {
    const string url = "https://github.com/Infiltrator-Projects/ssmithnet.net/releases/download/model-11-sandy-v1.0.0/Model_11_Sandy_v.1.0.0.zip";
    auto page = page_start("Model 11 Sandy v1.0.0", "Download Model 11 Sandy v1.0.0.", "Download");
    const auto head_end = page.find("</head>");
    page.insert(head_end, "<meta http-equiv=\"refresh\" content=\"2;url=" + esc(url) + "\">");
    return page + "<section class=\"page-intro\"><div class=\"kicker\">Download</div><h1>Model 11 Sandy<br>v1.0.0</h1><p class=\"lead\">Your download should begin automatically.</p><p><a class=\"text-link\" href=\"" + esc(url) + "\">Download the ZIP now <span aria-hidden=\"true\">↗</span></a></p></section>" + page_end("Direct download");
}

int main(int argc, char** argv) {
    try {
        if (argc > 2) throw std::runtime_error("usage: sitegen [output-directory]");
        const fs::path root = argc == 2 ? fs::absolute(argv[1]) : fs::current_path();
        write(root / "index.html", render_home());
        write(root / "workbench.html", render_workbench());
        write(root / "garage.html", render_garage());
        write(root / "archive.html", render_archive());
        write(root / "downloads/model-11-sandy/index.html", render_download());
        std::cout << "Generated ssmithnet.net pages from C++ source.\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "sitegen: " << ex.what() << '\n';
        return 1;
    }
}
