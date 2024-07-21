/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_IdlePeriodState_h
#define mozilla_IdlePeriodState_h

/**
 * A class for tracking the state of our idle period.  This includes keeping
 * track of both the state of our process-local idle period estimate and, for
 * content processes, managing communication with the parent process for
 * cross-pprocess idle detection.
 */

#include "base/lock.h"
#include "mozilla/MemoryReporting.h"
#include "mozilla/RefPtr.h"
#include "mozilla/TimeStamp.h"
#include "nsCOMPtr.h"

#include <stdint.h>

class nsIIdlePeriod;

namespace mozilla {

class IdlePeriodState {
 public:
  explicit IdlePeriodState(already_AddRefed<nsIIdlePeriod>&& aIdlePeriod);

  ~IdlePeriodState();

  // Integration with memory reporting.
  size_t SizeOfExcludingThis(MallocSizeOf aMallocSizeOf) const;

  // Notification that whoever we are tracking idle state has idle tasks that
  // they are considering ready to run and that we should keep claiming they are
  // ready to run until they call ForgetPendingTaskGuarantee().
  void EnforcePendingTaskGuarantee() {
    mHasPendingEventsPromisedIdleEvent = true;
  }

  // Notification that whoever we are tracking idle state for is done with our
  // "we have an idle event ready to run" guarantee.  When this happens, we can
  // reset mHasPendingEventsPromisedIdleEvent to false, because we have
  // fulfilled our contract.
  void ForgetPendingTaskGuarantee() {
    mHasPendingEventsPromisedIdleEvent = false;
  }

  // Update our cached idle deadline so consumers can use it while holding
  // locks. Consumers must ClearCachedIdleDeadline() once they are done.
  void UpdateCachedIdleDeadline() {
    mCachedIdleDeadline = GetIdleDeadline();
  }

  // Reset our cached idle deadline, so we stop allowing idle runnables to run.
  void ClearCachedIdleDeadline() { mCachedIdleDeadline = TimeStamp(); }

  // Get the current cached idle deadline.  This may return a null timestamp.
  TimeStamp GetCachedIdleDeadline() { return mCachedIdleDeadline; }

  // Returns a null TimeStamp if we're not in the idle period.
  //
  // aMutexToUnlock is the mutex to unlock if we do anything that might lock
  // other mutexes.
  TimeStamp GetIdleDeadline();

  // Set to true if we have claimed we have a ready-to-run idle task when asked.
  // In that case, we will ensure that we allow at least one task to run when
  // someone tries to run a task, even if we have run out of idle period at that
  // point.  This ensures that we never fail to produce a task to run if we
  // claim we have a task ready to run.
  bool mHasPendingEventsPromisedIdleEvent = false;

  // Our cached idle deadline.  This is set by UpdateCachedIdleDeadline() and
  // cleared by ClearCachedIdleDeadline().  Consumers should do the former while
  // not holding any locks, but may do the latter while holding locks.
  TimeStamp mCachedIdleDeadline;

  // mIdlePeriod keeps track of the current idle period. Calling
  // mIdlePeriod->GetIdlePeriodHint() will give an estimate of when
  // the current idle period will end.
  nsCOMPtr<nsIIdlePeriod> mIdlePeriod;
};

}  // namespace mozilla

#endif  // mozilla_IdlePeriodState_h
