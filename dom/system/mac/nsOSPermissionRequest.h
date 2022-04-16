/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsOSPermissionRequest_h__
#define nsOSPermissionRequest_h__

#include "nsOSPermissionRequestBase.h"

class nsOSPermissionRequest : public nsOSPermissionRequestBase {
 public:
  nsOSPermissionRequest(){};

  NS_IMETHOD GetAudioCapturePermissionState(uint16_t* aAudio) override;

  NS_IMETHOD GetVideoCapturePermissionState(uint16_t* aVideo) override;

  NS_IMETHOD RequestVideoCapturePermission(
      JSContext* aCx, mozilla::dom::Promise** aPromiseOut) override;

  NS_IMETHOD RequestAudioCapturePermission(
      JSContext* aCx, mozilla::dom::Promise** aPromiseOut) override;
};

#endif
