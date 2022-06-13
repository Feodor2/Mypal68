/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/EventQueue.h"
#include "nsIRunnable.h"

using namespace mozilla;

EventQueue::EventQueue(EventQueuePriority aPriority) {}

void EventQueue::PutEvent(already_AddRefed<nsIRunnable>&& aEvent,
                          EventQueuePriority aPriority,
                          const AutoLock& aProofOfLock) {
  nsCOMPtr<nsIRunnable> event(aEvent);
  mQueue.Push(std::move(event));
}

already_AddRefed<nsIRunnable> EventQueue::GetEvent(
    EventQueuePriority* aPriority, const AutoLock& aProofOfLock) {
  if (mQueue.IsEmpty()) {
    return nullptr;
  }

  if (aPriority) {
    *aPriority = EventQueuePriority::Normal;
  }

  nsCOMPtr<nsIRunnable> result = mQueue.Pop();
  return result.forget();
}

bool EventQueue::IsEmpty(const AutoLock& aProofOfLock) {
  return mQueue.IsEmpty();
}

bool EventQueue::HasReadyEvent(const AutoLock& aProofOfLock) {
  return !IsEmpty(aProofOfLock);
}

already_AddRefed<nsIRunnable> EventQueue::PeekEvent(
    const AutoLock& aProofOfLock) {
  if (mQueue.IsEmpty()) {
    return nullptr;
  }

  nsCOMPtr<nsIRunnable> result = mQueue.FirstElement();
  return result.forget();
}

size_t EventQueue::Count(const AutoLock& aProofOfLock) const {
  return mQueue.Count();
}
