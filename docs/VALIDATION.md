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

The deterministic gate covers all five generated pages, including the download landing page. `tests/check_web_family.py .` additionally checks the common navigation, canonical/Open Graph metadata, unique landmarks, asset references and the three WOFF2 fonts. Package Repository runs this same contract against its generated `public/` tree.
