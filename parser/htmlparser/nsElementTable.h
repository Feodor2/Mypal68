/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsElementTable_h
#define nsElementTable_h

#include "nsHTMLTags.h"

#ifdef DEBUG
void CheckElementTable();
#endif

struct nsHTMLElement {
  static bool IsContainer(nsHTMLTag aTag);
  static bool IsBlock(nsHTMLTag aTag);
};

#endif  // nsElementTable_h
