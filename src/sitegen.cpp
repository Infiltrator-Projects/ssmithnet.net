#include <filesystem>
#include "../include/site-family.hpp"
#include <cstring>
#include <iostream>
#include <infiltratr/escape.h>
#include <infiltratr/posix.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using std::string;
using std::vector;

struct Car {
    string tag, title, make, model, series, powertrain, description;
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
        "</head><body class=\"" + esc(body_class) + "\">" + infiltrator_web::navigation(active, prefix) + "<main id=\"main\">";
}

static string page_end(const string& right) {
    return "</main>" + infiltrator_web::footer(right);
}

static string spec(const string& label, const string& value) {
    return "<div class=\"spec\"><span class=\"spec-label\">" + esc(label) + "</span><span class=\"spec-value\">" + esc(value) + "</span></div>";
}

static string card(const Car& c) {
    std::ostringstream o;
    o << "<article class=\"panel\"><div class=\"tag\">" << esc(c.tag) << "</div><h3>" << esc(c.title)
      << "</h3><p>" << esc(c.description) << "</p><div class=\"specs\">"
      << spec("Make",c.make) << spec("Model",c.model) << spec("Chassis / Series",c.series)
      << spec("Powertrain",c.powertrain) << "</div></article>";
    return o.str();
}

static vector<Car> historical_cars() {
    return {
        {"W116","450SEL 6.9","Mercedes-Benz","450SEL 6.9","116.036","M100 · 6.8 L V8","The car behind the original Infiltrator 6.9 page and the earliest chapter of the domain."},
        {"W116","280SE","Mercedes-Benz","280SE","116.024","M110 · 2.8 L I6","The six-cylinder W116 in my garage history, alongside the 450SEL 6.9."},
        {"C126 · AMG","500SEC","Mercedes-Benz","500SEC","126.044","5.0 L V8 · AMG","The C126 coupé in my Mercedes history. This one was AMG."},
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
    <a class="text-link" href="workbench.html">Inside the Workbench <span aria-hidden="true">↗</span></a>
  </div>
  <aside class="home-index" aria-label="Explore the site"><div class="index-caption">A personal collection</div>
    <a href="workbench.html"><span class="index-no">01</span><span><strong>Workbench</strong><small>Software, filesystems &amp; vehicle diagnostics</small></span><span aria-hidden="true">↗</span></a>
    <a href="garage.html"><span class="index-no">02</span><span><strong>Garage</strong><small>Mercedes, Falcons &amp; everything between</small></span><span aria-hidden="true">↗</span></a>
    <a href="archive.html"><span class="index-no">03</span><span><strong>Archive</strong><small>The 6.9 page, beta years &amp; the open web</small></span><span aria-hidden="true">↗</span></a>
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
<section class="page-intro"><div class="kicker">Workbench</div><h1>From the question<br>to the working machine.</h1><p class="lead">Storage, native software and vehicle diagnostics share the bench here. The common thread is the same: understand what the machine is doing, own the important behaviour, preserve the evidence, and test the result.</p><nav class="section-nav" aria-label="Workbench sections"><a href="#infiltratorfs">InfiltratorFS</a><a href="#software">Software</a><a href="#link">LINK family</a><a href="#repository">Package repository</a><a href="#principles">Working principles</a></nav></section>
<section id="infiltratorfs" class="project-section"><div class="project-heading"><div><span class="kicker">01 / Storage</span><h2>InfiltratorFS</h2></div><span class="status-note">Active development · Format 0.18</span></div>
<div class="project-intro"><p class="large-copy">A filesystem built from scratch, with the on-disk format and recovery behaviour at the centre of the design.</p><p>The Linux path is a native kernel driver. The project also includes formatters, inspection and scrub tools, and Windows access through a driverless Explorer bridge. A native Windows filesystem driver remains future work.</p></div>
<div class="engineering-notes"><article><span class="note-no">01</span><h3>Publish a new generation</h3><p>Copy-on-write transactions create new state while retaining earlier generations. Checksummed checkpoints provide a way to select a recoverable generation after interruption.</p></article><article><span class="note-no">02</span><h3>Share data deliberately</h3><p>Snapshots and reflinks share file data. Sparse files and paged extent metadata describe where that data lives without requiring every logical byte to occupy storage.</p></article><article><span class="note-no">03</span><h3>Verify what comes back</h3><p>CRC64 protects metadata and SHA-256 checks logical file data. Inspection and deep-scrub tools make verification an explicit operation.</p></article></div>
<details class="technical-detail"><summary>Inside the format <span aria-hidden="true">+</span></summary><dl class="fact-list"><div><dt>Identity</dt><dd>128-bit object identifiers</dd></div><div><dt>Names</dt><dd>UTF-8 components up to 1,023 bytes</dd></div><div><dt>Compression</dt><dd>Native IAC1; LZ4 retained as a non-default representation</dd></div><div><dt>Linux integration</dt><dd>Native VFS driver installed through DKMS</dd></div></dl></details>
<div class="work-note"><h3>The work still ahead</h3><p>Booting a real Linux installation brings mount latency, writeback and metadata costs into focus. Those remain active engineering work. Feature support and passing tests are recorded separately from claims about everyday performance.</p></div>
<div class="resource-links"><a href="https://github.com/Infiltrator-Projects/InfiltratorFS">Source code ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/QUALIFICATION.md">Test evidence ↗</a><a href="https://github.com/Infiltrator-Projects/InfiltratorFS/blob/main/docs/ROADMAP.md">Feature status ↗</a></div></section>
<section id="software" class="project-section"><div class="project-heading"><div><span class="kicker">02 / Native software</span><h2>The software bench.</h2></div><span class="status-note">Personal projects · Begun 2016</span></div>
<div class="project-intro"><p class="large-copy">Four different tools, built for the same reason: understand the problem properly and own the important behaviour.</p><p>System Monitor, Defragmenter, Calendar and Calculator grew alongside InfiltratorFS as after-hours and weekend projects. They share engineering ideas and selected Common primitives, but each keeps its own domain rules and platform responsibilities.</p></div>
<div class="family-register"><a href="https://github.com/Infiltrator-Projects/System-Monitor"><strong>System Monitor</strong><span>Linux · native system management through direct OS and hardware interfaces</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Defragmenter"><strong>Defragmenter</strong><span>Linux · offline filesystem allocation analysis, defragmentation and recovery</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calendar"><strong>Calendar</strong><span>Cinnamon · alternative clocks, chronology, calendars and astronomy</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Calculator"><strong>Calculator</strong><span>Linux, Windows &amp; iPhone · shared calculation core with native interfaces</span><b aria-hidden="true">↗</b></a></div></section>
<section id="link" class="project-section"><div class="project-heading"><div><span class="kicker">03 / Automotive</span><h2>The LINK family.</h2></div><span class="status-note">Shared engine · Manufacturer-specific knowledge</span></div>
<div class="project-intro"><p class="large-copy">One diagnostic engine, with each vehicle's knowledge in the right place.</p><p>LINK owns the common transport, OBD and UDS handling, sessions and scheduling. Mercedes, Jaguar, BMW, Audi and Ford applications add their own identities, definitions and manufacturer-specific behaviour.</p></div>
<ol class="diagnostic-flow" aria-label="Diagnostic workflow"><li><span>01</span><strong>Connect</strong><p>Select the adapter.</p></li><li><span>02</span><strong>Identify</strong><p>Read vehicle and module identity.</p></li><li><span>03</span><strong>Match</strong><p>Use supported definitions.</p></li><li><span>04</span><strong>Select</strong><p>Choose the data to request.</p></li><li><span>05</span><strong>Observe</strong><p>Keep results tied to their source.</p></li></ol>
<div class="family-register"><a href="https://github.com/Infiltrator-Projects/LINK"><strong>LINK</strong><span>The shared C11 diagnostic and application engine</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/MBLINK"><strong>MBLINK</strong><span>Mercedes-Benz · developed with the C207 / OM651</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/Jaglink"><strong>JAGLINK</strong><span>Jaguar · X-Type / X400 knowledge</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/BMWLINK"><strong>BMWLINK</strong><span>BMW</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/AUDILINK"><strong>AUDILINK</strong><span>Audi</span><b aria-hidden="true">↗</b></a><a href="https://github.com/Infiltrator-Projects/FORDLINK"><strong>FORDLINK</strong><span>Ford</span><b aria-hidden="true">↗</b></a></div>
<details class="technical-detail"><summary>What Discover is for <span aria-hidden="true">+</span></summary><p>Discover is the specialist side of the family: module identification, read-only inventory, captures and evidence export. Shared interrogation mechanics belong in LINK; manufacturer-specific addresses and meanings belong with the vehicle application.</p><p>An unknown response stays unknown until documentation or reproducible captures establish its meaning.</p></details></section>
<section id="repository" class="project-section"><div class="project-heading"><div><span class="kicker">04 / Distribution</span><h2>Package Repository.</h2></div><span class="status-note">Linux Mint &amp; Debian-family desktops · Beta</span></div><div class="project-intro"><p class="large-copy">One place to find the software.<br>One APT source to keep it updated.</p><p>The software centre brings the Linux applications together, including InfiltratorFS, System Monitor, Defragmenter, Calendar, Calculator and the LINK applications. Each project releases independently; the repository checks release package hashes and builds the package catalogue and APT metadata from those packages.</p></div><div class="resource-links"><a href="https://infiltrator-projects.github.io/Infiltrator-Repository/">Browse software &amp; setup instructions ↗</a><a href="https://github.com/Infiltrator-Projects/Infiltrator-Repository">Repository source ↗</a></div></section>
<section id="principles" class="feature-row"><div><div class="kicker">Working principles</div><h2>Keep the evidence.<br>Question the result.</h2></div><div><p>A successful build proves that code compiled. A clean test proves the case it exercised. Neither automatically proves that an application behaves correctly on a real car or a filesystem handles a real desktop workload.</p><p>That gap is where much of the work happens.</p><a class="text-link" href="https://github.com/Infiltrator-Projects">The project repositories ↗</a></div></section>
)" + page_end("The best or nothing.");
}

static string render_garage() {
    std::ostringstream o;
    o << page_start("Garage","My Mercedes-Benz C207 and the cars I have owned: W116s, Falcons, LPG V8s, a Torana, Discovery and Peugeot diesel.","Garage","garage-page");
    o << R"(<section><div class="section-head"><div><div class="kicker">Garage</div><h1>The machines behind the stories.</h1></div><p>From the W116 that started the website to the C207 in the garage today.</p></div><p class="section-intro">The early ssmithnet.net was about Mercedes-Benz. That history is still part of the site because it explains a lot about what came later: an appreciation for things that are engineered properly, refined over time and made to last.</p><div class="panel"><div class="tag">Current · VIN verified</div><h2>E 250 CDI BlueEFFICIENCY Coupé</h2><p>The car in the garage now: a 2011 Mercedes-Benz C207.</p><div class="specs">)";
    o << spec("Make","Mercedes-Benz") << spec("Model","E 250 CDI Coupé") << spec("Chassis / Baumuster","207.303")
      << spec("Engine","2,143 cc OM651 diesel") << spec("Power","150 kW / 204 PS") << spec("Production","12 May 2011 · Euro 5") << spec("VIN","WDD2073032F129158");
    o << R"(</div><div class="meta"><span class="pill">Leather</span><span class="pill">Panoramic roof</span><span class="pill">PARKTRONIC</span><span class="pill">Heated front seats</span><span class="pill">THERMATIC climate control</span><span class="pill">Intelligent Light System</span><span class="pill">Parameter steering</span></div></div></section>)";
    o << R"(<section><div class="section-head"><div><div class="kicker">Mercedes-Benz</div><h2>The cars that started it.</h2></div><p>The original subject of ssmithnet.net.</p></div><div class="grid three">)";
    auto cars = historical_cars();
    for (std::size_t i=0;i<3;i++) o << card(cars[i]);
    o << R"(</div></section><section><div class="section-head"><div><div class="kicker">Australian Fords &amp; further afield</div><h2>Beyond the three-pointed star.</h2></div><p>Falcons, LPG V8s, a little Torana and two very different diesels.</p></div><div class="grid three">)";
    for (std::size_t i=3;i<cars.size();i++) o << card(cars[i]);
    o << R"(</div></section><section class="feature-row"><div><div class="kicker">A recurring theme</div><h2>Different answers<br>to the same fascination.</h2></div><div><p>The 6.9 and the 1.1-litre Torana sit at very different ends of this collection. Both belong in it. So do the LPG Fords and the Peugeot diesel I ran on waste motor oil.</p><p>The current Mercedes connects the garage to the Workbench: the C207 and its OM651 diesel are the physical development vehicle for MBLINK.</p><a class="text-link" href="workbench.html#link">Follow that connection →</a></div></section><div class="center"><a class="back" href="index.html">← Back to ssmithnet.net</a></div>)";
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
    return page + "<section class=\"page-intro\"><div class=\"kicker\">Download</div><h1>Model 11 Sandy<br>v1.0.0</h1><p class=\"lead\">Your download should begin automatically.</p><p><a class=\"text-link\" href=\"" + esc(url) + "\">Download the ZIP now <span aria-hidden=\"true\">→</span></a></p></section>" + page_end("Direct download");
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

