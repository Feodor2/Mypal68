/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* thread-safe container of information for resolving url values */

#include "mozilla/URLExtraData.h"

#include "mozilla/NullPrincipalURI.h"
#include "nsProxyRelease.h"
#include "ReferrerInfo.h"

namespace mozilla {

StaticRefPtr<URLExtraData> URLExtraData::sDummy;
StaticRefPtr<URLExtraData> URLExtraData::sDummyChrome;

/* static */
void URLExtraData::Init() {
  RefPtr<nsIURI> baseURI = new NullPrincipalURI();
  nsCOMPtr<nsIReferrerInfo> referrerInfo = new dom::ReferrerInfo(nullptr);
  sDummy = new URLExtraData(do_AddRef(baseURI), do_AddRef(referrerInfo),
                            NullPrincipal::CreateWithoutOriginAttributes());

  sDummyChrome =
      new URLExtraData(baseURI.forget(), referrerInfo.forget(),
                       NullPrincipal::CreateWithoutOriginAttributes());
  sDummyChrome->mChromeRulesEnabled = true;
}

/* static */
void URLExtraData::Shutdown() {
  sDummy = nullptr;
  sDummyChrome = nullptr;
}

URLExtraData::~URLExtraData() {
  if (!NS_IsMainThread()) {
    NS_ReleaseOnMainThreadSystemGroup("URLExtraData::mPrincipal",
                                      mPrincipal.forget());
  }
}

StaticRefPtr<URLExtraData>
    URLExtraData::sShared[size_t(UserAgentStyleSheetID::Count)];

}  // namespace mozilla
