/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_HTMLMetaElement_h
#define mozilla_dom_HTMLMetaElement_h

#include "mozilla/Attributes.h"
#include "nsGenericHTMLElement.h"

namespace mozilla {
namespace dom {

class HTMLMetaElement final : public nsGenericHTMLElement {
 public:
  explicit HTMLMetaElement(
      already_AddRefed<mozilla::dom::NodeInfo>&& aNodeInfo);

  // nsISupports
  NS_INLINE_DECL_REFCOUNTING_INHERITED(HTMLMetaElement, nsGenericHTMLElement)

  virtual nsresult BindToTree(BindContext&, nsINode& aParent) override;
  virtual void UnbindFromTree(bool aNullParent = true) override;

  virtual nsresult AfterSetAttr(int32_t aNameSpaceID, nsAtom* aName,
                                const nsAttrValue* aValue,
                                const nsAttrValue* aOldValue,
                                nsIPrincipal* aSubjectPrincipal,
                                bool aNotify) override;

  void CreateAndDispatchEvent(Document* aDoc, const nsAString& aEventName);

  virtual nsresult Clone(dom::NodeInfo*, nsINode** aResult) const override;

  void GetName(nsAString& aValue) { GetHTMLAttr(nsGkAtoms::name, aValue); }
  void SetName(const nsAString& aName, ErrorResult& aRv) {
    SetHTMLAttr(nsGkAtoms::name, aName, aRv);
  }
  void GetHttpEquiv(nsAString& aValue) {
    GetHTMLAttr(nsGkAtoms::httpEquiv, aValue);
  }
  void SetHttpEquiv(const nsAString& aHttpEquiv, ErrorResult& aRv) {
    SetHTMLAttr(nsGkAtoms::httpEquiv, aHttpEquiv, aRv);
  }
  void GetContent(nsAString& aValue) {
    GetHTMLAttr(nsGkAtoms::content, aValue);
  }
  void SetContent(const nsAString& aContent, ErrorResult& aRv) {
    SetHTMLAttr(nsGkAtoms::content, aContent, aRv);
  }
  void GetScheme(nsAString& aValue) { GetHTMLAttr(nsGkAtoms::scheme, aValue); }
  void SetScheme(const nsAString& aScheme, ErrorResult& aRv) {
    SetHTMLAttr(nsGkAtoms::scheme, aScheme, aRv);
  }

  virtual JSObject* WrapNode(JSContext* aCx,
                             JS::Handle<JSObject*> aGivenProto) override;

 protected:
  virtual ~HTMLMetaElement();

 private:
  void SetMetaReferrer(Document* aDocument);
  void ProcessViewportContent(Document* aDocument);
  void DiscardViewportContent(Document* aDocument);
};

}  // namespace dom
}  // namespace mozilla

#endif  // mozilla_dom_HTMLMetaElement_h
