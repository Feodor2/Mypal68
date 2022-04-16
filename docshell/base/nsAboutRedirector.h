/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsAboutRedirector_h__
#define nsAboutRedirector_h__

#include "nsIAboutModule.h"

class nsAboutRedirector : public nsIAboutModule {
 public:
  NS_DECL_ISUPPORTS

  NS_DECL_NSIABOUTMODULE

  nsAboutRedirector() {}

  static nsresult Create(nsISupports* aOuter, REFNSIID aIID, void** aResult);

 protected:
  virtual ~nsAboutRedirector() {}
};

#endif  // nsAboutRedirector_h__
