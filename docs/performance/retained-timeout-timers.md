# Retain graphics sanity and Marionette polling timers until completion

## Scope and provenance

Existing cross-architecture timer lifetime fixes, not a claimed uplift.

Extracted from existing downstream commits: 1fa3198f83, e54bc16ffe. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Keep the graphics timeout on its listener and the window-polling timer on GeckoDriver. Clear references when callbacks run and cancel outstanding timers during test/session cleanup, so garbage collection cannot discard a function-local timer prematurely.

## Existing measurements

There is no reliable percentage speedup. The changes address timeout/progress behavior found during the existing XP investigation.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Downstream forced-GC timer controls passed with separate retained control timers. Those probes were not full graphics-sanity or session lifecycle end-to-end tests. Recheck timer cancellation, session teardown and GPU sanity behavior on this extracted build. Import compatibility changes and profiling hooks are excluded.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
