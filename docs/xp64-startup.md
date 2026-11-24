# Windows XP x64 startup support

This series concerns native AMD64 Mypal on Windows XP Professional x64 SP2.
It contains no CPU tuning, JavaScript/CSS optimizations or profiling hooks.
Related report: <https://github.com/Feodor2/Mypal68/issues/35>.

## Loader and child startup fixes

The AMD64 interceptor can reserve a trampoline at the preferred image address
of XP64 USER32. In the observed failure, a child reserved `0x77CC0000`, both
USER32 load attempts returned error 487, and the child exited with code 128.
Randomized low-address searching now stops below `0x70000000` on pre-Vista
Windows. Vista and later retain the original limit; x86 does not compile this
function. The existing system-selected reservation fallback is retained.

The XP64 child also initializes its default C locale after its DLL blocklist
and before profiler/IPC worker startup. A separate UCRT control reproduced
an inversion between locale and loader locks during first initialization.
The existing XPCOM behavior of replacing the initial `C` locale is preserved.
Other OS versions and architectures retain their original startup ordering.

## Opt-in XP64 loader path

Add `ac_add_options --enable-xp64-startup` to an existing Windows AMD64
clang-cl mozconfig. Configure rejects this option for x86, other OS targets
and other compiler families. Normal builds keep their current loader path.

The option adds a normal link-time import of `XRE_GetBootstrap` from XUL
while retaining standalone XPCOM glue. XP64 can then initialize XUL's static
PE TLS in the initial loader graph. No PE rewriting or manual TLS callback
dispatch is used. Normal linking and packaging should produce the intended
launcher without a separate downstream relink script.

Since XUL now loads before `main`, its USER32-dependent imports and NSS's
WINMM import are delayed using the existing Windows delay-load mechanism.
DllMain records the XUL module handle; the bootstrap caller initializes
toolkit state before constructing the bootstrap implementation. The original
Rust TLS implementation is retained, without custom detach/cleanup hooks.

The opt-in configuration also selects LMDB's documented `MDB_NOTLS` reader
slots, preserving all caller flags. Host Rust recipes retain their own flags.
This does not change the default RKV flags in other configurations.

An XP64-compatible Rust standard library and C/C++ runtime remain toolchain
requirements. This series does not make a modern Windows-only runtime XP
compatible. An already installed `x86_64-xp-windows-msvc` Rust target can be
selected with `MYPAL_XP64_RUST_TARGET=x86_64-xp-windows-msvc` and its matching
`RUST_TARGET_PATH`. That override is rejected outside the XP64 startup mode.
The downstream investigation used LLVM 9.0.1, MSVC 14.25 and a custom Rust
1.45.2 standard library; reproducing that external toolchain is separate.

## Focused controls

Run `python build/win32/tests/test_xp64_config_guards.py` to exercise the
actual configure guard function bodies. This checks target restrictions,
not the full configure dependency graph. On Windows with an x64 MinGW C++
compiler, set `XP64_TEST_CXX` to its executable and run
`python build/win32/tests/test_xp64_toolkit_startup.py`. It compiles the real
DllMain against an observable toolkit stub and checks both ordinary and
deferred attach/bootstrap/detach ordering. It is not a full browser build.

## Validation scope

These source mechanisms were tested in a separate downstream build: ordinary
and instrumented binaries independently completed 1800 tab cycles over 30
minutes on a physical E8300/XP64 SP2 machine. The ordinary binary also passed
YouTube/Interia navigation, Canvas 2D, WebGL and 60 seconds of muted 720p
playback. This is not exhaustive browser, audio, hardware or OS coverage.

Those binaries also contained unrelated downstream changes. Their results
are evidence for the investigation, not a claim that this clean upstream
series has already passed the same complete build and runtime validation.
Keep the pull request in draft until that independent validation is complete.

Before marking ready for review:

- Build this clean series with the XP64-capable toolchain, without the
  unrelated downstream optimization stack.
- Inspect AMD64 imports: the launcher imports `XRE_GetBootstrap`; XUL and
  NSS expose the expected delayed system imports; packaged EXE matches it.
- Repeat the tab-cycle and functional checks above on physical XP64.
- Check ordinary x86 configuration/build for regressions and validate the
  opted-in AMD64 binary on a supported later Windows installation.

The normal-build integration and unified option are new in this draft;
previous downstream results do not substitute for those outstanding checks.
