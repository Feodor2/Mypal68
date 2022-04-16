/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* an identifier for User Agent style sheets */

#ifndef mozilla_UserAgentStyleSheetID_h
#define mozilla_UserAgentStyleSheetID_h

namespace mozilla {

enum class UserAgentStyleSheetID : uint8_t {
#define STYLE_SHEET(identifier_, url_, shared_) identifier_,
#include "mozilla/UserAgentStyleSheetList.h"
#undef STYLE_SHEET
  Count
};

}  // namespace mozilla

#endif  // mozilla_UserAgentStyleSheetID_h
