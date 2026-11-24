# Cache Windows mouse enumeration with generation-based invalidation

## Scope and provenance

Windows x86/x64 input-capability queries, without CPU tuning.

Extracted from existing downstream commits: 526c881bbf, c74889a3ac, c5cce637a9. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Advance the SetupAPI interface index independently of active mouse count, release the device list and cache only successful complete enumerations. Use an aligned lock-free generation/count state so invalidation rejects stale fills. Device/session/theme changes invalidate the cache; generation saturation disables caching rather than wrapping.

## Existing measurements

A downstream fixture screen reported median restart load 2029.5 to 1961.5 ms (about 3.4% less). Live load changed from 2147.5 to 2202.5 ms while visibility moved in the opposite direction. No reliable live page uplift is claimed; the enumeration correctness and avoided repeat work are the primary reasons for the downstream adoption.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

The included production-header test covers cache hits, failed enumeration, competing fills, generation saturation and invalidation during an in-flight enumeration. Earlier downstream mocked enumeration and race checks passed. Compile the standalone test with C++17, MYPAL_MOUSE_CACHE_TEST, the widget include path and generated Mypal headers; test real device/session invalidation on the clean browser.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.

## Export control

The extracted production-header race test was compiled with the local MinGW C++17 compiler and returned `MOUSE_COUNT_CACHE_TEST_OK`, exit 0, on the developer Windows x64 host. It used the matching existing generated Mypal include directory. This checks the exported cache logic, not browser device notifications or a clean XP browser build.
