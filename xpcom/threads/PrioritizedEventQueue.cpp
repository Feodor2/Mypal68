/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "PrioritizedEventQueue.h"
#include "mozilla/EventQueue.h"
#include "mozilla/ScopeExit.h"
#include "mozilla/StaticPrefs_threads.h"
#include "nsThreadManager.h"
#include "nsXPCOMPrivate.h"  // for gXPCOMThreadsShutDown
#include "InputEventStatistics.h"

using namespace mozilla;

PrioritizedEventQueue::PrioritizedEventQueue(
    already_AddRefed<nsIIdlePeriod> aIdlePeriod)
    : mHighQueue(MakeUnique<EventQueue>(EventQueuePriority::High)),
      mInputQueue(MakeUnique<EventQueueSized<32>>(EventQueuePriority::Input)),
      mMediumHighQueue(MakeUnique<EventQueue>(EventQueuePriority::MediumHigh)),
      mNormalQueue(MakeUnique<EventQueueSized<64>>(EventQueuePriority::Normal)),
      mDeferredTimersQueue(
          MakeUnique<EventQueue>(EventQueuePriority::DeferredTimers)),
      mIdleQueue(MakeUnique<EventQueue>(EventQueuePriority::Idle)),
      mIdlePeriod(aIdlePeriod) {}

PrioritizedEventQueue::~PrioritizedEventQueue() = default;

void PrioritizedEventQueue::PutEvent(already_AddRefed<nsIRunnable>&& aEvent,
                                     EventQueuePriority aPriority,
                                     const AutoLock& aProofOfLock,
                                     mozilla::TimeDuration* aDelay) {
  // Double check the priority with a QI.
  RefPtr<nsIRunnable> event(aEvent);
  EventQueuePriority priority = aPriority;

  if (priority == EventQueuePriority::Input &&
      mInputQueueState == STATE_DISABLED) {
    priority = EventQueuePriority::Normal;
  } else if (priority == EventQueuePriority::MediumHigh &&
             !StaticPrefs::threads_medium_high_event_queue_enabled()) {
    priority = EventQueuePriority::Normal;
  }

  switch (priority) {
    case EventQueuePriority::High:
      mHighQueue->PutEvent(event.forget(), priority, aProofOfLock, aDelay);
      break;
    case EventQueuePriority::Input:
      mInputQueue->PutEvent(event.forget(), priority, aProofOfLock, aDelay);
      break;
    case EventQueuePriority::MediumHigh:
      mMediumHighQueue->PutEvent(event.forget(), priority, aProofOfLock,
                                 aDelay);
      break;
    case EventQueuePriority::Normal:
      mNormalQueue->PutEvent(event.forget(), priority, aProofOfLock, aDelay);
      break;
    case EventQueuePriority::DeferredTimers:
      mDeferredTimersQueue->PutEvent(event.forget(), priority, aProofOfLock,
                                     aDelay);
      break;
    case EventQueuePriority::Idle:
      mIdleQueue->PutEvent(event.forget(), priority, aProofOfLock, aDelay);
      break;
    case EventQueuePriority::Count:
      MOZ_CRASH("EventQueuePriority::Count isn't a valid priority");
      break;
  }
}

TimeStamp PrioritizedEventQueue::GetIdleDeadline() {
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
  {
    // Releasing the lock temporarily since getting the idle period
    // might need to lock the timer thread. Unlocking here might make
    // us receive an event on the main queue, but we've committed to
    // run an idle event anyhow.
    AutoUnlock unlock(*mMutex);
    mIdlePeriod->GetIdlePeriodHint(&idleDeadline);
  }

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

EventQueuePriority PrioritizedEventQueue::SelectQueue(
    bool aUpdateState, const AutoLock& aProofOfLock) {
  size_t inputCount = mInputQueue->Count(aProofOfLock);

  if (aUpdateState && mInputQueueState == STATE_ENABLED &&
      mInputHandlingStartTime.IsNull() && inputCount > 0) {
    mInputHandlingStartTime =
        InputEventStatistics::Get().GetInputHandlingStartTime(inputCount);
  }

  // We check the different queues in the following order. The conditions we use
  // are meant to avoid starvation and to ensure that we don't process an event
  // at the wrong time.
  //
  // HIGH: if mProcessHighPriorityQueue
  // INPUT: if inputCount > 0 && TimeStamp::Now() > mInputHandlingStartTime
  // MEDIUMHIGH: if medium high pending
  // NORMAL: if normal pending
  //
  // If we still don't have an event, then we take events from the queues
  // in the following order:
  //
  // HIGH
  // INPUT
  // DEFERREDTIMERS: if GetIdleDeadline()
  // IDLE: if GetIdleDeadline()
  //
  // If we don't get an event in this pass, then we return null since no events
  // are ready.

  // This variable determines which queue we will take an event from.
  EventQueuePriority queue;
  bool highPending = !mHighQueue->IsEmpty(aProofOfLock);

  if (mProcessHighPriorityQueue) {
    queue = EventQueuePriority::High;
  } else if (inputCount > 0 && (mInputQueueState == STATE_FLUSHING ||
                                (mInputQueueState == STATE_ENABLED &&
                                 !mInputHandlingStartTime.IsNull() &&
                                 TimeStamp::Now() > mInputHandlingStartTime))) {
    queue = EventQueuePriority::Input;
  } else if (!mMediumHighQueue->IsEmpty(aProofOfLock)) {
    MOZ_ASSERT(
        mInputQueueState != STATE_FLUSHING,
        "Shouldn't consume medium high event when flushing input events");
    queue = EventQueuePriority::MediumHigh;
  } else if (!mNormalQueue->IsEmpty(aProofOfLock)) {
    MOZ_ASSERT(mInputQueueState != STATE_FLUSHING,
               "Shouldn't consume normal event when flushing input events");
    queue = EventQueuePriority::Normal;
  } else if (highPending) {
    queue = EventQueuePriority::High;
  } else if (inputCount > 0 && mInputQueueState != STATE_SUSPEND) {
    MOZ_ASSERT(
        mInputQueueState != STATE_DISABLED,
        "Shouldn't consume input events when the input queue is disabled");
    queue = EventQueuePriority::Input;
  } else if (!mDeferredTimersQueue->IsEmpty(aProofOfLock)) {
    // We may not actually return an idle event in this case.
    queue = EventQueuePriority::DeferredTimers;
  } else {
    // We may not actually return an idle event in this case.
    queue = EventQueuePriority::Idle;
  }

  MOZ_ASSERT_IF(
      queue == EventQueuePriority::Input,
      mInputQueueState != STATE_DISABLED && mInputQueueState != STATE_SUSPEND);

  if (aUpdateState) {
    mProcessHighPriorityQueue = highPending;
  }

  return queue;
}

// The delay returned is the queuing delay a hypothetical Input event would
// see due to the current running event if it had arrived while the current
// event was queued.  This means that any event running at  priority below
// Input doesn't cause queuing delay for Input events, and we return
// TimeDuration() for those cases.
already_AddRefed<nsIRunnable> PrioritizedEventQueue::GetEvent(
    EventQueuePriority* aPriority, const AutoLock& aProofOfLock,
    mozilla::TimeDuration* aHypotheticalInputEventDelay) {
  auto guard =
      MakeScopeExit([&] { mHasPendingEventsPromisedIdleEvent = false; });

  EventQueuePriority queue = SelectQueue(true, aProofOfLock);

  if (aPriority) {
    *aPriority = queue;
  }

  // Since Input events will only be delayed behind Input or High events,
  // the amount of time a lower-priority event spent in the queue is
  // irrelevant in knowing how long an input event would be delayed.
  // Alternatively, we could export the delay and let the higher-level code
  // key off the returned priority level (though then it'd need to know
  // if the thread's queue was a PrioritizedEventQueue or normal/other
  // EventQueue).
  nsCOMPtr<nsIRunnable> event;
  switch (queue) {
    default:
      MOZ_CRASH();
      break;

    case EventQueuePriority::High:
      event = mHighQueue->GetEvent(aPriority, aProofOfLock,
                                   aHypotheticalInputEventDelay);
      MOZ_ASSERT(event);
      mInputHandlingStartTime = TimeStamp();
      mProcessHighPriorityQueue = false;
      break;

    case EventQueuePriority::Input:
      event = mInputQueue->GetEvent(aPriority, aProofOfLock,
                                    aHypotheticalInputEventDelay);
      MOZ_ASSERT(event);
      break;

      // All queue priorities below Input don't add their queuing time to the
      // time an input event will be delayed, so report 0 for time-in-queue
      // if we're below Input; input events will only be delayed by the time
      // an event actually runs (if the event is below Input event's priority)
    case EventQueuePriority::MediumHigh:
      event = mMediumHighQueue->GetEvent(aPriority, aProofOfLock);
      *aHypotheticalInputEventDelay = TimeDuration();
      break;

    case EventQueuePriority::Normal:
      event = mNormalQueue->GetEvent(aPriority, aProofOfLock);
      *aHypotheticalInputEventDelay = TimeDuration();
      break;

    case EventQueuePriority::Idle:
    case EventQueuePriority::DeferredTimers:
      *aHypotheticalInputEventDelay = TimeDuration();
      // If we get here, then all queues except deferredtimers and idle are
      // empty.

      if (mIdleQueue->IsEmpty(aProofOfLock) &&
          mDeferredTimersQueue->IsEmpty(aProofOfLock)) {
        MOZ_ASSERT(!mHasPendingEventsPromisedIdleEvent);
        return nullptr;
      }

      TimeStamp idleDeadline = GetIdleDeadline();
      if (!idleDeadline) {
        return nullptr;
      }

      event = mDeferredTimersQueue->GetEvent(aPriority, aProofOfLock);
      if (!event) {
        event = mIdleQueue->GetEvent(aPriority, aProofOfLock);
      }
      if (event) {
        nsCOMPtr<nsIIdleRunnable> idleEvent = do_QueryInterface(event);
        if (idleEvent) {
          idleEvent->SetDeadline(idleDeadline);
        }
      }
      break;
  }  // switch (queue)

  if (!event) {
    *aHypotheticalInputEventDelay = TimeDuration();
  }

  return event.forget();
}

bool PrioritizedEventQueue::IsEmpty(const AutoLock& aProofOfLock) {
  // Just check IsEmpty() on the sub-queues. Don't bother checking the idle
  // deadline since that only determines whether an idle event is ready or not.
  return mHighQueue->IsEmpty(aProofOfLock) &&
         mInputQueue->IsEmpty(aProofOfLock) &&
         mMediumHighQueue->IsEmpty(aProofOfLock) &&
         mNormalQueue->IsEmpty(aProofOfLock) &&
         mDeferredTimersQueue->IsEmpty(aProofOfLock) &&
         mIdleQueue->IsEmpty(aProofOfLock);
}

bool PrioritizedEventQueue::HasReadyEvent(const AutoLock& aProofOfLock) {
  mHasPendingEventsPromisedIdleEvent = false;

  EventQueuePriority queue = SelectQueue(false, aProofOfLock);

  if (queue == EventQueuePriority::High) {
    return mHighQueue->HasReadyEvent(aProofOfLock);
  } else if (queue == EventQueuePriority::Input) {
    return mInputQueue->HasReadyEvent(aProofOfLock);
  } else if (queue == EventQueuePriority::MediumHigh) {
    return mMediumHighQueue->HasReadyEvent(aProofOfLock);
  } else if (queue == EventQueuePriority::Normal) {
    return mNormalQueue->HasReadyEvent(aProofOfLock);
  }

  MOZ_ASSERT(queue == EventQueuePriority::Idle ||
             queue == EventQueuePriority::DeferredTimers);

  // If we get here, then both the high and normal queues are empty.

  if (mDeferredTimersQueue->IsEmpty(aProofOfLock) &&
      mIdleQueue->IsEmpty(aProofOfLock)) {
    return false;
  }

  TimeStamp idleDeadline = GetIdleDeadline();
  if (idleDeadline && (mDeferredTimersQueue->HasReadyEvent(aProofOfLock) ||
                       mIdleQueue->HasReadyEvent(aProofOfLock))) {
    mHasPendingEventsPromisedIdleEvent = true;
    return true;
  }

  return false;
}

bool PrioritizedEventQueue::HasPendingHighPriorityEvents(
    const AutoLock& aProofOfLock) {
  return !mHighQueue->IsEmpty(aProofOfLock);
}

size_t PrioritizedEventQueue::Count(const AutoLock& aProofOfLock) const {
  MOZ_CRASH("unimplemented");
}

void PrioritizedEventQueue::EnableInputEventPrioritization(
    const AutoLock& aProofOfLock) {
  MOZ_ASSERT(mInputQueueState == STATE_DISABLED);
  mInputQueueState = STATE_ENABLED;
  mInputHandlingStartTime = TimeStamp();
}

void PrioritizedEventQueue::FlushInputEventPrioritization(
    const AutoLock& aProofOfLock) {
  MOZ_ASSERT(mInputQueueState == STATE_ENABLED ||
             mInputQueueState == STATE_SUSPEND);
  mInputQueueState =
      mInputQueueState == STATE_ENABLED ? STATE_FLUSHING : STATE_SUSPEND;
}

void PrioritizedEventQueue::SuspendInputEventPrioritization(
    const AutoLock& aProofOfLock) {
  MOZ_ASSERT(mInputQueueState == STATE_ENABLED ||
             mInputQueueState == STATE_FLUSHING);
  mInputQueueState = STATE_SUSPEND;
}

void PrioritizedEventQueue::ResumeInputEventPrioritization(
    const AutoLock& aProofOfLock) {
  MOZ_ASSERT(mInputQueueState == STATE_SUSPEND);
  mInputQueueState = STATE_ENABLED;
}
