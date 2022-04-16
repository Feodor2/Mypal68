/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CUBEB_ASSERT
#define CUBEB_ASSERT

#include <stdio.h>
#include <stdlib.h>
#include <mozilla/Assertions.h>

/* Forward fatal asserts to MOZ_RELEASE_ASSERT when built inside Gecko. */
#define XASSERT(expr) MOZ_RELEASE_ASSERT(expr)

#endif
