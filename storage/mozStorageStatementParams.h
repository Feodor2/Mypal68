/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZSTORAGESTATEMENTPARAMS_H
#define MOZSTORAGESTATEMENTPARAMS_H

#include "mozilla/Attributes.h"
#include "mozilla/ErrorResult.h"
#include "nsPIDOMWindow.h"
#include "nsWrapperCache.h"

namespace mozilla {
namespace storage {

class Statement;

class StatementParams final : public nsISupports, public nsWrapperCache {
 public:
  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS(StatementParams)

  explicit StatementParams(nsPIDOMWindowInner* aWindow, Statement* aStatement);

  void NamedGetter(JSContext* aCx, const nsAString& aName, bool& aFound,
                   JS::MutableHandle<JS::Value> aResult,
                   mozilla::ErrorResult& aRv);

  void NamedSetter(JSContext* aCx, const nsAString& aName,
                   JS::Handle<JS::Value> aValue, mozilla::ErrorResult& aRv);

  uint32_t Length() const { return mParamCount; }

  void IndexedGetter(JSContext* aCx, uint32_t aIndex, bool& aFound,
                     JS::MutableHandle<JS::Value> aResult,
                     mozilla::ErrorResult& aRv);

  void IndexedSetter(JSContext* aCx, uint32_t aIndex,
                     JS::Handle<JS::Value> aValue, mozilla::ErrorResult& aRv);

  void GetSupportedNames(nsTArray<nsString>& aNames);

  JSObject* WrapObject(JSContext* aCx,
                       JS::Handle<JSObject*> aGivenProto) override;

  nsPIDOMWindowInner* GetParentObject() const { return mWindow; }

 private:
  ~StatementParams() {}

  nsCOMPtr<nsPIDOMWindowInner> mWindow;
  Statement* mStatement;
  uint32_t mParamCount;

  friend class StatementParamsHolder;
};

}  // namespace storage
}  // namespace mozilla

#endif /* MOZSTORAGESTATEMENTPARAMS_H */
