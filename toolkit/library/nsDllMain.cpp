/**
 * @file nsDllMain.cpp
 * @brief Handle XUL loader notifications and deferred XP64 toolkit startup.
 * Responsibilities:
 *  - Save the module handle before explicit bootstrap initialization.
 * Invariants:
 *  - XP64 toolkit startup occurs outside DllMain exactly once via bootstrap.
 * Thread-safety:
 *  - DllMain runs under the loader lock; bootstrap runs on its caller thread.
 * Performance notes:
 *  - Deferral is enabled only by the explicit XP64 build option.
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <windows.h>
#include "nsToolkit.h"
#include "mozilla/Assertions.h"
#include "mozilla/WindowsVersion.h"

#if defined(MYPAL_XP64_STARTUP)
#  if !defined(_WIN64)
#    error "XP64 startup requires the Win64 target"
#  endif
/** Initialize toolkit state on the bootstrap caller, outside DllMain. */
extern "C" void MypalXP64InitializeToolkit() {
  MOZ_RELEASE_ASSERT(nsToolkit::mDllInstance);
  nsToolkit::Startup(nsToolkit::mDllInstance);
}
#endif

#if defined(__GNUC__)
// If DllMain gets name mangled, it won't be seen.
extern "C" {
#endif

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD reason, LPVOID lpReserved) {
  switch (reason) {
    case DLL_PROCESS_ATTACH:
#if defined(MYPAL_XP64_STARTUP)
      nsToolkit::mDllInstance = hModule;
#else
      nsToolkit::Startup((HINSTANCE)hModule);
#endif
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;

    case DLL_PROCESS_DETACH:
      nsToolkit::Shutdown();
      break;
  }

  return TRUE;
}

#if defined(__GNUC__)
}  // extern "C"
#endif
