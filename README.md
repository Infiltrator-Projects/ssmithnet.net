# ssmithnet.net

Personal website for Shannon Smith — a quiet corner of the internet that has been around since the 1990s.

The deployed site is deliberately boring: static HTML, local MB Corpo WOFF2 fonts, local graphics and no external runtime dependencies.

The source is not a pile of hand-maintained HTML. `src/sitegen.cpp` is the deterministic C++ site generator. It owns the shared page structure, navigation, standardised car cards and repeated presentation logic, while the generated `index.html`, `workbench.html`, `garage.html` and `archive.html` remain the published artefacts.

Build locally with:

```text
make
```

Validation with:

```text
make check
```

GitHub Actions regenerates the published pages when the source, assets or design documentation changes. This keeps the website simple for visitors while keeping the implementation maintainable for a C++ programmer.
