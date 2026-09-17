#!/usr/bin/env python3
"""Validate deterministic C++ output and local navigation without checked-in HTML."""
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PAGES = ("index.html", "workbench.html", "garage.html", "archive.html")
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

        parser = Page()
        parser.feed(generated.decode("utf-8"))
        pages[name] = parser

        assert parser.headings == 1, f"{name}: expected exactly one h1"
        assert parser.current == 1, f"{name}: expected exactly one aria-current page link"
        assert parser.canonical == 1, f"{name}: expected exactly one canonical link"
        assert len(parser.ids) == len(set(parser.ids)), f"{name}: duplicate IDs"

    for name, page in pages.items():
        for link in page.links:
            url = urlsplit(link)
            if url.scheme or url.netloc:
                continue

            target = url.path or name
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

print("PASS: deterministic C++ output, page landmarks, assets and local links")
