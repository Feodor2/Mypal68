/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsChangeObserver_h_
#define nsChangeObserver_h_

class nsIContent;
class nsAtom;
namespace mozilla {
namespace dom {
class Document;
}
}  // namespace mozilla

#define NS_DECL_CHANGEOBSERVER                                            \
  void ObserveAttributeChanged(mozilla::dom::Document* aDocument,         \
                               nsIContent* aContent, nsAtom* aAttribute)  \
      override;                                                           \
  void ObserveContentRemoved(mozilla::dom::Document* aDocument,           \
                             nsIContent* aContainer, nsIContent* aChild,  \
                             nsIContent* aPreviousChild) override;        \
  void ObserveContentInserted(mozilla::dom::Document* aDocument,          \
                              nsIContent* aContainer, nsIContent* aChild) \
      override;

// Something that wants to be alerted to changes in attributes or changes in
// its corresponding content object.
//
// This interface is used by our menu code so we only have to have one
// nsIDocumentObserver.
//
// Any class that implements this interface must take care to unregister itself
// on deletion.
class nsChangeObserver {
 public:
  // XXX use dom::Element
  virtual void ObserveAttributeChanged(mozilla::dom::Document* aDocument,
                                       nsIContent* aContent,
                                       nsAtom* aAttribute) = 0;

  virtual void ObserveContentRemoved(mozilla::dom::Document* aDocument,
                                     nsIContent* aContainer, nsIContent* aChild,
                                     nsIContent* aPreviousSibling) = 0;

  virtual void ObserveContentInserted(mozilla::dom::Document* aDocument,
                                      nsIContent* aContainer,
                                      nsIContent* aChild) = 0;
};

#endif  // nsChangeObserver_h_
