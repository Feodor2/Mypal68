/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "DDTimeStamp.h"

namespace mozilla {

double ToSeconds(const DDTimeStamp& aTimeStamp) {
  // Timestamp at first call, used internally to convert log timestamps
  // to a duration from this timestamp.
  // What's important is the relative time between log messages.
  static const DDTimeStamp sInitialTimeStamp = TimeStamp::Now();

  return (aTimeStamp - sInitialTimeStamp).ToSeconds();
}

}  // namespace mozilla
