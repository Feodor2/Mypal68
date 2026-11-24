# Migrate RKV store records in one transaction and propagate errors

## Scope and provenance

Shared vendored RKV migration correctness and transaction batching. Independent of the XP64 startup/LMDB slot configuration.

Extracted from existing downstream commits: 9a16e61762, e2f3688160. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Open stores before transactions, use one source reader and one destination writer for all store records, and commit once. Propagate database-list, iterator and put errors instead of panicking or silently stopping at an invalid record. This avoids committing earlier-store records when a later store fails. Store/schema creation is not claimed to be rolled back atomically.

## Existing measurements

This replaces per-store record commits with one record transaction. There is no measured browser page-load uplift. It was retained as an existing migration correctness fix during the x64 investigation.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

Included existing tests cover invalid values, failure after an earlier store and a two-store LMDB/SafeMode round trip. Re-run the vendored RKV migration tests and the clean browser build. Only edited migration/test checksum entries are regenerated. The checksum manifest also changes in #769; reconcile its disjoint entries when combining the PRs.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
