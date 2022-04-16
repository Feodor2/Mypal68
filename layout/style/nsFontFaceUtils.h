/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* helper utilities for working with downloadable fonts */

#ifndef nsFontFaceUtils_h_
#define nsFontFaceUtils_h_

class gfxUserFontEntry;
class nsIFrame;

class nsFontFaceUtils {
 public:
  // mark dirty frames affected by a downloadable font
  static void MarkDirtyForFontChange(nsIFrame* aSubtreeRoot,
                                     const gfxUserFontEntry* aFont);
};

#endif /* !defined(nsFontFaceUtils_h_) */
