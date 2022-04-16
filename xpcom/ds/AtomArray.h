/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_AtomArray_h
#define mozilla_AtomArray_h

#include "mozilla/RefPtr.h"
#include "nsTArray.h"

class nsAtom;

namespace mozilla {
typedef nsTArray<RefPtr<nsAtom>> AtomArray;
}

#endif  // mozilla_AtomArray_h
