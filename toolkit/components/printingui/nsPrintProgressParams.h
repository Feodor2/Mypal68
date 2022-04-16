/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nsPrintProgressParams_h
#define __nsPrintProgressParams_h

#include "nsIPrintProgressParams.h"
#include "nsString.h"

class nsPrintProgressParams : public nsIPrintProgressParams {
 public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPRINTPROGRESSPARAMS

  nsPrintProgressParams();

 protected:
  virtual ~nsPrintProgressParams();

 private:
  nsString mDocTitle;
  nsString mDocURL;
};

#endif
