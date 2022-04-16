/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/Assertions.h"

#include <float.h>
#include <intrin.h>
#include <stdlib.h>
#include <windows.h>

#include "mozilla/PlatformConditionVariable.h"
#include "mozilla/PlatformMutex.h"
#include "MutexPlatformData_windows.h"

// Some versions of the Windows SDK have a bug where some interlocked functions
// are not redefined as compiler intrinsics. Fix that for the interlocked
// functions that are used in this file.
#if defined(_MSC_VER) && !defined(InterlockedExchangeAdd)
#  define InterlockedExchangeAdd(addend, value) \
    _InterlockedExchangeAdd((volatile long*)(addend), (long)(value))
#endif

#if defined(_MSC_VER) && !defined(InterlockedIncrement)
#  define InterlockedIncrement(addend) \
    _InterlockedIncrement((volatile long*)(addend))
#endif

// Wrapper for native condition variable APIs.
/* struct mozilla::detail::ConditionVariableImpl::PlatformData {
  CONDITION_VARIABLE cv_;
};

mozilla::detail::ConditionVariableImpl::ConditionVariableImpl() {
  InitializeConditionVariable(&platformData()->cv_);
}

void mozilla::detail::ConditionVariableImpl::notify_one() {
  WakeConditionVariable(&platformData()->cv_);
}

void mozilla::detail::ConditionVariableImpl::notify_all() {
  WakeAllConditionVariable(&platformData()->cv_);
}

void mozilla::detail::ConditionVariableImpl::wait(MutexImpl& lock) {
  SRWLOCK* srwlock = &lock.platformData()->lock;
  bool r =
      SleepConditionVariableSRW(&platformData()->cv_, srwlock, INFINITE, 0);
  MOZ_RELEASE_ASSERT(r);
}

mozilla::CVStatus mozilla::detail::ConditionVariableImpl::wait_for(
    MutexImpl& lock, const mozilla::TimeDuration& rel_time) {
  if (rel_time == mozilla::TimeDuration::Forever()) {
    wait(lock);
    return CVStatus::NoTimeout;
  }

  SRWLOCK* srwlock = &lock.platformData()->lock;

  // Note that DWORD is unsigned, so we have to be careful to clamp at 0. If
  // rel_time is Forever, then ToMilliseconds is +inf, which evaluates as
  // greater than UINT32_MAX, resulting in the correct INFINITE wait. We also
  // don't want to round sub-millisecond waits to 0, as that wastes energy (see
  // bug 1437167 comment 6), so we instead round submillisecond waits to 1ms.
  double msecd = rel_time.ToMilliseconds();
  DWORD msec;
  if (msecd < 0.0) {
    msec = 0;
  } else if (msecd > UINT32_MAX) {
    msec = INFINITE;
  } else {
    msec = static_cast<DWORD>(msecd);
    // Round submillisecond waits to 1ms.
    if (msec == 0 && !rel_time.IsZero()) {
      msec = 1;
    }
  }

  BOOL r = SleepConditionVariableSRW(&platformData()->cv_, srwlock, msec, 0);
  if (r) return CVStatus::NoTimeout;
  MOZ_RELEASE_ASSERT(GetLastError() == ERROR_TIMEOUT);
  return CVStatus::Timeout;
}

mozilla::detail::ConditionVariableImpl::~ConditionVariableImpl() {
  // Native condition variables don't require cleanup.
}

inline mozilla::detail::ConditionVariableImpl::PlatformData*
mozilla::detail::ConditionVariableImpl::platformData() {
  static_assert(sizeof platformData_ >= sizeof(PlatformData),
                "platformData_ is too small");
  return reinterpret_cast<PlatformData*>(platformData_);




}*/

  static const uint32_t WAKEUP_MODE_NONE = 0;
  static const uint32_t WAKEUP_MODE_ONE = 0x40000000;
  static const uint32_t WAKEUP_MODE_ALL = 0x80000000;

  static const uint32_t WAKEUP_MODE_MASK = WAKEUP_MODE_ONE | WAKEUP_MODE_ALL;
  static const uint32_t SLEEPERS_COUNT_MASK = ~WAKEUP_MODE_MASK;


// Fallback condition variable support for Windows XP and Server 2003. Given the
// difficulty of testing on these antiquated platforms and their rapidly
// diminishing market share, this implementation trades performance for
// predictable behavior.
class ConditionVariableFallback
{
  /*HANDLE waiting_sem_;
  HANDLE received_sem_;
  HANDLE signal_event_;*/
public:
  void initialize()
  {
    // Initialize the state variable to 0 sleepers, no wakeup.
    sleepersCountAndWakeupMode_ = 0 | WAKEUP_MODE_NONE;

    // Create a semaphore that prevents threads from entering sleep,
    // or waking other threads while a wakeup is ongoing.
    sleepWakeupSemaphore_ = CreateSemaphoreW(NULL, 1, 1, NULL);
    MOZ_RELEASE_ASSERT(sleepWakeupSemaphore_);

    // Use an auto-reset event for waking up a single sleeper.
    wakeOneEvent_ = CreateEventW(NULL, FALSE, FALSE, NULL);
    MOZ_RELEASE_ASSERT(wakeOneEvent_);

    // Use a manual-reset event for waking up all sleepers.
    wakeAllEvent_ = CreateEventW(NULL, TRUE, FALSE, NULL);
    MOZ_RELEASE_ASSERT(wakeAllEvent_);
    /*waiting_sem_ = CreateSemaphore(NULL, 0, MAX_DECODE_THREADS, NULL);
    MOZ_RELEASE_ASSERT(waiting_sem_);
    received_sem_ = CreateSemaphore(NULL, 0, MAX_DECODE_THREADS, NULL);
    MOZ_RELEASE_ASSERT(received_sem_);
    signal_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    MOZ_RELEASE_ASSERT(signal_event_);*/
  }

  void destroy()
  {
    BOOL r;

    //MOZ_RELEASE_ASSERT(sleepersCountAndWakeupMode_ == (0 | WAKEUP_MODE_NONE));

    r = CloseHandle(sleepWakeupSemaphore_);
    //MOZ_RELEASE_ASSERT(r);

    r = CloseHandle(wakeOneEvent_);
    //MOZ_RELEASE_ASSERT(r);

    r = CloseHandle(wakeAllEvent_);
    //MOZ_RELEASE_ASSERT(r);
    /*CloseHandle(waiting_sem_);
    CloseHandle(received_sem_);
    CloseHandle(signal_event_);*/
  }

private:
  void wakeup(uint32_t wakeupMode, HANDLE wakeEvent)
  {
    // Ensure that only one thread at a time can wake up others.
    BOOL result = WaitForSingleObject(sleepWakeupSemaphore_, INFINITE);
    //MOZ_RELEASE_ASSERT(result == WAIT_OBJECT_0);

    // Atomically set the wakeup mode and retrieve the number of sleepers.
    uint32_t wcwm = InterlockedExchangeAdd(&sleepersCountAndWakeupMode_,
                                           wakeupMode);
    uint32_t sleepersCount = wcwm & SLEEPERS_COUNT_MASK;
    //MOZ_RELEASE_ASSERT((wcwm & WAKEUP_MODE_MASK) == WAKEUP_MODE_NONE);

    if (sleepersCount > 0) {
      // If there are any sleepers, set the wake event. The (last) woken
      // up thread is responsible for releasing the semaphore.
      BOOL success = SetEvent(wakeEvent);
      MOZ_RELEASE_ASSERT(success);

    } else {
      // If there are no sleepers, set the wakeup mode back to 'none'
      // and release the semaphore ourselves.
      sleepersCountAndWakeupMode_ = 0 | WAKEUP_MODE_NONE;

      BOOL success = ReleaseSemaphore(sleepWakeupSemaphore_, 1, NULL);
      //MOZ_RELEASE_ASSERT(success);
    }
  }

public:
  void notify_one() { wakeup(WAKEUP_MODE_ONE, wakeOneEvent_); }

  void notify_all() { wakeup(WAKEUP_MODE_ALL, wakeAllEvent_); }

  bool wait(CRITICAL_SECTION* userLock, DWORD msec)
  {
    // Make sure that we can't enter sleep when there are other threads
    // that still need to wake up on either of the wake events being set.
    DWORD result = WaitForSingleObject(sleepWakeupSemaphore_, INFINITE);
    //MOZ_RELEASE_ASSERT(result == WAIT_OBJECT_0);

    // Register ourselves as a sleeper. Use an atomic operation, because
    // if another thread times out at the same time, it will decrement the
    // sleepers count without acquiring the semaphore.
    uint32_t wcwm = InterlockedIncrement(&sleepersCountAndWakeupMode_);
    //MOZ_RELEASE_ASSERT((wcwm & WAKEUP_MODE_MASK) == WAKEUP_MODE_NONE);

    // Now that that this thread has been enlisted as a sleeper, it is safe
    // again for other threads to do a wakeup.
    BOOL success = ReleaseSemaphore(sleepWakeupSemaphore_, 1, NULL);
    MOZ_RELEASE_ASSERT(success);

    // Release the caller's mutex.
    LeaveCriticalSection(userLock);

    // Wait for either event to become signaled, which happens when
    // notify_one() or notify_all() is called, or for a timeout.
    HANDLE handles[2] = { wakeOneEvent_, wakeAllEvent_ };
    DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, msec);
    /*MOZ_RELEASE_ASSERT(waitResult == WAIT_OBJECT_0 ||
                       waitResult == WAIT_OBJECT_0 + 1 ||
                       (waitResult == WAIT_TIMEOUT && msec != INFINITE));*/

    // Atomically decrease the sleepers count and retrieve the wakeup mode
    // and new sleepers count.
    // If the wait returned because wakeOneEvent_ was set, we are certain
    // that the wakeup mode will be WAKEUP_MODE_ONE. In that case,
    // atomically reset the wakeup mode to 'none', because if another
    // thread's sleep times out at same time and it finds that it was the
    // last sleeper, it decides whether or not to reset the wakeOneEvent_
    // based on the current wakeup mode.
    uint32_t sub;
    if (waitResult == WAIT_OBJECT_0)
      sub = 1 | WAKEUP_MODE_ONE;
    else
      sub = 1;
    // Note that InterlockedExchangeAdd returns the old value, but it's
    // easier to work with the new value.
    wcwm = InterlockedExchangeAdd(&sleepersCountAndWakeupMode_, -sub) - sub;

    uint32_t wakeupMode = wcwm & WAKEUP_MODE_MASK;
    uint32_t sleepersCount = wcwm & SLEEPERS_COUNT_MASK;

    bool releaseSleepWakeupSemaphore = false;

    if (waitResult == WAIT_OBJECT_0) {
      // The wake-one event is an auto-reset event so if we're woken by
      // it, it should already have been reset. We also already removed
      // the WAKEUP_MODE_ONE bit so the wakeup mode should now be 'none'
      // again.
      //MOZ_RELEASE_ASSERT(wakeupMode == WAKEUP_MODE_NONE);

      // The signaling thread has acquired the enter-wakeup semaphore and
      // expects the woken (this) thread to release it again.
      releaseSleepWakeupSemaphore = true;

    } else if (waitResult == WAIT_TIMEOUT && wakeupMode == WAKEUP_MODE_ONE &&
               sleepersCount == 0) {
      // In theory a race condition is possible where the last sleeper
      // times out right at the moment that another thread signals it.
      // If that just happened we now have a dangling signal event and
      // mode, but no threads to be woken up by it, and we need to clean
      // that up.
      BOOL success = ResetEvent(wakeOneEvent_);
      MOZ_RELEASE_ASSERT(success);

      // This is safe - we are certain there are no other sleepers that
      // could wake up right now, and the semaphore ensures that no
      // non-sleeping threads are messing with
      // sleepersCountAndWakeupMode_.
      sleepersCountAndWakeupMode_ = 0 | WAKEUP_MODE_NONE;

      // The signaling thread has acquired the sleep-wakeup semaphore and
      // expects the woken thread to release it. But since there are no
      // sleeping threads left this thread will do it instead.
      releaseSleepWakeupSemaphore = true;

    } else if (wakeupMode == WAKEUP_MODE_ALL && sleepersCount == 0) {
      // If this was the last thread waking up in response to a
      // notify_all, clear the wakeup mode and reset the wake-all event.
      // A race condition similar to the case described above could
      // occur, so waitResult could be WAIT_TIMEOUT, but that doesn't
      // matter for the actions that need to be taken.
      MOZ_RELEASE_ASSERT(waitResult = WAIT_OBJECT_0 + 1 ||
                         waitResult == WAIT_TIMEOUT);

      BOOL success = ResetEvent(wakeAllEvent_);
      MOZ_RELEASE_ASSERT(success);

      sleepersCountAndWakeupMode_ = 0 | WAKEUP_MODE_NONE;

      // The broadcasting thread has acquired the enter-wakeup semaphore
      // and expects the last thread that wakes up to release it.
      releaseSleepWakeupSemaphore = true;

    } else if ((waitResult == WAIT_TIMEOUT && msec != INFINITE) ||
               (waitResult == WAIT_OBJECT_0 + 1 &&
                wakeupMode == WAKEUP_MODE_ALL)) {
      // Either:
      //   * The wait timed out but found no active notify_one or notify_all
      //     the moment it decreased the wait count.
      //   * A notify_all woke up this thread but there are more threads
      //     that need to be woken up by the wake-all event.
      // These are ordinary conditions in which we don't have to do
      // anything.

    } else {
      MOZ_CRASH("invalid wakeup condition");
    }

    // Release the enter-wakeup semaphore if the wakeup condition requires
    // us to do it.
    if (releaseSleepWakeupSemaphore) {
      BOOL success = ReleaseSemaphore(sleepWakeupSemaphore_, 1, NULL);
      //MOZ_RELEASE_ASSERT(success);
    }

    // Reacquire the user mutex.
    EnterCriticalSection(userLock);

    // Return true if woken up, false when timed out.
    if (waitResult == WAIT_TIMEOUT) {
      SetLastError(ERROR_TIMEOUT);
      return false;
    }
    return true;
  }

private:
  uint32_t sleepersCountAndWakeupMode_;
  HANDLE sleepWakeupSemaphore_;
  HANDLE wakeOneEvent_;
  HANDLE wakeAllEvent_;
};

struct mozilla::detail::ConditionVariableImpl::PlatformData
{
    ConditionVariableFallback fallback;
};

mozilla::detail::ConditionVariableImpl::ConditionVariableImpl() {
    platformData()->fallback.initialize();
}

void mozilla::detail::ConditionVariableImpl::notify_one() {
    platformData()->fallback.notify_one();
}

void mozilla::detail::ConditionVariableImpl::notify_all() {
    platformData()->fallback.notify_all();
}

void mozilla::detail::ConditionVariableImpl::wait(MutexImpl& lock) {
  CRITICAL_SECTION* cs = &lock.platformData->criticalSection;
  bool r;
  //if (sNativeImports.supported())
  //  r = platformData()->native.wait(cs, INFINITE);
  //else
    r = platformData()->fallback.wait(cs, INFINITE);
  MOZ_RELEASE_ASSERT(r);
}

mozilla::CVStatus mozilla::detail::ConditionVariableImpl::wait_for(
    MutexImpl& lock, const mozilla::TimeDuration& rel_time) {
  CRITICAL_SECTION* cs = &lock.platformData->criticalSection;

  // Note that DWORD is unsigned, so we have to be careful to clamp at 0.
  // If rel_time is Forever, then ToMilliseconds is +inf, which evaluates as
  // greater than UINT32_MAX, resulting in the correct INFINITE wait.
  double msecd = rel_time.ToMilliseconds();
  DWORD msec = msecd < 0.0
               ? 0
               : msecd > UINT32_MAX
                 ? INFINITE
                 : static_cast<DWORD>(msecd);

  BOOL r;
  //if (sNativeImports.supported())
  //  r = platformData()->native.wait(cs, msec);
  //else
    r = platformData()->fallback.wait(cs, msec);
  if (r)
    return CVStatus::NoTimeout;
  MOZ_RELEASE_ASSERT(GetLastError() == ERROR_TIMEOUT);
  return CVStatus::Timeout;
}

mozilla::detail::ConditionVariableImpl::~ConditionVariableImpl() {
  //if (sNativeImports.supported())
  //  platformData()->native.destroy();
  //else
    platformData()->fallback.destroy();
}

inline mozilla::detail::ConditionVariableImpl::PlatformData*
mozilla::detail::ConditionVariableImpl::platformData() {
  static_assert(sizeof platformData_ >= sizeof(PlatformData),
                "platformData_ is too small");
  return reinterpret_cast<PlatformData*>(platformData_);
}