/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ScopedLogExtraInfo.h"

namespace mozilla::dom::quota {

#ifdef QM_SCOPED_LOG_EXTRA_INFO_ENABLED
MOZ_THREAD_LOCAL(const nsACString*) ScopedLogExtraInfo::sQueryValue;

/* static */
auto ScopedLogExtraInfo::FindSlot(const char* aTag) {
  // XXX For now, don't use a real map but just allow the known tag values.

  if (aTag == kTagQuery) {
    return &sQueryValue;
  }

  MOZ_CRASH("Unknown tag!");
}

ScopedLogExtraInfo::~ScopedLogExtraInfo() {
  if (mTag) {
    MOZ_ASSERT(&mCurrentValue == FindSlot(mTag)->get(),
               "Bad scoping of ScopedLogExtraInfo, must not be interleaved!");

    FindSlot(mTag)->set(mPreviousValue);
  }
}

ScopedLogExtraInfo::ScopedLogExtraInfo(ScopedLogExtraInfo&& aOther)
    : mTag(aOther.mTag),
      mPreviousValue(aOther.mPreviousValue),
      mCurrentValue(std::move(aOther.mCurrentValue)) {
  aOther.mTag = nullptr;
  FindSlot(mTag)->set(&mCurrentValue);
}

/* static */ ScopedLogExtraInfo::ScopedLogExtraInfoMap
ScopedLogExtraInfo::GetExtraInfoMap() {
  // This could be done in a cheaper way, but this is never called on a hot
  // path, so we anticipate using a real map inside here to make use simpler for
  // the caller(s).

  ScopedLogExtraInfoMap map;
  if (XRE_IsParentProcess()) {
    if (sQueryValue.get()) {
      map.emplace(kTagQuery, sQueryValue.get());
    }
  }
  return map;
}

/* static */ void ScopedLogExtraInfo::Initialize() {
  MOZ_ALWAYS_TRUE(sQueryValue.init());
}

void ScopedLogExtraInfo::AddInfo() {
  auto* slot = FindSlot(mTag);
  MOZ_ASSERT(slot);
  mPreviousValue = slot->get();

  slot->set(&mCurrentValue);
}
#endif

}  // namespace mozilla::dom::quota
