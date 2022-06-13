/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_PrioritizedEventQueue_h
#define mozilla_PrioritizedEventQueue_h

#include "mozilla/AbstractEventQueue.h"
#include "mozilla/TimeStamp.h"
#include "mozilla/TypeTraits.h"
#include "mozilla/UniquePtr.h"
#include "nsCOMPtr.h"
#include "nsIIdlePeriod.h"

class nsIRunnable;

namespace mozilla {

// This AbstractEventQueue implementation has one queue for each
// EventQueuePriority. The type of queue used for each priority is determined by
// the template parameter.
//
// When an event is pushed, its priority is determined by QIing the runnable to
// nsIRunnablePriority, or by falling back to the aPriority parameter if the QI
// fails.
//
// When an event is popped, a queue is selected based on heuristics that
// optimize for performance. Roughly, events are selected from the highest
// priority queue that is non-empty. However, there are a few exceptions:
// - We try to avoid processing too many high-priority events in a row so
//   that the normal priority queue is not starved. When there are high-
//   and normal-priority events available, we interleave popping from the
//   normal and high queues.
// - We do not select events from the idle queue if the current idle period
//   is almost over.
template <class InnerQueueT>
class PrioritizedEventQueue final : public AbstractEventQueue {
 public:
  static const bool SupportsPrioritization = true;

  explicit PrioritizedEventQueue(already_AddRefed<nsIIdlePeriod> aIdlePeriod)
      : mHighQueue(MakeUnique<InnerQueueT>(EventQueuePriority::High)),
        mInputQueue(MakeUnique<InnerQueueT>(EventQueuePriority::Input)),
        mMediumHighQueue(
            MakeUnique<InnerQueueT>(EventQueuePriority::MediumHigh)),
        mNormalQueue(MakeUnique<InnerQueueT>(EventQueuePriority::Normal)),
        mDeferredTimersQueue(
            MakeUnique<InnerQueueT>(EventQueuePriority::DeferredTimers)),
        mIdleQueue(MakeUnique<InnerQueueT>(EventQueuePriority::Idle)),
        mIdlePeriod(aIdlePeriod) {}

  void PutEvent(already_AddRefed<nsIRunnable>&& aEvent,
                EventQueuePriority aPriority,
                const AutoLock& aProofOfLock) final;
  already_AddRefed<nsIRunnable> GetEvent(
      EventQueuePriority* aPriority, const AutoLock& aProofOfLock) final;

  bool IsEmpty(const AutoLock& aProofOfLock) final;
  size_t Count(const AutoLock& aProofOfLock) const final;
  bool HasReadyEvent(const AutoLock& aProofOfLock) final;
  bool HasPendingHighPriorityEvents(const AutoLock& aProofOfLock) final;

  // When checking the idle deadline, we need to drop whatever mutex protects
  // this queue. This method allows that mutex to be stored so that we can drop
  // it and reacquire it when checking the idle deadline. The mutex must live at
  // least as long as the queue.
  void SetMutexRef(Lock& aMutex) { mMutex = &aMutex; }

#ifndef RELEASE_OR_BETA
  // nsThread.cpp sends telemetry containing the most recently computed idle
  // deadline. We store a reference to a field in nsThread where this deadline
  // will be stored so that it can be fetched quickly for telemetry.
  void SetNextIdleDeadlineRef(TimeStamp& aDeadline) {
    mNextIdleDeadline = &aDeadline;
  }
#endif

  void EnableInputEventPrioritization(const AutoLock& aProofOfLock) final;
  void FlushInputEventPrioritization(const AutoLock& aProofOfLock) final;
  void SuspendInputEventPrioritization(const AutoLock& aProofOfLock) final;
  void ResumeInputEventPrioritization(const AutoLock& aProofOfLock) final;

  size_t SizeOfExcludingThis(
      mozilla::MallocSizeOf aMallocSizeOf) const override {
    size_t n = 0;

    n += mHighQueue->SizeOfIncludingThis(aMallocSizeOf);
    n += mInputQueue->SizeOfIncludingThis(aMallocSizeOf);
    n += mMediumHighQueue->SizeOfIncludingThis(aMallocSizeOf);
    n += mNormalQueue->SizeOfIncludingThis(aMallocSizeOf);
    n += mDeferredTimersQueue->SizeOfIncludingThis(aMallocSizeOf);
    n += mIdleQueue->SizeOfIncludingThis(aMallocSizeOf);

    if (mIdlePeriod) {
      n += aMallocSizeOf(mIdlePeriod);
    }

    return n;
  }

 private:
  EventQueuePriority SelectQueue(bool aUpdateState,
                                 const AutoLock& aProofOfLock);

  // Returns a null TimeStamp if we're not in the idle period.
  mozilla::TimeStamp GetIdleDeadline();

  UniquePtr<InnerQueueT> mHighQueue;
  UniquePtr<InnerQueueT> mInputQueue;
  UniquePtr<InnerQueueT> mMediumHighQueue;
  UniquePtr<InnerQueueT> mNormalQueue;
  UniquePtr<InnerQueueT> mDeferredTimersQueue;
  UniquePtr<InnerQueueT> mIdleQueue;

  // We need to drop the queue mutex when checking the idle deadline, so we keep
  // a pointer to it here.
  Lock* mMutex = nullptr;

#ifndef RELEASE_OR_BETA
  // Pointer to a place where the most recently computed idle deadline is
  // stored.
  TimeStamp* mNextIdleDeadline = nullptr;
#endif

  // Try to process one high priority runnable after each normal
  // priority runnable. This gives the processing model HTML spec has for
  // 'Update the rendering' in the case only vsync messages are in the
  // secondary queue and prevents starving the normal queue.
  bool mProcessHighPriorityQueue = false;

  // mIdlePeriod keeps track of the current idle period. If at any
  // time the main event queue is empty, calling
  // mIdlePeriod->GetIdlePeriodHint() will give an estimate of when
  // the current idle period will end.
  nsCOMPtr<nsIIdlePeriod> mIdlePeriod;

  // Set to true if HasPendingEvents() has been called and returned true because
  // of a pending idle event.  This is used to remember to return that idle
  // event from GetIdleEvent() to ensure that HasPendingEvents() never lies.
  bool mHasPendingEventsPromisedIdleEvent = false;

  TimeStamp mInputHandlingStartTime;

  enum InputEventQueueState {
    STATE_DISABLED,
    STATE_FLUSHING,
    STATE_SUSPEND,
    STATE_ENABLED
  };
  InputEventQueueState mInputQueueState = STATE_DISABLED;
};

class EventQueue;
extern template class PrioritizedEventQueue<EventQueue>;

}  // namespace mozilla

#endif  // mozilla_PrioritizedEventQueue_h
