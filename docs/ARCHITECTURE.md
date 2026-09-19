# Architecture

## Purpose

ssmithnet.net is a deterministic static personal website generated from first-party C++ source and published as ordinary HTML and local assets.

## System decomposition

- C++17 site generator
- shared page/navigation/card composition
- generated HTML review mirrors
- local assets and typography
- Common web/design/publication primitives
- GitHub Pages deployment workflow
- site-generation regression test

## Ownership boundaries

The C++ generator is the source of site structure and repeated presentation logic. Generated HTML is review output. GitHub Pages hosts the result but does not define site semantics; Common owns only product-neutral web primitives.

Platform APIs, reference implementations and first-party shared libraries are mechanisms or evidence behind explicit boundaries. They must not silently redefine product behaviour.

## Source of truth

Code and tests define executable behaviour. Generated output is authoritative only where the build explicitly defines it as an artifact; editable source remains the owning source of truth.

## Change discipline

Keep game/site/calculation domain logic independent from rendering/toolkit/hosting details where practical. Reuse shared first-party primitives without moving product-specific policy into Common.

## Specialist documentation

- DESIGN.md
