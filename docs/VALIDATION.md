# Validation

## Evidence model

Compilation, deterministic regression tests, platform integration and human visual/interaction testing prove different things.

## Automated gates

- .github/workflows/generate-site.yml

tests/check_site.py validates generated-site structure and the Make validation path ensures committed review mirrors match generator output.

## Manual/environment evidence

Browser rendering, typography fallback and visual quality across real browsers/screens still require human inspection; static generation tests do not prove every rendering engine.

Manual evidence supplements automation and should record the platform/environment actually observed.

## Release criterion

The exact source intended for release/publication must pass required automated checks, and generated/package artifacts must correspond to that source identity.

## Regression rule

Reproducible defects should gain permanent automated coverage at the narrowest layer that captures the original failure.

The deterministic gate covers all six generated pages, including About, its legacy Archive alias and the download landing page. `tests/check_web_family.py .` additionally checks the common navigation, canonical/Open Graph metadata, unique landmarks, asset references and the three WOFF2 fonts. Package Repository runs this same contract against its generated `public/` tree.

Garage checks preserve the complete twelve-car history, keep the current 2011 E-Class as the first and only hero vehicle, require consistent field order on both documented Mercedes records and the historical vehicles, and cover factory serial numbers, UK/Australian delivery, distinct production/release/delivery dates and all 69 unique E-Class equipment codes. Native disclosures expose the full records without a JavaScript dependency. The shared type scale, spacing and page compositions are owned by `assets/site.css`; `assets/garage.css` owns only the expanded VIN record details. Historical records share the Make/Model/Chassis/Powertrain fields, while the current car has a separate hero composition. Shared changes require updating the Software Centre family pin.

The browser review shell in `tests/browser-review.html` gives the actual pages fixed 320, 390, 768 and 1280 CSS-pixel viewports without changing their content. It is a review artifact, is not linked from the site and is removed from the publication workflow after review. Layout checks must distinguish iframe viewport rendering from a physical mobile-browser test.

## Browser evidence — 16 October 2026

Chrome desktop rendering was reviewed at 1363 × 936. The four primary pages were also rendered inside real browser frames at 320, 390 and 768 CSS pixels. Document widths matched their available viewport widths without horizontal overflow. Review covered the Home reading order, Workbench project links and disclosures, Garage history and the expanded E-Class record/equipment table, and About interests, timeline and quotation. The narrow-screen review caught and corrected word spacing around a hidden desktop line break.

Software Centre review covered the shared identity, phone-width catalogue, search narrowing to MBLink and its package dialog. The original three WOFF2 files were compared byte-for-byte with the prior site assets. Automated checks preserve all twelve vehicles and 69 unique E-Class equipment codes. This is desktop Chrome and fixed CSS viewport evidence; no physical-device or Safari result is claimed.
