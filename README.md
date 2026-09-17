# ssmithnet.net

Personal website for Shannon Smith — a quiet corner of the internet that has been around since the 1990s.

The deployed site is deliberately boring at runtime: static HTML, local assets and no server-side application. The interesting part happens before deployment.

`src/sitegen.cpp` is the authoritative deterministic C++17 site generator. It owns the shared page structure, navigation, standardised car cards and repeated presentation logic for Home, Workbench, Garage and Archive.

GitHub Actions builds that generator on an Ubuntu runner, validates its output, generates a clean `public/` tree, adds the local assets and custom-domain file, and deploys that tree directly as a GitHub Pages artifact. Generated HTML is therefore a deployment artefact, not source code committed back into `main`.

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
