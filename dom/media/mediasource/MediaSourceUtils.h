/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_MEDIASOURCEUTILS_H_
#define MOZILLA_MEDIASOURCEUTILS_H_

#include "nsString.h"
#include "TimeUnits.h"

namespace mozilla {

nsCString DumpTimeRanges(const media::TimeIntervals& aRanges);

}  // namespace mozilla

#endif /* MOZILLA_MEDIASOURCEUTILS_H_ */
