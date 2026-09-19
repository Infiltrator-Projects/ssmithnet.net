# ssmithnet.net

Personal website for Shannon Smith — a quiet corner of the internet that has been around since the 1990s.

The deployed site is deliberately boring at runtime: static HTML, local assets and no server-side application. The interesting part happens before deployment.

`src/sitegen.cpp` is the authoritative deterministic C++17 site generator. It owns the shared page structure, navigation, standardised car cards and repeated presentation logic for Home, Workbench, Garage and Archive.

Shared neutral presentation and infrastructure come from the immutable Infiltratr Common 1.19.3 release (`de7251ce12ed176048df1bad05ef7e4d0db7e9ec`) rather than being reimplemented here. COMMON owns the canonical graphite/silver design tokens and web adapter, generic output escaping, generic durable file publication and the product-neutral GitHub Pages deployment action. This site owns its personal content, page composition, Workbench/Garage/Archive information architecture, local MB Corpo font assets, cyan/warm-material treatment and original graphics.

GitHub Pages is published from the generated GitHub Actions artifact. Actions builds the generator on an Ubuntu runner, verifies the committed COMMON 1.19.3 web snapshot before the build modifies it, validates the generated site, creates a clean `public/` tree, adds the local assets and custom-domain file, and hands that directory to COMMON's reusable Pages deployment action.

The four HTML files kept on `main` are generated review mirrors, not source. Do not edit them by hand. `make check` regenerates the site and requires those mirrors to match `src/sitegen.cpp` byte-for-byte so reviewers can inspect the rendered output without creating a second source of truth.

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

## Engineering ethos

What does a personal website look like when it is built from first principles, the public runtime is kept deliberately simple, and the site remains reproducible from owned source? ssmithnet.net treats generated static pages as an output, not as the source of truth.

The authoritative C++ generator owns structure, navigation and repeated presentation logic. Common contributes exact pinned first-party primitives where the contract is genuinely shared; GitHub Pages is a delivery mechanism rather than an application dependency. The site remains inspectable as ordinary static HTML and local assets, so a change in a framework or hosted runtime cannot redefine the content model.

The project prefers durable, understandable machinery over fashionable web stacks. A dependency is added because it provides a stronger contract, not merely because it is newer or convenient, and generated output must remain deterministic and reviewable.

