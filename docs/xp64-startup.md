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
