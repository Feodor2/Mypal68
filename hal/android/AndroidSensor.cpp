/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Hal.h"
#include "GeneratedJNIWrappers.h"

using namespace mozilla::hal;

namespace mozilla {
namespace hal_impl {

void EnableSensorNotifications(SensorType aSensor) {
  java::GeckoAppShell::EnableSensor(aSensor);
}

void DisableSensorNotifications(SensorType aSensor) {
  java::GeckoAppShell::DisableSensor(aSensor);
}

}  // namespace hal_impl
}  // namespace mozilla
