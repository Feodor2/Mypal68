/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * Other similar methods can be added if needed.
 */

#include "ThreeDPoint.h"
#include "WebAudioUtils.h"

namespace mozilla {

namespace dom {

bool ThreeDPoint::FuzzyEqual(const ThreeDPoint& other) {
  return WebAudioUtils::FuzzyEqual(x, other.x) &&
         WebAudioUtils::FuzzyEqual(y, other.y) &&
         WebAudioUtils::FuzzyEqual(z, other.z);
}

ThreeDPoint operator-(const ThreeDPoint& lhs, const ThreeDPoint& rhs) {
  return ThreeDPoint(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

ThreeDPoint operator*(const ThreeDPoint& lhs, const ThreeDPoint& rhs) {
  return ThreeDPoint(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

ThreeDPoint operator*(const ThreeDPoint& lhs, const double rhs) {
  return ThreeDPoint(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

bool operator==(const ThreeDPoint& lhs, const ThreeDPoint& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

}  // namespace dom
}  // namespace mozilla
