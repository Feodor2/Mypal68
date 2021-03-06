/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef TOOLKIT_XRE_UIKITDIRPROVIDER_H_
#define TOOLKIT_XRE_UIKITDIRPROVIDER_H_

#include "nsString.h"

bool GetUIKitDirectory(bool aLocal, nsACString& aUserDir);

#endif  // TOOLKIT_XRE_UIKITDIRPROVIDER_H_
