/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_a11y_ImageAccessibleWrap_h__
#define mozilla_a11y_ImageAccessibleWrap_h__

#include "ImageAccessible.h"

namespace mozilla {
namespace a11y {

class ImageAccessibleWrap : public ImageAccessible, public ia2AccessibleImage {
 public:
  ImageAccessibleWrap(nsIContent* aContent, DocAccessible* aDoc)
      : ImageAccessible(aContent, aDoc) {}

  // IUnknown
  DECL_IUNKNOWN_INHERITED

  // nsISupports
  NS_DECL_ISUPPORTS_INHERITED

 protected:
  ~ImageAccessibleWrap() {}
};

}  // namespace a11y
}  // namespace mozilla

#endif
