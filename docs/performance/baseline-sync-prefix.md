# Skip the synchronized prefix when spilling the Baseline virtual stack

## Scope and provenance

Architecture-independent Baseline compiler code; measured on Windows x86/XP64.

Extracted from existing downstream commits: d8f4ed75dc. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Find the unsynchronized suffix by walking backward, then spill only that suffix. A debug assertion checks that the skipped prefix contains only Stack values. Observable evaluation order and spilling semantics stay intact.

## Existing measurements

The downstream microbenchmark took approximately 40% less time. Two live YouTube release blocks reported median restart load 2270.5 to 2205 ms (about 2.9% less), with opposite directions in individual series. This small live-site result is not a guaranteed isolated page gain.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Earlier downstream checks covered nested expression/evaluation order, exceptions, generators and playback. The standalone xpcshell regression script is included. This extracted branch still needs a full browser build and execution of that script on its own binary.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
