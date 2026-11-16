# ssmithnet.net

**Project copyright:** © 1993-2026 Shannon Smith

Personal website for Shannon Smith — a quiet corner of the internet founded in 1993.

The deployed site is deliberately boring at runtime: static HTML, local assets and no server-side application. The interesting part happens before deployment.

`src/sitegen.cpp` is the authoritative deterministic C++17 site generator. It owns the shared page structure, navigation, standardised car cards and repeated presentation logic for Home, Workbench, Projects, Garage, About and the download landing page.

Shared neutral presentation and infrastructure come from the immutable Common revision owned by [Makefile](Makefile) and [the Pages workflow](.github/workflows/generate-site.yml) rather than being reimplemented here. COMMON owns the canonical graphite/silver design tokens and web adapter, generic output escaping, generic durable file publication and the product-neutral GitHub Pages deployment action. This site owns its personal content, page composition, Workbench/Garage/About information architecture, local MB Corpo font assets, cyan/warm-material treatment and original graphics.

GitHub Pages is published from the generated GitHub Actions artifact. Actions builds the generator on an Ubuntu runner, verifies the committed Common web snapshot before the build modifies it, validates the generated site, creates a clean `public/` tree, adds the local assets and custom-domain file, and hands that directory to COMMON's reusable Pages deployment action.

The XF Falcon S-Pack gallery uses the beige 1985 XF reference photograph supplied during the site revision and a clearly identified illustrative front reconstruction. It keeps the original car's observed front-left direction in the main image, rather than reversing the view, and follows the standard two-photo gallery dimensions (1021×409 and 520×400). Neither is represented as an original owner photograph.

The original 500SEC AMG photographs are retained unchanged as `assets/photos/500sec-amg-side.jpg` and `assets/photos/500sec-amg-front.jpg`. The build prepares two faithful, lightly colour-corrected and sharpened WebP delivery images using native C++17 `tools/prepare_garage_photos.cpp`, libjpeg and libwebp, standardised to the E 250 gallery sizes (1021×409 and 520×400). The generated WebP files are not tracked in Git. The native pipeline preserves EXIF orientation, centre-crops using Lanczos resampling, applies restrained adjustments and writes validated WebP output atomically. No details of the vehicle or photographic background are synthesised.

The six HTML files kept on `main` are generated review mirrors, not source. Do not edit them by hand. `make check` regenerates the site and requires those mirrors to match `src/sitegen.cpp` byte-for-byte so reviewers can inspect the rendered output without creating a second source of truth.

Build locally with:

```text
make
```

Validation with:

```text
make check
```

Generate the six pages into an explicit directory with:

```text
./build/sitegen OUTPUT_DIRECTORY
```

This is deliberately the same Pages model used by the Software Centre: source is built on a temporary GitHub Actions Linux runner and only the finished static artifact is served to visitors.

The public site is `https://ssmithnet.net/`.

## Engineering ethos

What does a personal website look like when it is built from first principles, the public runtime is kept deliberately simple, and the site remains reproducible from owned source? ssmithnet.net treats generated static pages as an output, not as the source of truth.

The authoritative C++ generator owns structure, navigation and repeated presentation logic. Common contributes exact pinned first-party primitives where the contract is genuinely shared; GitHub Pages is a delivery mechanism rather than an application dependency. The site remains inspectable as ordinary static HTML and local assets, so a change in a framework or hosted runtime cannot redefine the content model.

The project prefers durable, understandable machinery over fashionable web stacks. A dependency is added because it provides a stronger contract, not merely because it is newer or convenient, and generated output must remain deterministic and reviewable.


## Website family

`include/site-family.hpp` and `assets/` are the canonical identity shared with Package Repository: page metadata, wordmark, cross-site navigation, footer, three local Corpo faces, colour/spacing conventions and accessible controls. The repository consumes an immutable, byte-verified snapshot, keeping both sites self-contained at runtime. Personal editorial composition and catalogue-specific behaviour remain with their respective products.

`tests/check_web_family.cpp` is the shared output contract. It validates navigation, metadata, landmarks, local resources and actual WOFF2 assets on both published trees. A change to the family is complete when the repository snapshot and its pinned source commit are updated and both sites pass this check. Common supplies neutral primitives underneath the family; its exact revision belongs to the build and deployment configuration linked above.

## Full project portfolio

The [Projects catalogue](projects.html) provides a dated, searchable index of all **104 public repositories** visible in `Infiltrator-Projects` on **16 November 2026**. This includes 31 Calendar modules, 23 Clock modules and 23 independently owned Filesystem engines, plus the operating-system, applications, diagnostic, creative and publishing projects. The repo names and editorial grouping are preserved in `data/github-portfolio-2026-11-16.tsv`. A module's presence is not a qualification or release claim. Current status belongs to the linked GitHub source.

The homepage and Workbench introduce featured work and link to the catalogue. The native C++ generator owns the generated HTML; no live GitHub API call is needed to browse the static page. Review both the curated portfolio and the source snapshot when repositories are added or renamed.

## Editorial ownership (16 November 2026)

**Home** introduces the site without duplicating its projects or its full history. **Workbench** owns substantial engineering explanation across InfiltratorOS, InfiltratorFS, native applications, diagnostics, games and audio. **Projects** contains the full source repository inventory; it is not an app store. **Garage** preserves chronological vehicle records and identifies photo provenance. **About** owns the 1993 origin, beta-testing history and personal interests. **Software Centre** is the separate release and package distribution surface. **Archive** intentionally aliases About and the Model 11 Sandy page is a short download redirect. Use cross-links instead of repeating complete sections.

## Site-wide usability pass — 16 November 2026

The shared visual system now caps engineering paragraphs at a readable line length, uses a compact two-column Workbench directory on ordinary phone widths, preserves all five primary links in a clear two-row phone header and supports a horizontal vehicle index on smaller displays. Long Workbench chapters and each historic Garage entry provide an explicit return path to their index. The footer of every generated page links back to the main content.

The Projects catalogue searches repository titles, descriptions and project families without requesting GitHub at runtime. A Clear control, visible results count and a no-match explanation make filtering reversible and understandable. The static collection remains browseable without JavaScript. The source generator, committed mirrors and stylesheet are one versioned change; browser pixel-level verification is distinct from successful build tests.

## Distinct Workbench and Projects surfaces — 17 November 2026

Workbench is an **engineering journal**, structured as six readable chapters with boundaries, reasoning, project-specific experiments and status qualifications. It deliberately avoids exhaustive cards or GitHub directories; readers reach family-specific source from a small contextual link after the relevant technical explanation. Its visual system uses a numbered contents list and continuous editorial chapters with an architecture map, diagnostic process and design notes.

Projects is a **repository lookup tool**: a concise header, snapshot metadata, search, 104 exact public repository links across eight groups, and a small note explaining the difference between source code and installable packages. It is intentionally less editorial and has no second Workbench promotional panel. Workbench's technical story does not belong in the repository directory, and Projects' exhaustive source listings do not belong in Workbench.
