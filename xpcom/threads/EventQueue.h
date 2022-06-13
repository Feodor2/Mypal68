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

class EventQueue final : public AbstractEventQueue {
 public:
  static const bool SupportsPrioritization = false;

  EventQueue() {}
  explicit EventQueue(EventQueuePriority aPriority);

  void PutEvent(already_AddRefed<nsIRunnable>&& aEvent,
                EventQueuePriority aPriority,
                const AutoLock& aProofOfLock) final;
  already_AddRefed<nsIRunnable> GetEvent(
      EventQueuePriority* aPriority, const AutoLock& aProofOfLock) final;

  bool IsEmpty(const AutoLock& aProofOfLock) final;
  bool HasReadyEvent(const AutoLock& aProofOfLock) final;
  bool HasPendingHighPriorityEvents(const AutoLock& aProofOfLock) final {
    // EventQueue doesn't support any prioritization.
    return false;
  }

  size_t Count(const AutoLock& aProofOfLock) const final;
  already_AddRefed<nsIRunnable> PeekEvent(const AutoLock& aProofOfLock);

  void EnableInputEventPrioritization(const AutoLock& aProofOfLock) final {
  }
  void FlushInputEventPrioritization(const AutoLock& aProofOfLock) final {}
  void SuspendInputEventPrioritization(
      const AutoLock& aProofOfLock) final {}
  void ResumeInputEventPrioritization(const AutoLock& aProofOfLock) final {
  }

  size_t SizeOfExcludingThis(
      mozilla::MallocSizeOf aMallocSizeOf) const override {
    return mQueue.ShallowSizeOfExcludingThis(aMallocSizeOf);
  }

 private:
  mozilla::Queue<nsCOMPtr<nsIRunnable>> mQueue;
};

}  // namespace mozilla

#endif  // mozilla_EventQueue_h
