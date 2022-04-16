/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_widget_InputDeviceUtils_h__
#define mozilla_widget_InputDeviceUtils_h__

#include <windows.h>

namespace mozilla {
namespace widget {

class InputDeviceUtils {
 public:
  static HDEVNOTIFY RegisterNotification(HWND aHwnd);
  static void UnregisterNotification(HDEVNOTIFY aHandle);

  // Returns the number of mouse type devices connected to this system.
  static DWORD CountMouseDevices();
};

}  // namespace widget
}  // namespace mozilla
#endif  // mozilla_widget_InputDeviceUtils_h__
