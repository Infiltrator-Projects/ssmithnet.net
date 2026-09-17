#!/usr/bin/env python3
"""Validate generated output and local navigation without rewriting checked-in pages."""
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PAGES = ('index.html', 'workbench.html', 'garage.html', 'archive.html')
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
        if 'id' in attrs: self.ids.append(attrs['id'])
        if tag == 'h1': self.headings += 1
        if attrs.get('aria-current') == 'page': self.current += 1
        if tag == 'link' and attrs.get('rel') == 'canonical': self.canonical += 1
        for key in ('href', 'src'):
            if key in attrs: self.links.append(attrs[key])

with tempfile.TemporaryDirectory() as directory:
    out = Path(directory) / 'output'
    subprocess.run([str(ROOT / 'build/sitegen'), str(out)], cwd=directory, check=True)
    pages = {}
    for name in PAGES:
        generated = (out / name).read_bytes()
        assert generated == (ROOT / name).read_bytes(), f'{name}: regenerate with make'
        p = Page(); p.feed(generated.decode()); pages[name] = p
        assert p.headings == 1, name
        assert p.current == 1 and p.canonical == 1, name
        assert len(p.ids) == len(set(p.ids)), f'duplicate IDs in {name}'
    for name, page in pages.items():
        for link in page.links:
            u = urlsplit(link)
            if u.scheme or u.netloc: continue
            target = u.path or name
            assert (ROOT / target).is_file(), (name, link)
            if u.fragment:
                assert target in pages and u.fragment in pages[target].ids, (name, link)
    first = {name: (out / name).read_bytes() for name in PAGES}
    subprocess.run([str(ROOT / 'build/sitegen'), str(out)], cwd=directory, check=True)
    assert all(first[name] == (out / name).read_bytes() for name in PAGES)
print('PASS: deterministic output, committed-page parity, page landmarks and local links')
