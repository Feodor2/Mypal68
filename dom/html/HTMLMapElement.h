/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_HTMLMapElement_h
#define mozilla_dom_HTMLMapElement_h

#include "mozilla/Attributes.h"
#include "nsGenericHTMLElement.h"
#include "nsGkAtoms.h"

class nsContentList;

namespace mozilla {
namespace dom {

class HTMLMapElement final : public nsGenericHTMLElement {
 public:
  explicit HTMLMapElement(already_AddRefed<mozilla::dom::NodeInfo>&& aNodeInfo);

  // nsISupports
  NS_DECL_ISUPPORTS_INHERITED

  virtual nsresult Clone(dom::NodeInfo*, nsINode** aResult) const override;

  NS_DECL_CYCLE_COLLECTION_CLASS_INHERITED_NO_UNLINK(HTMLMapElement,
                                                     nsGenericHTMLElement)

  void GetName(nsAString& aValue) { GetHTMLAttr(nsGkAtoms::name, aValue); }
  void SetName(const nsAString& aName, ErrorResult& aError) {
    SetHTMLAttr(nsGkAtoms::name, aName, aError);
  }
  nsIHTMLCollection* Areas();

  virtual JSObject* WrapNode(JSContext* aCx,
                             JS::Handle<JSObject*> aGivenProto) override;

 protected:
  ~HTMLMapElement() = default;

  RefPtr<nsContentList> mAreas;
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_HTMLMapElement_h
