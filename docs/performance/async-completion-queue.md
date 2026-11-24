# Move completed off-thread async scripts to the loaded queue

## Scope and provenance

Shared script-loader correctness fix, not a default async-speculation change.

Extracted from existing downstream commits: a335716dcb. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

After marking an off-thread classic request ready, move a still-linked async request from mLoadingAsyncRequests to mLoadedAsyncRequests before processing ready queues. Requests already removed for compilation are left alone; a debug assertion checks the expected source list.

## Existing measurements

The prior downstream binary stalled a 4 MiB async fixture with speculative async enabled; the fixed binary completed it and produced result 1000. This is a correctness repair, not a claimed page-load speedup. Default async speculation remains disabled.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Downstream guarded XP controls covered the async fixture, Interia, valid/invalid SRI and short muted playback. Earlier observations with possible background-browser interference are not clean performance evidence. Reproduce with a large HTTP classic async script and both speculative_omt_parse.enabled and speculate_async.enabled enabled; verify load, execution, SRI/error handling and clean shutdown on a new build of this isolated branch.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
