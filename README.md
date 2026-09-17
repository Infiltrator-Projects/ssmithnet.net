# ssmithnet.net

Personal website for Shannon Smith — a quiet corner of the internet that has been around since the 1990s.

The deployed site is deliberately boring at runtime: static HTML, local assets and no server-side application. The interesting part happens before deployment.

`src/sitegen.cpp` is the authoritative deterministic C++17 site generator. It owns the shared page structure, navigation, standardised car cards and repeated presentation logic for Home, Workbench, Garage and Archive.

Shared neutral presentation and infrastructure come from Infiltratr Common rather than being reimplemented here. COMMON owns the canonical graphite/silver design tokens and web adapter, generic output escaping, generic durable file publication and the product-neutral GitHub Pages deployment action. This site owns its personal content, page composition, Workbench/Garage/Archive information architecture, local MB Corpo font assets, cyan/warm-material treatment and original graphics.

GitHub Actions builds that generator on an Ubuntu runner, validates its output, generates a clean `public/` tree, adds the local assets and custom-domain file, and hands that directory to COMMON's reusable Pages deployment action. While the repository is still configured for legacy branch-based Pages as a compatibility fallback, generated HTML may also be kept on `main`; once the repository Pages source is switched to GitHub Actions, those checked-in generated copies can be removed.

Build locally with:

```text
make
```

Validation with:

```text
make check
```

Generate the four pages into an explicit directory with:

```text
./build/sitegen OUTPUT_DIRECTORY
```

This is deliberately the same Pages model used by the Infiltrator Software Centre: source is built on a temporary GitHub Actions Linux runner and only the finished static artifact is served to visitors.

The public site is `https://ssmithnet.net/`.
