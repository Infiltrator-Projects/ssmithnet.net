# Design

## First-principles position

ssmithnet.net begins with the experience and behaviour it must own. Existing products, frameworks and techniques are evidence to study, not specifications that must be copied.

## Goals

- keep the deployed site simple and dependency-light
- make generated output deterministic and reviewable
- own content structure and presentation rather than depend on a web framework runtime
- reuse exact first-party Common primitives where they are genuinely shared

## Non-goals

The project is not intended to become a server-side application or framework-dependent SPA. Generated mirrors are not independently edited sources.

## Dependency and language policy

Prefer first-party C/C++ for native/core logic where appropriate. Use platform-native code at real platform boundaries and exact first-party shared dependencies for generic primitives. Dependencies must not become hidden owners of product semantics.

## Failure and quality

Invalid, unavailable and unsupported states are explicit. Persistent state should be durable and recoverable at the level promised by the product. A design change should improve correctness, fidelity, usability, performance, resilience or maintainability; newness alone is not a benefit.

## Completion quality

A feature is complete when behaviour, edge cases, tests and documentation agree, not simply when the common case renders or calculates once.
