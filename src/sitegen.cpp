#include <filesystem>
#include <fstream>
#include <iostream>
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

static string esc(string s) {
    const std::pair<string,string> r[] = {
        {"&","&amp;"},{"<","&lt;"},{">","&gt;"},{"\"","&quot;"},{"'","&#39;"}
    };
    for (const auto& [a,b] : r) {
        std::size_t p = 0;
        while ((p = s.find(a, p)) != string::npos) {
            s.replace(p, a.size(), b);
            p += b.size();
        }
    }
    return s;
}

static void write(const fs::path& p, const string& s) {
    fs::create_directories(p.parent_path());
    std::ofstream f(p, std::ios::binary);
    if (!f) throw std::runtime_error("cannot open " + p.string() + " for writing");
    f << s;
    f.flush();
    if (!f) throw std::runtime_error("failed to write " + p.string());
}

static string page_start(const string& title, const string& description, const string& active, const string& body_class = "") {
    std::ostringstream o;
    o << "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><meta name=\"theme-color\" content=\"#090b0f\"><meta name=\"description\" content=\"" << esc(description) << "\"><title>" << esc(title) << " — ssmithnet.net</title><link rel=\"stylesheet\" href=\"assets/site.css?v=20260916-7\"><link rel=\"stylesheet\" href=\"assets/site-overrides.css?v=20260916-1\"></head><body";
    if (!body_class.empty()) o << " class=\"" << body_class << "\"";
    o << "><div class=\"shell\"><nav aria-label=\"Primary\"><a class=\"brand\" href=\"index.html\">ssmith<span>net</span>.net</a><div class=\"navlinks\">";
    for (const auto& n : {std::pair<string,string>{"Home","index.html"},{"Workbench","workbench.html"},{"Garage","garage.html"},{"Archive","archive.html"}})
        o << "<a" << (n.first == active ? " class=\"active\"" : "") << " href=\"" << n.second << "\">" << n.first << "</a>";
    o << "</div></nav><main>";
    return o.str();
}

static string page_end(const string& right) {
    return "</main><footer><div class=\"footerline\"><span>© Shannon Smith · ssmithnet.net</span><span>" + esc(right) + "</span></div></footer></div></body></html>";
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
        {"W116","280SE","Mercedes-Benz","280SE","116.024","M110 · 2.8 L I6","One of the Mercedes that formed the original personal archive."},
        {"C126 · AMG","500SEC","Mercedes-Benz","500SEC","126.044","5.0 L V8 · AMG","The coupé that joined the original Mercedes story — and, importantly, it was AMG."},
        {"Ford · Tickford LPG","EL Falcon","Ford","EL Falcon","EL","Petrol / LPG · Tickford","An earlier Falcon in the Australian chapter, with Tickford-installed LPG."},
        {"Ford","XF Falcon","Ford","XF Falcon","XF","Petrol","Another piece of the Falcon years."},
        {"Ford · BA · V8 · LPG","2003 Fairmont","Ford","Fairmont","BA","5.4 L V8 · Petrol / LPG","A BA Fairmont with the 5.4-litre V8 and dual-fuel LPG."},
        {"Holden","LC Torana","Holden","Torana","LC","1.1 L I4","The little 1.1-litre four-cylinder version I had."},
        {"Ford · BA · V8 · LPG","2004 LTD","Ford","LTD","BA","5.4 L V8 · Petrol / LPG","The later Australian luxury-car chapter, also a BA with the 5.4-litre V8 and dual-fuel LPG."},
        {"Ford · AU Series II · LPG","Fairmont","Ford","Fairmont","AU Series II","4.0 L · Petrol / LPG","An AU Series II Fairmont with the 4.0-litre engine and dual-fuel LPG."},
        {"Land Rover","Discovery 2 TDI","Land Rover","Discovery 2","Discovery II","TDI diesel","A diesel 4WD chapter in the collection of interesting machinery."},
        {"Peugeot","504 Diesel","Peugeot","504","504","Diesel · waste motor oil","An old-school diesel that I ran on waste motor oil. The exact variant remains deliberately unspecified."}
    };
}

static string render_home() {
    std::ostringstream o;
    o << page_start("Shannon Smith","Shannon Smith — a personal corner of the internet since the 1990s.","Home");
    o << R"(<section class="hero" aria-labelledby="hero-title"><div><div class="kicker">Personal domain · since the 1990s</div><h1 id="hero-title">A quiet corner of the internet, still being built.</h1><p class="lead">Cars were here first. Technology followed. Decades later, this is still the place for the things I care enough about to keep working on.</p><div class="actions"><a class="btn primary" href="workbench.html">Enter the Workbench</a><a class="btn" href="garage.html">Visit the Garage</a></div></div><div class="console" aria-label="System console"><div class="console-top" aria-hidden="true"><span class="dot"></span><span class="dot"></span><span class="dot"></span></div><div class="console-body"><div><span class="prompt">shannon@ssmithnet</span>:~$ whoami</div><div>ICT / technologist / tinkerer</div><br><div><span class="prompt">shannon@ssmithnet</span>:~$ status</div><div>online<br>curious<br>still building</div><br><div><span class="prompt">shannon@ssmithnet</span>:~$ echo $PRINCIPLE</div><div>the best or nothing.</div></div></div></section><div class="home-rule"></div>)";
    o << R"(<section class="instrument" aria-labelledby="now-title"><div class="instrument-head"><div><div class="kicker">Now</div><h2 id="now-title">Things currently occupying my brain</h2></div><p>A few things currently in the workshop.</p></div><div class="focus"><div class="focus-main"><div class="tag">Current obsession</div><h3>InfiltratorFS</h3><p>A ground-up filesystem project. The full technical story, development history and code are in the Workbench.</p><a class="focus-link" href="workbench.html">Enter the Workbench →</a></div><aside class="focus-side"><div class="tag">Also in the workshop</div><div class="readout"><div class="readout-row"><span class="readout-label">Automotive</span><a class="readout-value" href="workbench.html">LINK family</a></div><div class="readout-row"><span class="readout-label">Current machine</span><a class="readout-value" href="garage.html">E 250 Coupé</a></div><div class="readout-row"><span class="readout-label">History</span><a class="readout-value" href="archive.html">Since the 1990s</a></div></div></aside></div></section>)";
    o << R"(<section class="life" aria-labelledby="life-title"><div class="life-head"><div><div class="kicker">Outside the console</div><h2 id="life-title">The important distractions</h2></div><p>Cars, watches, open-reel tape, music and the machinery that is worth keeping around.</p></div><div class="life-layout"><div class="life-main"><h3>Well-made things tend to stay interesting.</h3><p>Cars, watches, tape machines and the other things that have managed to stick around.</p><div class="life-list"><a class="life-item" href="garage.html"><strong>The Garage</strong><span>Mercedes-Benz · Australian cars</span></a><span class="life-item"><strong>Open-reel</strong><span>Tape machines · analogue audio</span></span><span class="life-item"><strong>Cosmograph Daytona</strong><span>126500LN · Oystersteel</span></span><span class="life-item"><strong>After hours</strong><span>Music · films · games</span></span></div></div><aside class="life-aside"><div class="tag">Origin</div><h3>The page that started the domain.</h3><p>The first chapter was the Infiltrator 450SEL 6.9 page. The deeper story lives in the Garage and Archive.</p><a class="focus-link" href="garage.html">Enter the Garage →</a></aside></div></section>)";
    o << R"(<section class="story" aria-labelledby="story-title"><div class="story-strip"><div class="story-tag">Since the 1990s</div><div><h2 id="story-title">Same domain. Different machinery.</h2><p>The old site grew from cars into technology, beta testing and a lifetime of things interesting enough to keep.</p><a href="archive.html">Open the archive →</a></div></div></section><section class="quote-home"><div class="quote">“I am free, no matter what rules surround me. If I find them tolerable, I tolerate them; if I find them too obnoxious, I break them. I am free because I know that I alone am morally responsible for everything I do.”<small>— Robert A. Heinlein</small></div></section>)";
    return o.str() + page_end("Since the 1990s. Still here.");
}

static string render_workbench() {
    std::ostringstream o;
    o << page_start("Workbench","Shannon Smith — Workbench","Workbench");
    o << R"(<section><div class="section-head"><div><div class="kicker">Workbench</div><h1>Things I actually build.</h1></div><p>The machinery behind the ideas.</p></div><div class="panel"><div class="tag">Open source · current obsession</div><h2>InfiltratorFS</h2><p>A ground-up filesystem project exploring snapshots, copy-on-write, crash consistency, encryption, compression and serious storage engineering.</p><div class="actions"><a class="btn primary" href="https://github.com/Infiltrator-Projects/InfiltratorFS">Open InfiltratorFS</a></div></div></section><section><div class="section-head"><div><div class="kicker">Automotive</div><h2>The LINK family.</h2></div><p>One shared diagnostic architecture with product-specific knowledge where the vehicle actually requires it.</p></div><div class="grid"><article class="card"><div class="tag">Family</div><h3>LINK</h3><p>The common foundation: transport, sessions, profiles, scheduling and shared platform machinery.</p><div class="actions"><a class="btn" href="https://github.com/Infiltrator-Projects/LINK">Open LINK</a></div></article><article class="card"><div class="tag">Mercedes-Benz</div><h3>MBLINK</h3><p>The Mercedes member, with Mercedes-specific identification, catalogue knowledge and diagnostic handling.</p><div class="actions"><a class="btn" href="https://github.com/Infiltrator-Projects/MBLINK">Open MBLINK</a></div></article><article class="card"><div class="tag">Other marques</div><h3>JAGLINK · FORDLINK · AUDILINK · BMWLINK</h3><p>Product-specific members built on the shared LINK approach rather than separate worlds.</p></article></div></section><section><div class="section-head"><div><div class="kicker">Personal systems</div><h2>Linux &amp; Windows.</h2></div><p>Personal computing, software, packaging and making machines behave properly.</p></div><div class="panel"><p>This is the broad workshop, not a list of day-job tooling. The interesting part is the engineering: finding the right solution, refining it and keeping it useful.</p></div></section><section><div class="section-head"><div><div class="kicker">The standard</div><h2>Best, not newest.</h2></div><p>A solution earns its place by being refined enough to matter.</p></div><div class="panel"><p>Not every new thing is better. Not every old thing is obsolete. The standard is whether the solution is the right one after it has been properly thought through.</p></div></section><div class="center"><a class="back" href="index.html">← Back to ssmithnet.net</a></div>)";
    return o.str() + page_end("The best or nothing.");
}

static string render_garage() {
    std::ostringstream o;
    o << page_start("Garage","Shannon Smith — Garage","Garage","garage-page");
    o << R"(<section><div class="section-head"><div><div class="kicker">Garage</div><h1>Machines with personality.</h1></div><p>Where the story started.</p></div><p class="section-intro">The early ssmithnet.net was about Mercedes-Benz. That history is still part of the site because it explains a lot about what came later: an appreciation for things that are engineered properly, refined over time and made to last.</p><div class="panel"><div class="tag">Current · VIN verified</div><h2>E 250 CDI BlueEFFICIENCY Coupé</h2><p>The car in the garage now: a 2011 Mercedes-Benz C207.</p><div class="specs">)";
    o << spec("Make","Mercedes-Benz") << spec("Model","E 250 CDI Coupé") << spec("Chassis / Baumuster","207.303")
      << spec("Engine","2,143 cc OM651 diesel") << spec("Power","150 kW / 204 PS") << spec("Production","12 May 2011 · Euro 5") << spec("VIN","WDD2073032F129158");
    o << R"(</div><div class="meta"><span class="pill">Leather</span><span class="pill">Panoramic roof</span><span class="pill">Burmester surround sound</span><span class="pill">PARKTRONIC</span><span class="pill">Heated front seats</span><span class="pill">THERMATIC climate control</span><span class="pill">Intelligent Light System</span><span class="pill">Parameter steering</span></div></div></section>)";
    o << R"(<section><div class="section-head"><div><div class="kicker">Mercedes-Benz</div><h2>The cars that started it.</h2></div><p>The original subject of ssmithnet.net.</p></div><div class="grid three">)";
    auto cars = historical_cars();
    for (std::size_t i=0;i<3;i++) o << card(cars[i]);
    o << R"(</div></section><section><div class="section-head"><div><div class="kicker">Australian &amp; other cars</div><h2>The rest of the garage.</h2></div><p>Different machinery, different eras.</p></div><div class="grid three">)";
    for (std::size_t i=3;i<cars.size();i++) o << card(cars[i]);
    o << R"(</div></section><section><div class="section-head"><div><div class="kicker">Philosophy</div><h2>Engineering earns its place.</h2></div><p>What matters across the garage.</p></div><div class="panel"><p>Old, new, mechanical or digital: the interesting question is not how fashionable something is. It is whether it is genuinely the better solution.</p></div></section><div class="center"><a class="back" href="index.html">← Back to ssmithnet.net</a></div>)";
    return o.str() + page_end("Garage archive");
}

static string render_archive() {
    std::ostringstream o;
    o << page_start("Archive","Shannon Smith — Internet archive","Archive");
    vector<Event> e = {
        {"c. 1996","The 6.9 page","The Infiltrator 450SEL 6.9 page becomes the seed of ssmithnet.net and its Mercedes-Benz history."},
        {"1990s–2000s","Cars, web pages, experiments.","The domain remains a personal place for cars and whatever else is worth putting online."},
        {"Beta years","Windows and Solaris","Voluntary participation in Microsoft Windows ME beta testing and the Solaris beta team, along with game beta testing for EA projects."},
        {"Later","The hobby became the profession.","The web page stayed personal while computing and systems work became increasingly serious."},
        {"Now","The domain returns.","The current revision brings the old material together with the projects, cars and ideas that still deserve a place here."}
    };
    o << R"(<section><div class="section-head"><div><div class="kicker">Archive</div><h1>The old internet is part of the story.</h1></div><p>A record of where this came from.</p></div><p class="section-intro">ssmithnet.net dates from the 1990s, when personal sites were something you built rather than something a platform gave you. This page keeps a few pieces of that history visible.</p><div class="panel"><div class="timeline">)";
    for (const auto& x : e) o << "<article class=\"event\"><time>" << esc(x.date) << "</time><div><h3>" << esc(x.title) << "</h3><p>" << esc(x.description) << "</p></div></article>";
    o << R"(</div></div></section><section><div class="section-head"><div><div class="kicker">Memory</div><h2>The interesting bits stay interesting.</h2></div><p>Some details belong here simply because they happened.</p></div><div class="grid two"><article class="card"><div class="tag">Testing</div><h3>Beta tester</h3><p>Windows ME beta testing included a Beta Tester of the Month award. Other beta work included Solaris and EA game projects.</p></article><article class="card"><div class="tag">Web</div><h3>Before everything was a profile.</h3><p>The original site was a personal place on the open web, long before social platforms decided what a personal page should look like.</p></article></div></section><div class="center"><a class="back" href="index.html">← Back to ssmithnet.net</a></div>)";
    return o.str() + page_end("Since the 1990s. Still here.");
}

int main() {
    try {
        const fs::path root = fs::current_path();
        write(root / "index.html", render_home());
        write(root / "workbench.html", render_workbench());
        write(root / "garage.html", render_garage());
        write(root / "archive.html", render_archive());
        std::cout << "Generated ssmithnet.net pages from C++ source.\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "sitegen: " << ex.what() << '\n';
        return 1;
    }
}
