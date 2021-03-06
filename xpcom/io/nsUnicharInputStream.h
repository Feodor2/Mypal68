/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsUnicharInputStream_h__
#define nsUnicharInputStream_h__

#include "nsIUnicharInputStream.h"

nsresult NS_NewUnicharInputStream(nsIInputStream* aStreamToWrap,
                                  nsIUnicharInputStream** aResult);

#endif  // nsUnicharInputStream_h__
