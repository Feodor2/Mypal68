"""Compile the real DLL entry point against a small observable toolkit stub."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
COMPILER = Path(os.environ.get('XP64_TEST_CXX', 'g++.exe'))
DLL_MAIN = ROOT / 'toolkit/library/nsDllMain.cpp'

TOOLKIT_HEADER = r'''
/** @file nsToolkit.h
 * @brief Observable toolkit stub.
 * Responsibilities: count initialization and shutdown calls.
 * Invariants: module identity is preserved.
 * Thread-safety: single test thread.
 * Performance notes: correctness control only.
 */
#pragma once
#include <windows.h>
class nsToolkit {
 public:
  static HINSTANCE mDllInstance;
  static int startupCount;
  static int shutdownCount;
  static void Startup(HMODULE module);
  static void Shutdown();
};
'''

DRIVER = r'''
/**
 * @file toolkit_probe.cpp
 * @brief Exercises attach and explicit toolkit startup ordering.
 *
 * Responsibilities:
 *  - Call the actual XUL entry point with controlled notifications.
 *
 * Invariants:
 *  - Deferred startup must preserve the attached module handle.
 *
 * Thread-safety:
 *  - The probe runs on a single thread.
 *
 * Performance notes:
 *  - This is a correctness probe, not a benchmark.
 */
#include <windows.h>
#include "nsToolkit.h"

HINSTANCE nsToolkit::mDllInstance = nullptr;
int nsToolkit::startupCount = 0;
int nsToolkit::shutdownCount = 0;

void nsToolkit::Startup(HMODULE module) {
  mDllInstance = module;
  ++startupCount;
}

void nsToolkit::Shutdown() {
  ++shutdownCount;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
#if defined(MYPAL_XP64_STARTUP)
extern "C" void MypalXP64InitializeToolkit();
#endif

int main() {
  HINSTANCE module = GetModuleHandleW(nullptr);
  if (!module || !DllMain(module, DLL_PROCESS_ATTACH, nullptr)) return 1;
  if (nsToolkit::mDllInstance != module) return 2;
#if defined(MYPAL_XP64_STARTUP)
  if (nsToolkit::startupCount != 0) return 3;
  MypalXP64InitializeToolkit();
#endif
  if (nsToolkit::startupCount != 1) return 4;
  if (nsToolkit::mDllInstance != module) return 5;
  if (!DllMain(module, DLL_PROCESS_DETACH, nullptr)) return 6;
  if (nsToolkit::shutdownCount != 1) return 7;
  return 0;
}
'''


@unittest.skipUnless(COMPILER.is_file(), 'Configured MinGW compiler is unavailable')
class ToolkitStartupOrderingTests(unittest.TestCase):
    def test_original_and_deferred_startup_order(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'mozilla').mkdir()
            (root / 'nsToolkit.h').write_text(TOOLKIT_HEADER, encoding='ascii')
            (root / 'mozilla' / 'Assertions.h').write_text(
                '/** @file Assertions.h\n * @brief Assertion stub.\n * Responsibilities: validate contracts.\n * Invariants: assertions remain enabled.\n * Thread-safety: test thread only.\n * Performance notes: correctness only.\n */\n#pragma once\n#include <cassert>\n#define MOZ_RELEASE_ASSERT(x) assert(x)\n',
                encoding='ascii')
            (root / 'mozilla' / 'WindowsVersion.h').write_text('/** @file WindowsVersion.h\n * @brief Empty include stub.\n * Responsibilities: satisfy unused include.\n * Invariants: no runtime behavior.\n * Thread-safety: no state.\n * Performance notes: no code.\n */\n#pragma once\n', encoding='ascii')
            driver = root / 'toolkit_probe.cpp'
            driver.write_text(DRIVER, encoding='ascii')
            environment = os.environ.copy()
            environment['PATH'] = str(COMPILER.parent) + os.pathsep + environment['PATH']
            for deferred in (False, True):
                with self.subTest(deferred=deferred):
                    executable = root / ('deferred.exe' if deferred else 'original.exe')
                    command = [str(COMPILER), '-std=c++17', '-O2', '-I', str(root)]
                    if deferred:
                        command.append('-DMYPAL_XP64_STARTUP')
                    command.extend([str(DLL_MAIN), str(driver), '-o', str(executable)])
                    subprocess.run(command, check=True, env=environment, capture_output=True)
                    subprocess.run([str(executable)], check=True, env=environment, capture_output=True)


if __name__ == '__main__':
    unittest.main()
