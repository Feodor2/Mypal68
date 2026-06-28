/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A namespace class for static content security utilities. */

#ifndef nsContentSecurityUtils_h___
#define nsContentSecurityUtils_h___

#include "nsStringFwd.h"

struct JSContext;
class nsIPrincipal;
class NS_ConvertUTF8toUTF16;

namespace mozilla {
namespace dom {
class Document;
}  // namespace dom
}  // namespace mozilla

class nsContentSecurityUtils {
 public:
  // CSPs upgrade-insecure-requests directive applies to same origin top level
  // navigations. Using the SOP would return false for the case when an https
  // page triggers and http page to load, even though that http page would be
  // upgraded to https later. Hence we have to use that custom function instead
  // of simply calling aTriggeringPrincipal->Equals(aResultPrincipal).
  static bool IsConsideredSameOriginForUIR(nsIPrincipal* aTriggeringPrincipal,
                                           nsIPrincipal* aResultPrincipal);

  static bool IsEvalAllowed(JSContext* cx, bool aIsSystemPrincipal,
                            const nsAString& aScript);
  static void NotifyEvalUsage(bool aIsSystemPrincipal,
                              NS_ConvertUTF8toUTF16& aFileNameA,
                              uint64_t aWindowID, uint32_t aLineNumber,
                              uint32_t aColumnNumber);

#if defined(DEBUG)
  static void AssertAboutPageHasCSP(mozilla::dom::Document* aDocument);
#endif
};

#endif /* nsContentSecurityUtils_h___ */
