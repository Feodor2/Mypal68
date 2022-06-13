/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_Monitor2_h
#define mozilla_Monitor2_h

#include "base/condition_variable.h"
#include "base/lock.h"

namespace mozilla {

/**
 * Monitor provides a *non*-reentrant monitor: *not* a Java-style
 * monitor.  If your code needs support for reentrancy, use
 * ReentrantMonitor instead.  (Rarely should reentrancy be needed.)
 *
 * Instead of directly calling Monitor methods, it's safer and simpler
 * to instead use the RAII wrappers MonitorAutoLock and
 * MonitorAutoUnlock.
 */
class Monitor2 {
 public:
  explicit Monitor2(const char* aName)
      : mMutex(aName), mCondVar(mMutex, aName) {}

  ~Monitor2() {}

  void Acquire() { mMutex.Acquire(); }
  bool Try() { return mMutex.Try(); }
  void Release() { mMutex.Release(); }

  void Wait() { mCondVar.Wait(); }
  CVStatus2 Wait(TimeDuration aDuration) { return mCondVar.TimedWait(aDuration); }

  nsresult Signal() { mCondVar.Signal(); return NS_OK; }
  nsresult Broadcast() { mCondVar.Broadcast(); return NS_OK; }

  void AssertCurrentThreadOwns() const {}

  void AssertNotCurrentThreadOwns() const {}

 private:
  Monitor2();
  Monitor2(const Monitor2&);
  Monitor2& operator=(const Monitor2&);

  Lock mMutex;
  ConditionVariable mCondVar;
};

/**
 * Lock the monitor for the lexical scope instances of this class are
 * bound to (except for MonitorAutoUnlock in nested scopes).
 *
 * The monitor must be unlocked when instances of this class are
 * created.
 */
class MOZ_STACK_CLASS Monitor2AutoLock {
 public:
  explicit Monitor2AutoLock(Monitor2& aMonitor) : mMonitor(&aMonitor) {
    mMonitor->Acquire();
  }

  ~Monitor2AutoLock() { mMonitor->Release(); }

  void Wait() { mMonitor->Wait(); }
  CVStatus2 Wait(TimeDuration aDuration) { return mMonitor->Wait(aDuration); }

  nsresult Signal() { return mMonitor->Signal(); }
  nsresult Broadcast() { return mMonitor->Broadcast(); }

 private:
  Monitor2AutoLock();
  Monitor2AutoLock(const Monitor2AutoLock&);
  Monitor2AutoLock& operator=(const Monitor2AutoLock&);
  static void* operator new(size_t) CPP_THROW_NEW;

  friend class Monitor2AutoUnlock;

  Monitor2* mMonitor;
};

/**
 * Unlock the monitor for the lexical scope instances of this class
 * are bound to (except for MonitorAutoLock in nested scopes).
 *
 * The monitor must be locked by the current thread when instances of
 * this class are created.
 */
class MOZ_STACK_CLASS Monitor2AutoUnlock {
 public:
  explicit Monitor2AutoUnlock(Monitor2& aMonitor) : mMonitor(&aMonitor) {
    mMonitor->Release();
  }

  explicit Monitor2AutoUnlock(Monitor2AutoLock& aMonitorLock)
      : mMonitor(aMonitorLock.mMonitor) {
    mMonitor->Release();
  }

  ~Monitor2AutoUnlock() { mMonitor->Acquire(); }

 private:
  Monitor2AutoUnlock();
  Monitor2AutoUnlock(const Monitor2AutoUnlock&);
  Monitor2AutoUnlock& operator=(const Monitor2AutoUnlock&);
  static void* operator new(size_t) CPP_THROW_NEW;

  Monitor2* mMonitor;
};

}  // namespace mozilla

#endif  // mozilla_Monitor_h
