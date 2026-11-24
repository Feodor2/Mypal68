# Enable eligible speculative script parsing by default on Windows x86

## Scope and provenance

All 32-bit Windows application builds, not an E8300-only setting. x64 and other OS defaults are unchanged.

Extracted from existing downstream commits: 8fccb1218e. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Package the existing main speculative parsing preference as an application default. Async, link preload and non-parser-inserted speculation remain disabled. User preference overrides still work.

## Existing measurements

Four downstream live blocks using identical XUL but different app preferences reported restart load 2271 to 2077 ms (8.5% less), while visibility was 3372 to 3494.5 ms. Four independently linked source-build blocks reported 2219.5 to 2208 ms (about 0.5% less), with four of eight favourable load pairs. The full-build result does not establish the larger overlay gain.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Earlier downstream Interia, SRI, bytecode restart and playback controls passed. Keep async/preload categories off; enabling both previously caused Interia timeouts. Check generated defaults, user rollback and cross-site behavior on this clean build. The async-completion fix is a separate draft and is not bundled into this default change.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
