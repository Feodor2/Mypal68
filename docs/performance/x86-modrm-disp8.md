# Pack x86 ModRM and disp8 bytes into one assembler append

## Scope and provenance

JS_CODEGEN_X86 only. The existing JS_CODEGEN_X64 path is retained.

Extracted from existing downstream commits: ee73a94882. Upstream base: `71878a48bbc0353a6ca3d7a8f7b59a6b953fd5f3`. No new optimization experiment is introduced.

## Change

Build the ModRM byte and signed 8-bit displacement as a packed little-endian pair and use one existing unchecked short append. The caller continues to reserve buffer capacity.

## Existing measurements

Downstream mixed-emission microbenchmarks took 2.63 to 2.72% less time. A fixture screen reported 1.84% shorter load and a small live series 2202 to 2090 ms (about 5.1%). Independent binary layout and live-site noise prevent attributing the entire live difference to this patch.

These are historical downstream results from a larger source stack on a Core 2 Duo E8300 / Windows XP x64 SP2 host. They are not new measurements of this clean branch, and percentages from different experiments must not be added together.

## Validation and draft status

The included test uses the production assembler and checks reference bytes for every base/destination register, offsets -129 through 128 and four wide boundaries: 16768 operands. Earlier downstream encoding/playback checks passed. Compile with an x86-configured Mypal object directory and run on the extracted build; full clean build validation is still outstanding.

The public diff contains only this change, its related tests and this note. Lab automation, profiles, cookies, SDK/runtime patches, CPU tuning, diagnostics and unrelated optimizations are excluded.
