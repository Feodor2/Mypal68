/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Hal.h"

using mozilla::hal::WindowIdentifier;

namespace mozilla {
namespace hal_impl {

void Vibrate(const nsTArray<uint32_t>& pattern, const hal::WindowIdentifier&) {}

void CancelVibrate(const hal::WindowIdentifier&) {}

}  // namespace hal_impl
}  // namespace mozilla
