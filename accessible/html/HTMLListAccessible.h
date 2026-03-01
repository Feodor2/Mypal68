/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_a11y_HTMLListAccessible_h__
#define mozilla_a11y_HTMLListAccessible_h__

#include "BaseAccessibles.h"
#include "HyperTextAccessibleWrap.h"

namespace mozilla {
namespace a11y {

class HTMLListBulletAccessible;

/**
 * Used for HTML list (like HTML ul).
 */
class HTMLListAccessible : public HyperTextAccessibleWrap {
 public:
  HTMLListAccessible(nsIContent* aContent, DocAccessible* aDoc)
      : HyperTextAccessibleWrap(aContent, aDoc) {
    mGenericTypes |= eList;
  }

  // nsISupports
  NS_INLINE_DECL_REFCOUNTING_INHERITED(HTMLListAccessible,
                                       HyperTextAccessibleWrap)

  // Accessible
  virtual a11y::role NativeRole() const override;
  virtual uint64_t NativeState() const override;

 protected:
  virtual ~HTMLListAccessible() {}
};

/**
 * Used for HTML list item (e.g. HTML li).
 */
class HTMLLIAccessible : public HyperTextAccessibleWrap {
 public:
  HTMLLIAccessible(nsIContent* aContent, DocAccessible* aDoc);

  // nsISupports
  NS_INLINE_DECL_REFCOUNTING_INHERITED(HTMLLIAccessible,
                                       HyperTextAccessibleWrap)

  // Accessible
  virtual nsRect BoundsInAppUnits() const override;
  virtual a11y::role NativeRole() const override;
  virtual uint64_t NativeState() const override;

  // HTMLLIAccessible
  Accessible* Bullet() const;

 protected:
  virtual ~HTMLLIAccessible() {}
};

/**
 * Used for bullet of HTML list item element (for example, HTML li).
 */
class HTMLListBulletAccessible : public LeafAccessible {
 public:
  HTMLListBulletAccessible(nsIContent* aContent, DocAccessible* aDoc);
  virtual ~HTMLListBulletAccessible() {}

  // Accessible
  virtual ENameValueFlag Name(nsString& aName) const override;
  virtual a11y::role NativeRole() const override;
  virtual uint64_t NativeState() const override;
  virtual already_AddRefed<nsIPersistentProperties> NativeAttributes() override;
  virtual void AppendTextTo(nsAString& aText, uint32_t aStartOffset = 0,
                            uint32_t aLength = UINT32_MAX) override;
};

inline HTMLLIAccessible* Accessible::AsHTMLListItem() {
  return IsHTMLListItem() ? static_cast<HTMLLIAccessible*>(this) : nullptr;
}

}  // namespace a11y
}  // namespace mozilla

#endif
