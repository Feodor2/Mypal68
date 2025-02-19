/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

/*
 * Includes <wrl.h> and it's children. Defines imports needed by
 * corewrappers.h in the case where windows.h has already been
 * included w/WINVER < 0x600. Also ups WINVER/_WIN32_WINNT prior
 * to including wrl.h. Mozilla's build currently has WINVER set to
 * 0x502 for XP support.
 */

#if _WIN32_WINNT < 0x600

#include <windows.h>

BOOL
WINAPI
InitializeCriticalSectionEx(
    _Out_ LPCRITICAL_SECTION lpCriticalSection,
    _In_ DWORD dwSpinCount,
    _In_ DWORD Flags
    );

#undef WINVER
#undef _WIN32_WINNT
#define WINVER 0x600
#define _WIN32_WINNT 0x600

#endif // _WIN32_WINNT < 0x600

#include <wrl.h>
