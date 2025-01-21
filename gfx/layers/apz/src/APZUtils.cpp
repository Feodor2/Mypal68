/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/layers/APZUtils.h"

namespace mozilla {
namespace layers {

namespace apz {

/*static*/ bool IsCloseToHorizontal(float aAngle, float aThreshold) {
  return (aAngle < aThreshold || aAngle > (M_PI - aThreshold));
}

/*static*/ bool IsCloseToVertical(float aAngle, float aThreshold) {
  return (fabs(aAngle - (M_PI / 2)) < aThreshold);
}

}  // namespace apz
}  // namespace layers
}  // namespace mozilla
