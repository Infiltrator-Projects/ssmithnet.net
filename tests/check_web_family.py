#!/usr/bin/env python3
"""Shared output contract for both independently published website surfaces."""
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import urlsplit,unquote
import posixpath
import sys
root=Path(sys.argv[1] if len(sys.argv)>1 else '.').resolve()
class Page(HTMLParser):
    def __init__(self):
        super().__init__();self.ids=[];self.nav=[];self.in_nav=False;self.skip=False;self.h1=0;self.canonical=0;self.og=False;self.links=[]
    def handle_starttag(self,tag,attrs):
        a=dict(attrs)
        if 'id' in a:self.ids.append(a['id'])
        if tag=='nav' and a.get('aria-label')=='Primary':self.in_nav=True
        if tag=='a' and self.in_nav:self.nav.append(a.get('href',''))
        if tag=='a' and a.get('href')=='#main':self.skip=True
        if tag=='h1':self.h1+=1
        if tag=='link' and a.get('rel')=='canonical':self.canonical+=1
        if tag=='meta' and a.get('property')=='og:title':self.og=True
        for attr in ('href','src'):
            if attr in a:self.links.append(a[attr])
    def handle_endtag(self,tag):
        if tag=='nav':self.in_nav=False
pages=[root/'index.html']+[root/p for p in ('workbench.html','garage.html','archive.html','downloads/model-11-sandy/index.html') if (root/p).is_file()]
for path in pages:
    p=Page();p.feed(path.read_text())
    assert p.h1==1 and p.canonical==1 and p.og,path
    assert p.skip and 'main' in p.ids and len(p.ids)==len(set(p.ids)),path
    assert len(p.nav)==6 and 'https://infiltrator-projects.github.io/Infiltrator-Repository/' in p.nav,path
    for ref in p.links:
        u=urlsplit(ref)
        if u.scheme or u.netloc or not u.path:continue
        target=(path.parent/unquote(u.path)).resolve()
        assert target.is_file(),(path,ref)
    assert '@@HEAD@@' not in path.read_text() and '@@APP_CARDS@@' not in path.read_text()
fonts=('mb-corpo-a-cond-regular.woff2','mb-corpo-s-regular.woff2','mb-corpo-s-bold.woff2')
css=(root/'assets/site.css').read_text()
for font in fonts:
    assert font in css and (root/'assets/fonts'/font).read_bytes()[:4]==b'wOF2',font
assert css.count('@font-face')==3
print(f'PASS: shared navigation, metadata, landmarks, assets and three real Corpo fonts across {len(pages)} pages')
