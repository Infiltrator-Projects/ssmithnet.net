# Validation

## Evidence model

Compilation, deterministic regression tests, platform integration and human visual/interaction testing prove different things.

## Automated gates

- .github/workflows/generate-site.yml

`tests/check_site.cpp` validates generated-site structure and all six committed review mirrors against the native generator. `make check` also runs the shared contract in `tests/check_web_family.cpp`.

## Manual/environment evidence

Browser rendering, typography fallback and visual quality across real browsers/screens still require human inspection; static generation tests do not prove every rendering engine.

Manual evidence supplements automation and should record the platform/environment actually observed.

## Release criterion

The exact source intended for release/publication must pass required automated checks, and generated/package artifacts must correspond to that source identity.

## Regression rule

Reproducible defects should gain permanent automated coverage at the narrowest layer that captures the original failure.

The deterministic gate covers all six generated pages, including About, its legacy Archive alias and the download landing page. `tests/check_web_family.cpp` additionally checks shared navigation, canonical/Open Graph metadata, unique landmarks, local asset references and the three WOFF2 fonts. Package Repository runs this same contract against its generated `public/` tree.

Garage checks preserve the complete fourteen-car history (the current E 250 CDI and thirteen former vehicles), keep the current 2011 E-Class as the first and only hero vehicle, require consistent field order on both documented Mercedes records and the historical vehicles, and cover factory serial numbers, UK/Australian delivery, distinct production/release/delivery dates and all 69 unique E-Class equipment codes. Native disclosures expose the full records without a JavaScript dependency. The original 500SEC photographs are local assets; regression tests verify their references, dimensions and alternatives within the historic record. The compatibility baseline is in `assets/site.css`. The shared, consolidated page hierarchy and responsive presentation are owned by `assets/site-redesign.css`, loaded after the detailed Garage rules in `assets/garage.css`. Historical records share the Make/Model/Chassis/Powertrain fields, while the current car has a separate hero composition. Shared changes require updating the Software Centre family pin.

The browser review shell in `tests/browser-review.html` uses relative paths to the actual generated pages at selected CSS-pixel viewport widths. It is a manual inspection tool rather than an automated screenshot proof. It is a review artifact, is not linked from the site and is removed from the publication workflow after review. Layout checks must distinguish iframe viewport rendering from a physical mobile-browser test.

## Browser evidence — 16 October 2026

Chrome desktop rendering was reviewed at 1363 × 936. The four primary pages were also rendered inside real browser frames at 320, 390 and 768 CSS pixels. Document widths matched their available viewport widths without horizontal overflow. Review covered the Home reading order, Workbench project links and disclosures, Garage history and the expanded E-Class record/equipment table, and About interests, timeline and quotation. The narrow-screen review caught and corrected word spacing around a hidden desktop line break.

Software Centre review covered the shared identity, phone-width catalogue, search narrowing to MBLink and its package dialog. The original three WOFF2 files were compared byte-for-byte with the prior site assets. That October session checked the then-current twelve-car site and 69 unique E-Class equipment codes; the present native regression checks preserve fourteen cars. The older screenshots do not certify the current redesign, and no physical-device or Safari result is claimed.

## Website-wide 1993 and mobile regression checks — 16 November 2026

The owner-confirmed website start is **1993**. The homepage, Garage and About timeline include it explicitly, and all generated pages share the footer identifying the site as online since 1993. No build or design documentation should invent a 2016 beginning for the website or its whole software portfolio. Regression tests verify the generated mirrors, site-wide footer and individual historical sections; responsive CSS adds a one-column history/facts layout at narrow phone widths.

The automated build proves output consistency, source-to-mirror equivalence, local link targets and asset presence. It does **not** establish pixel-perfect agreement with a design mockup or actual iOS rendering; visual browser review remains a separate required step.

## Complete-site design integration — 16 November 2026

A single authored presentation layer now serves Home, Workbench, Garage, About, the Archive alias and the download page. The independently hosted Software Centre must be pinned to the same assets and validate with the native shared website checker. Generator/mirror equivalence and repository build tests are required for both repositories; visual review at phone, tablet and desktop sizes is a separate acceptance step and is not implied by passing builds.

## Complete GitHub portfolio snapshot — 16 November 2026

The generator now publishes `projects.html` from the native C++ source with direct links to all 104 public Infiltrator-Projects repositories inventoried at the snapshot date. The page distinguishes 27 core/application/diagnostic/creative/publishing projects from 31 calendar, 23 clock and 23 filesystem modules. Each category has an explicit purpose and module presence is not presented as release qualification. A search field progressively filters prerendered links, so browsing the Git repos still works without JavaScript.

`data/github-portfolio-2026-11-16.tsv` is retained as the exact name/category audit manifest. The native site test verifies all 104 names, one direct link per repository, the family counts, page navigation, determinism and an exact source/generated mirror match. The shared family checker now expects a six-link navigation including Projects on the personal site and the Software Centre.
