/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsMIMEInfoUIKit_h_
#define nsMIMEInfoUIKit_h_

#include "nsMIMEInfoImpl.h"

class nsMIMEInfoUIKit final : public nsMIMEInfoImpl {
 public:
  explicit nsMIMEInfoUIKit(const nsACString& aMIMEType)
      : nsMIMEInfoImpl(aMIMEType) {}
  nsMIMEInfoUIKit(const nsACString& aType, HandlerClass aClass)
      : nsMIMEInfoImpl(aType, aClass) {}

  NS_IMETHOD LaunchWithFile(nsIFile* aFile) override;

 protected:
  virtual nsresult LoadUriInternal(nsIURI* aURI);
#ifdef DEBUG
  virtual nsresult LaunchDefaultWithFile(nsIFile* aFile) {
    MOZ_ASSERT_UNREACHABLE("do not call this method, use LaunchWithFile");
    return NS_ERROR_UNEXPECTED;
  }
#endif
};

#endif
