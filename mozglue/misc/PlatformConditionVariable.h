/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_ConditionVariable_h
#define mozilla_ConditionVariable_h

#include "mozilla/Attributes.h"
#include "mozilla/Move.h"
#include "mozilla/PlatformMutex.h"
#include "mozilla/TimeStamp.h"

#include <stdint.h>
#ifndef XP_WIN
#  include <pthread.h>
#endif

namespace mozilla {

enum class CVStatus { NoTimeout, Timeout };

namespace detail {

class ConditionVariableImpl {
 public:
  struct PlatformData;

  MFBT_API ConditionVariableImpl();
  MFBT_API ~ConditionVariableImpl();

  // Wake one thread that is waiting on this condition.
  MFBT_API void notify_one();

  // Wake all threads that are waiting on this condition.
  MFBT_API void notify_all();

  // Atomically release |lock| and sleep the current thread of execution on
  // this condition variable.
  // |lock| will be re-acquired before this function returns.
  // The thread may be woken from sleep from another thread via notify_one()
  // or notify_all(), but may also wake spuriously.  The caller should recheck
  // its predicate after this function returns, typically in a while loop.
  MFBT_API void wait(MutexImpl& lock);

  MFBT_API CVStatus wait_for(MutexImpl& lock,
                             const mozilla::TimeDuration& rel_time);

 private:
  ConditionVariableImpl(const ConditionVariableImpl&) = delete;
  ConditionVariableImpl& operator=(const ConditionVariableImpl&) = delete;

#ifndef XP_WIN
  PlatformData* platformData();
  void* platformData_[sizeof(pthread_cond_t) / sizeof(void*)];
  static_assert(sizeof(pthread_cond_t) / sizeof(void*) != 0 &&
                    sizeof(pthread_cond_t) % sizeof(void*) == 0,
                "pthread_cond_t must have pointer alignment");
#else
  uint32_t sleepersCountAndWakeupMode_;
  void* sleepWakeupSemaphore_;
  void* wakeOneEvent_;
  void* wakeAllEvent_;
#endif
};

}  // namespace detail

}  // namespace mozilla

#endif  // mozilla_ConditionVariable_h
