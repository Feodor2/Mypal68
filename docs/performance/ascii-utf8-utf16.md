# Compare ASCII prefixes directly in UTF-8 to UTF-16 string comparisons

## Scope and provenance

Architecture-independent string comparison; inherited by x86 and x64 builds.

Extracted from existing downstream commits: 23665ff636, 5032e91438, 40c8c14d60. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Advance equal ASCII code units without Unicode decoding and order unequal ASCII scalars directly. Non-ASCII and malformed input retain the existing decoders and accumulated error behavior.

## Existing measurements

One downstream isolated equal-128-character ASCII comparison measured 81.64 to 24.25 ms (about 70.3% less time). Fixture page results were mixed. No isolated page-load uplift is established.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Included TestEncoding additions exercise every ASCII comparison pair, prefixes, non-ASCII transitions and error behavior. Downstream UTF controls passed; run the gtest suite on a clean build of this branch before treating it as independently validated.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
