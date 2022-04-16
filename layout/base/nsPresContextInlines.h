/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsPresContextInlines_h
#define nsPresContextInlines_h

#include "mozilla/PresShell.h"
#include "nsCSSFrameConstructor.h"

inline mozilla::ServoStyleSet* nsPresContext::StyleSet() const {
  return mDocument->StyleSetForPresShellOrMediaQueryEvaluation();
}

inline nsCSSFrameConstructor* nsPresContext::FrameConstructor() {
  return PresShell()->FrameConstructor();
}

#endif  // #ifndef nsPresContextInlines_h
