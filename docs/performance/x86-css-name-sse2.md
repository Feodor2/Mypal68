# Scan ASCII CSS name prefixes with bounded SSE2 loads on x86

## Scope and provenance

target_arch=x86 and target_feature=sse2 only, including non-Windows x86 targets. x64 and non-SSE2 targets retain the scalar implementation.

Extracted from existing downstream commits: 0bd6abc833. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Skip full or partial 16-byte ASCII name prefixes before the existing scalar tokenizer. Loads remain within input bounds; escapes, UTF-8 and delimiters fall through to the existing implementation.

## Existing measurements

On the captured YouTube stylesheet the downstream isolated tokenizer median improved from 47.845 to 41.030 ms (14.24%). Live restart load 2231.5 to 2207 ms had eight favourable and eight unfavourable pairs. There is no established page-load uplift. The later x64 port was rejected and is deliberately not part of this PR.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Earlier downstream boundary/guard-page and CSSOM controls passed. Run the vendored cssparser tests with an x86 SSE2 target plus scalar/non-x86 controls on this clean branch. The vendored checksum is checked against the exact edited tokenizer bytes during export; a clean full browser build remains outstanding.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
