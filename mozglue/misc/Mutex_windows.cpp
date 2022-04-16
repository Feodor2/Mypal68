/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/Assertions.h"
#include "mozilla/DebugOnly.h"
#include "mozilla/PlatformMutex.h"

#include <windows.h>

#include "MutexPlatformData_windows.h"

mozilla::detail::MutexImpl::MutexImpl(recordreplay::Behavior aRecorded) {
  platformData = reinterpret_cast<PlatformData*>(platformData_);
  BOOL r;
  r = InitializeCriticalSectionAndSpinCount(&platformData->criticalSection,1500);
  MOZ_RELEASE_ASSERT(r);
}

mozilla::detail::MutexImpl::~MutexImpl() {
  if (!platformData_)
    return;
  DeleteCriticalSection(&platformData->criticalSection);
}

void mozilla::detail::MutexImpl::lock() {
  EnterCriticalSection(&platformData->criticalSection);
}

bool mozilla::detail::MutexImpl::tryLock() { return mutexTryLock(); }

bool mozilla::detail::MutexImpl::mutexTryLock() {
  return !!TryEnterCriticalSection(&platformData->criticalSection);
}

void mozilla::detail::MutexImpl::unlock() {
  LeaveCriticalSection(&platformData->criticalSection);
}
