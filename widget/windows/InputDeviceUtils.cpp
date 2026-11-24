/**
 * @file InputDeviceUtils.cpp
 * @brief Enumerate and monitor Windows input device interfaces.
 *
 * Responsibilities:
 *  - Register HID interface notifications and count active mouse interfaces.
 * Invariants:
 *  - Enumeration advances for every interface, including inactive ones.
 *  - Every successfully opened device information list is released.
 * Thread-safety:
 *  - Each enumeration owns local SetupAPI state; cache access is atomic.
 * Performance notes:
 *  - Successful counts are cached until a device or theme change.
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "InputDeviceUtils.h"
#include "MouseDeviceCountCache.h"

#define INITGUID
#include <dbt.h>
#include <hidclass.h>
#include <ntddmou.h>
#include <setupapi.h>

namespace mozilla {
namespace widget {

HDEVNOTIFY
InputDeviceUtils::RegisterNotification(HWND aHwnd) {
  DEV_BROADCAST_DEVICEINTERFACE filter = {};

  filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
  filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
  // Some touchsreen devices are not GUID_DEVINTERFACE_MOUSE, so here we use
  // GUID_DEVINTERFACE_HID instead.
  filter.dbcc_classguid = GUID_DEVINTERFACE_HID;
  return RegisterDeviceNotification(aHwnd, &filter,
                                    DEVICE_NOTIFY_WINDOW_HANDLE);
}

void InputDeviceUtils::UnregisterNotification(HDEVNOTIFY aHandle) {
  if (!aHandle) {
    return;
  }
  UnregisterDeviceNotification(aHandle);
}

static MouseDeviceCountCache sMouseDeviceCountCache;

static MouseDeviceCountResult EnumerateMouseDevices() {
  HDEVINFO hdev =
      SetupDiGetClassDevs(&GUID_DEVINTERFACE_MOUSE, nullptr, nullptr,
                          DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
  if (hdev == INVALID_HANDLE_VALUE) {
    return {0, false};
  }

  DWORD count = 0;
  DWORD index = 0;
  SP_INTERFACE_DEVICE_DATA info = {};
  info.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
  while (SetupDiEnumDeviceInterfaces(hdev, nullptr, &GUID_DEVINTERFACE_MOUSE,
                                     index, &info)) {
    index++;
    if (info.Flags & SPINT_ACTIVE) {
      count++;
    }
  }
  const DWORD error = GetLastError();
  SetupDiDestroyDeviceInfoList(hdev);
  return {count, error == ERROR_NO_MORE_ITEMS};
}

DWORD InputDeviceUtils::CountMouseDevices() {
  return sMouseDeviceCountCache.Get(EnumerateMouseDevices);
}

void InputDeviceUtils::InvalidateMouseDeviceCount() {
  sMouseDeviceCountCache.Invalidate();
}

}  // namespace widget
}  // namespace mozilla
