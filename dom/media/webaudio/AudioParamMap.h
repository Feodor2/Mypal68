/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef AudioParamMap_h_
#define AudioParamMap_h_

#include "nsWrapperCache.h"
#include "nsCOMPtr.h"

class nsPIDOMWindowInner;

namespace mozilla {
namespace dom {

class AudioParamMap final : public nsWrapperCache {
 public:
  NS_INLINE_DECL_CYCLE_COLLECTING_NATIVE_REFCOUNTING(AudioParamMap)
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_NATIVE_CLASS(AudioParamMap)

  explicit AudioParamMap(nsPIDOMWindowInner* aParent);

  nsPIDOMWindowInner* GetParentObject() const { return mParent; }

  JSObject* WrapObject(JSContext* aCx,
                       JS::Handle<JSObject*> aGivenProto) override;

 private:
  ~AudioParamMap() = default;
  nsCOMPtr<nsPIDOMWindowInner> mParent;
};

}  // namespace dom
}  // namespace mozilla

#endif  // AudioParamMap_h_