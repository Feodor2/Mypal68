# Cache very large scripts earlier with a hybrid bytecode policy on Windows

## Scope and provenance

Existing hybrid policy and Windows x86/x64 default -5. Other platforms retain strategy 0; users can restore 0. No LZ4 format or tracing is included.

Extracted from existing downstream commits: f8325c6de2, a050971ade, e249ff2eaa, f4d97d5334. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

For -5, retain the existing strategy-0 minimum length and fetch threshold for smaller scripts, but bypass the fetch threshold at 4 Mi source units. UTF-8 units are bytes, UTF-16 units are 16-bit code units. The pre-existing downstream opt-in large-only -3 policy is retained. Encoding eligibility, cache channels, SRI and bytecode format remain on the existing loader paths.

## Existing measurements

Small downstream x86 live restart series reported about 26.3% and 25.0% less load time versus strategy 0. Two x64 original-Rust-runtime screens reported 30.27% and 26.68%. These are revisit/cache results, not cold startup or universal page gains. Early encoding increases cache size and process I/O; x86 preparation process-write medians were about 12.6 MB versus 51.0 MB in a four-per-variant diagnostic. Some x64 live series had substantial noise, and first-visit regressions are not fully resolved.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Included compile-time boundary controls cover the pure policy. Downstream XP x86/x64 tests covered 1023/1024-byte thresholds, small and >4 MiB sources, restarts, selected HTTP/SRI/corruption and module fallback cases. These are limited cases, not a new cache-integrity guarantee. Rebuild this isolated branch and repeat functional and first-visit/I/O/performance guardrails before review-ready status.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
