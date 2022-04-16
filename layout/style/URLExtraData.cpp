/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* thread-safe container of information for resolving url values */

#include "mozilla/URLExtraData.h"

#include "mozilla/NullPrincipalURI.h"
#include "nsProxyRelease.h"
#include "mozilla/net/ReferrerPolicy.h"

namespace mozilla {

StaticRefPtr<URLExtraData> URLExtraData::sDummy;

/* static */
void URLExtraData::InitDummy() {
  RefPtr<nsIURI> baseURI = NullPrincipalURI::Create();
  RefPtr<nsIURI> referrer = baseURI;
  sDummy = new URLExtraData(baseURI.forget(), referrer.forget(),
                            NullPrincipal::CreateWithoutOriginAttributes(),
                            net::RP_Unset);
}

/* static */
void URLExtraData::ReleaseDummy() { sDummy = nullptr; }

URLExtraData::~URLExtraData() {
  if (!NS_IsMainThread()) {
    NS_ReleaseOnMainThreadSystemGroup("URLExtraData::mPrincipal",
                                      mPrincipal.forget());
  }
}

StaticRefPtr<URLExtraData>
    URLExtraData::sShared[size_t(UserAgentStyleSheetID::Count)];

}  // namespace mozilla
