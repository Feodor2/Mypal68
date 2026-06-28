/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "js/experimental/TypedData.h"  // JS_GetFloat32ArrayData
#include "mozilla/HoldDropJSObjects.h"
#include "mozilla/dom/TypedArray.h"
#include "mozilla/dom/Pose.h"

namespace mozilla::dom {

NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE_WITH_JS_MEMBERS(
    Pose, (mParent),
    (mPosition, mLinearVelocity, mLinearAcceleration, mOrientation,
     mAngularVelocity, mAngularAcceleration))

NS_IMPL_CYCLE_COLLECTION_ROOT_NATIVE(Pose, AddRef)
NS_IMPL_CYCLE_COLLECTION_UNROOT_NATIVE(Pose, Release)

Pose::Pose(nsISupports* aParent)
    : mParent(aParent),
      mPosition(nullptr),
      mLinearVelocity(nullptr),
      mLinearAcceleration(nullptr),
      mOrientation(nullptr),
      mAngularVelocity(nullptr),
      mAngularAcceleration(nullptr) {
  mozilla::HoldJSObjects(this);
}

Pose::~Pose() { mozilla::DropJSObjects(this); }

nsISupports* Pose::GetParentObject() const { return mParent; }

void Pose::SetFloat32Array(JSContext* aJSContext,
                           JS::MutableHandle<JSObject*> aRetVal,
                           JS::Heap<JSObject*>& aObj, float* aVal,
                           uint32_t sizeOfVal, bool bCreate, ErrorResult& aRv) {
  if (bCreate) {
    aObj = Float32Array::Create(aJSContext, this, sizeOfVal, aVal);
    if (!aObj) {
      aRv.NoteJSContextException(aJSContext);
      return;
    }
  }

  aRetVal.set(aObj);
}

}  // namespace mozilla::dom
