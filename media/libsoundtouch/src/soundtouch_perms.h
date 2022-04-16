/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Include file for fixing symbol visibility on non-windows platforms, until
// system headers wrappers work uniformly across all of them.

#ifndef MOZILLA_SOUNDTOUCH_PERMS_H
#define MOZILLA_SOUNDTOUCH_PERMS_H

#pragma GCC visibility push(default)
#include "SoundTouch.h"
#include "SoundTouchFactory.h"
#pragma GCC visibility pop

#endif // MOZILLA_SOUNDTOUCH_PERMS_H
