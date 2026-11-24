# Defer very large Baseline functions on Win32 and expose the existing x64 probe

## Scope and provenance

Existing accepted Win32 policy plus the existing experimental Win64 opt-in, which remains off by default. No permanent no-JIT limit.

Extracted from existing downstream commits: 6b3de3a31a, 972c3be633; existing stable x64 confirmation branch. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

For function bytecode above 1000000 bytes, defer Baseline compilation until warm-up exceeds 10000, with classification cached in JitScript. Top-level/smaller scripts retain the normal threshold. Both the interpreter-generated transition and C++ eligibility checks use the same existing policy. The x64 environment switch MYPAL_X64_LARGE_BASELINE_PROBE=1 is checked against the Windows AMD64 target in the normal configure layer, rather than depending on private Rust-target setup. Running on XP64 still needs the separate startup support draft #769.

## Existing measurements

Historical Win32 four-block live medians: first load 2018 to 1891 ms (6.3% less), restart load 2213.5 to 2122.5 ms (4.1%). Existing x64 stable-base fixture confirmation found 5.99% less restart load, but its first-navigation upper regression bound 3.08094% exceeded the registered 3% guard. A later frozen-current-input eight-run diagnostic found 1980 to 1855 ms (6.31% less), with four favourable pairs. This is not an accepted x64 uplift or an additional gain to add to 5.99%. Live noise and post-load guardrails remain open.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Downstream semantic, debugger, GC, tier-transition and hot-loop controls passed. One synthetic medium-function x64 timeline moved a roughly 230 ms compile pause to a later call; a normal-function control was about 2% (0.56 ms) slower. Do not claim free work elimination or general responsiveness improvement. Matching x64 PDBs show JitScript remains 200 bytes with unchanged existing offsets. The pure-policy boundary test is included. The x64 configure integration, full browser build and post-load/live validation of this extracted branch remain draft work.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
