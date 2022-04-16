/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_GamepadServiceType_h_
#define mozilla_dom_GamepadServiceType_h_

namespace mozilla {
namespace dom {

// Standard channel is used for managing gamepads that
// are from GamepadPlatformService. VR channel
// is for gamepads that are from VRManagerChild.
enum class GamepadServiceType : uint16_t {
  Standard,
  VR,
  NumGamepadServiceType
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_GamepadServiceType_h_
