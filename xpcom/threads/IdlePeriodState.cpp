/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/IdlePeriodState.h"
#include "nsIIdlePeriod.h"
#include "nsThreadManager.h"
#include "nsThreadUtils.h"
#include "nsXPCOM.h"
#include "nsXULAppAPI.h"

namespace mozilla {

IdlePeriodState::IdlePeriodState(already_AddRefed<nsIIdlePeriod> &&aIdlePeriod)
    : mIdlePeriod(aIdlePeriod) {
  MOZ_ASSERT(NS_IsMainThread(),
             "Why are we touching idle state off the main thread?");
}

IdlePeriodState::~IdlePeriodState() = default;

size_t IdlePeriodState::SizeOfExcludingThis(MallocSizeOf aMallocSizeOf) const {
  size_t n = 0;
  if (mIdlePeriod) {
    n += aMallocSizeOf(mIdlePeriod);
  }

  return n;
}

TimeStamp IdlePeriodState::GetIdleDeadline() {
  // If we are shutting down, we won't honor the idle period, and we will
  // always process idle runnables.  This will ensure that the idle queue
  // gets exhausted at shutdown time to prevent intermittently leaking
  // some runnables inside that queue and even worse potentially leaving
  // some important cleanup work unfinished.
  if (gXPCOMThreadsShutDown ||
      nsThreadManager::get().GetCurrentThread()->ShuttingDown()) {
    return TimeStamp::Now();
  }

  TimeStamp idleDeadline;
  // This GetIdlePeriodHint() call is the reason we need a AutoUnlock here.
  mIdlePeriod->GetIdlePeriodHint(&idleDeadline);

  // If HasPendingEvents() has been called and it has returned true because of
  // pending idle events, there is a risk that we may decide here that we aren't
  // idle and return null, in which case HasPendingEvents() has effectively
  // lied.  Since we can't go back and fix the past, we have to adjust what we
  // do here and forcefully pick the idle queue task here.  Note that this means
  // that we are choosing to run a task from the idle queue when we would
  // normally decide that we aren't in an idle period, but this can only happen
  // if we fall out of the idle period in between the call to HasPendingEvents()
  // and here, which should hopefully be quite rare.  We are effectively
  // choosing to prioritize the sanity of our API semantics over the optimal
  // scheduling.
  if (!mHasPendingEventsPromisedIdleEvent &&
      (!idleDeadline || idleDeadline < TimeStamp::Now())) {
    return TimeStamp();
  }
  if (mHasPendingEventsPromisedIdleEvent && !idleDeadline) {
    // If HasPendingEvents() has been called and it has returned true, but we're
    // no longer in the idle period, we must return a valid timestamp to pretend
    // that we are still in the idle period.
    return TimeStamp::Now();
  }
  return idleDeadline;
}

}  // namespace mozilla
