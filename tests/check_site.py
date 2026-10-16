#!/usr/bin/env python3
"""Validate deterministic C++ output, checked-in mirrors and local navigation."""
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit
import subprocess
import tempfile
import re

ROOT = Path(__file__).resolve().parents[1]
PAGES = ("index.html", "workbench.html", "garage.html", "archive.html", "downloads/model-11-sandy/index.html")
GENERATOR = ROOT / "build" / "sitegen"


class Page(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.ids = []
        self.links = []
        self.headings = 0
        self.current = 0
        self.canonical = 0

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if "id" in attrs:
            self.ids.append(attrs["id"])
        if tag == "h1":
            self.headings += 1
        if attrs.get("aria-current") == "page":
            self.current += 1
        if tag == "link" and attrs.get("rel") == "canonical":
            self.canonical += 1
        for key in ("href", "src"):
            if key in attrs:
                self.links.append(attrs[key])


class GarageCard(HTMLParser):
    """Read public datacard fields without relying on the generator's data model."""
    def __init__(self, html):
        super().__init__(convert_charrefs=True)
        self.id = None
        self.classes = []
        self.fields = {}
        self.specification_labels = []
        self.equipment = []
        self.list_class = ""
        self.capture = None
        self.buffer = []
        self.label = ""
        self.feed(html)

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag == "article":
            self.id = attrs.get("id")
            self.classes = attrs.get("class", "").split()
        if tag == "dl":
            self.list_class = attrs.get("class", "")
        if tag in ("dt", "dd") or (tag == "th" and attrs.get("scope") == "row"):
            self.capture = tag
            self.buffer = []

    def handle_data(self, data):
        if self.capture:
            self.buffer.append(data)

    def handle_endtag(self, tag):
        if tag == self.capture:
            value = " ".join("".join(self.buffer).split())
            if tag == "dt":
                self.label = value
                if self.list_class == "vehicle-specifications":
                    self.specification_labels.append(value)
            elif tag == "dd":
                self.fields[self.label] = value
            elif tag == "th":
                self.equipment.append(value)
            self.capture = None
        if tag == "dl":
            self.list_class = ""


assert GENERATOR.is_file(), f"missing generator: {GENERATOR}"

with tempfile.TemporaryDirectory(prefix="ssmithnet-check-") as directory:
    out = Path(directory) / "site"
    subprocess.run([str(GENERATOR), str(out)], check=True)

    pages = {}
    first = {}
    for name in PAGES:
        generated_path = out / name
        assert generated_path.is_file(), f"generator did not create {name}"
        generated = generated_path.read_bytes()
        first[name] = generated

        committed_path = ROOT / name
        assert committed_path.is_file(), f"{name}: missing checked-in generated mirror"
        assert generated == committed_path.read_bytes(), (
            f"{name}: checked-in mirror is stale; regenerate with make"
        )

        parser = Page()
        parser.feed(generated.decode("utf-8"))
        pages[name] = parser

        assert parser.headings == 1, f"{name}: expected exactly one h1"
        assert parser.current == (0 if name.startswith("downloads/") else 1), f"{name}: expected exactly one aria-current page link"
        assert parser.canonical == 1, f"{name}: expected exactly one canonical link"
        assert len(parser.ids) == len(set(parser.ids)), f"{name}: duplicate IDs"

    for name, page in pages.items():
        for link in page.links:
            url = urlsplit(link)
            if url.scheme or url.netloc:
                continue

            target = str((Path(name).parent / url.path)) if url.path else name
            import posixpath
            target = posixpath.normpath(target)
            if target in PAGES:
                target_path = out / target
            else:
                target_path = ROOT / target
            assert target_path.is_file(), (name, link)

            if url.fragment:
                assert target in pages, (name, link)
                assert url.fragment in pages[target].ids, (name, link)

    subprocess.run([str(GENERATOR), str(out)], check=True)
    assert all(first[name] == (out / name).read_bytes() for name in PAGES), "generator output changed between identical runs"

garage = first["garage.html"].decode("utf-8")
cards = [GarageCard(html) for html in re.findall(r"<article\b[^>]*>.*?</article>", garage, re.S)]
featured = [card for card in cards if "vehicle-card" in card.classes]
assert [card.id for card in featured] == ["e250-cdi", "500-sec"], "current E-Class must lead the Mercedes records"
current, sec = featured
assert "vehicle-card--hero" in current.classes and "vehicle-card--hero" not in sec.classes
assert current.specification_labels == sec.specification_labels == [
    "Chassis / Baumuster", "Engine", "Power / torque", "Transmission", "Paint", "Upholstery"
]
for card, expected in [
    (current, {"VIN": "WDD2073032F129158", "Original delivery": "Australia",
               "Engine number": "651911 30 638336", "Transmission number": "722646 08 520376",
               "Release": "13 May 2011", "Delivery": "13 May 2011", "Registration": "YSF597"}),
    (sec, {"VIN": "WDB1260442A171593", "Original delivery": "United Kingdom",
           "Engine number": "117963 22 081711", "Transmission number": "722311 02 661254",
           "Production": "Approximately June 1985", "Order location": "Great Britain",
           "Registration": "1ARK884", "Acquired": "17 December 2002"})
]:
    for label, value in expected.items():
        assert card.fields.get(label) == value, (card.id, label, card.fields.get(label))
assert current.fields["Production"].startswith("12 May 2011"), "do not replace production with release/delivery"
assert len(current.equipment) == len(set(current.equipment)) == 69
assert {"197U", "251A", "527", "625", "901L", "R11", "VR"} <= set(current.equipment)
assert not sec.equipment, "do not invent SEC equipment codes"
assert "Mushroom leather" not in garage and "275A" not in garage
assert garage.count('class="vehicle-record"') == 2 and "<script" not in garage
assert 'href="assets/garage.css?v=' in garage
assert all("assets/garage.css" not in content.decode("utf-8") for name, content in first.items() if name != "garage.html")

print("PASS: deterministic C++ output, committed mirrors, page landmarks, assets and local links")
print("PASS: 2011 E-Class hero, matching Mercedes fields, delivery/serial/date evidence and 69 unique equipment codes")
