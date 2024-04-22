/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_EventQueue_h
#define mozilla_EventQueue_h

#include "mozilla/AbstractEventQueue.h"
#include "mozilla/Queue.h"
#include "nsCOMPtr.h"

class nsIRunnable;

namespace mozilla {

namespace detail {

template <size_t ItemsPerPage>
class EventQueueInternal : public AbstractEventQueue {
 public:
  static const bool SupportsPrioritization = false;

  EventQueueInternal() {}
  explicit EventQueueInternal(EventQueuePriority aPriority);

  void PutEvent(already_AddRefed<nsIRunnable>&& aEvent,
                EventQueuePriority aPriority, const AutoLock& aProofOfLock,
                mozilla::TimeDuration* aDelay = nullptr) final;
  already_AddRefed<nsIRunnable> GetEvent(
      EventQueuePriority* aPriority, const AutoLock& aProofOfLock,
      mozilla::TimeDuration* aLastEventDelay = nullptr) final;

  bool IsEmpty(const AutoLock& aProofOfLock) final;
  bool HasReadyEvent(const AutoLock& aProofOfLock) final;
  bool HasPendingHighPriorityEvents(const AutoLock& aProofOfLock) final {
    // EventQueueInternal doesn't support any prioritization.
    return false;
  }

  size_t Count(const AutoLock& aProofOfLock) const final;
  // For some reason, if we put this in the .cpp file the linker can't find it
  already_AddRefed<nsIRunnable> PeekEvent(const AutoLock& aProofOfLock) {
    if (mQueue.IsEmpty()) {
      return nullptr;
    }

    nsCOMPtr<nsIRunnable> result = mQueue.FirstElement();
    return result.forget();
  }

  void EnableInputEventPrioritization(const AutoLock& aProofOfLock) final {
  }
  void FlushInputEventPrioritization(const AutoLock& aProofOfLock) final {}
  void SuspendInputEventPrioritization(
      const AutoLock& aProofOfLock) final {}
  void ResumeInputEventPrioritization(const AutoLock& aProofOfLock) final {
  }

  size_t SizeOfExcludingThis(
      mozilla::MallocSizeOf aMallocSizeOf) const override {
    size_t size = mQueue.ShallowSizeOfExcludingThis(aMallocSizeOf);
#ifdef MOZ_GECKO_PROFILER
    size += mDispatchTimes.ShallowSizeOfExcludingThis(aMallocSizeOf);
#endif
    return size;
  }

 private:
  mozilla::Queue<nsCOMPtr<nsIRunnable>, ItemsPerPage> mQueue;
#ifdef MOZ_GECKO_PROFILER
  // This queue is only populated when the profiler is turned on.
  mozilla::Queue<mozilla::TimeStamp, ItemsPerPage> mDispatchTimes;
  TimeDuration mLastEventDelay;
#endif
};

}  // namespace detail

class EventQueue final : public mozilla::detail::EventQueueInternal<16> {
 public:
  EventQueue() : mozilla::detail::EventQueueInternal<16>() {}
  explicit EventQueue(EventQueuePriority aPriority)
      : mozilla::detail::EventQueueInternal<16>(aPriority){};
};

template <size_t ItemsPerPage = 16>
class EventQueueSized final
    : public mozilla::detail::EventQueueInternal<ItemsPerPage> {
 public:
  EventQueueSized() : mozilla::detail::EventQueueInternal<ItemsPerPage>() {}
  explicit EventQueueSized(EventQueuePriority aPriority)
      : mozilla::detail::EventQueueInternal<ItemsPerPage>(aPriority){};
};

}  // namespace mozilla

#endif  // mozilla_EventQueue_h
