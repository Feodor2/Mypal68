/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_SMILCompositorTable_h
#define mozilla_SMILCompositorTable_h

#include "nsTHashtable.h"

//----------------------------------------------------------------------
// SMILCompositorTable : A hashmap of SMILCompositors
//
// This is just a forward-declaration because it is included in
// SMILAnimationController which is used in Document. We don't want to
// expose all of SMILCompositor or otherwise any changes to it will mean the
// whole world will need to be rebuilt.

namespace mozilla {

class SMILCompositor;

typedef nsTHashtable<SMILCompositor> SMILCompositorTable;

}  // namespace mozilla

#endif  // mozilla_SMILCompositorTable_h
