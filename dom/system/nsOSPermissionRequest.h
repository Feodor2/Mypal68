/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsOSPermissionRequest_h__
#define nsOSPermissionRequest_h__

#include "nsOSPermissionRequestBase.h"

/*
 * The default implementation of nsOSPermissionRequestBase used on platforms
 * that don't have a platform-specific version.
 */
class nsOSPermissionRequest : public nsOSPermissionRequestBase {};

#endif /* nsOSPermissionRequest_h__ */
