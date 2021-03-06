/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsDocShellModule_h
#define nsDocShellModule_h

#include "nscore.h"

namespace mozilla {

nsresult InitDocShellModule();

void UnloadDocShellModule();

}  // namespace mozilla

#endif
