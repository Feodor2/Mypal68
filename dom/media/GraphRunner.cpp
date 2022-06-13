/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GraphRunner.h"

#include "GraphDriver.h"
#include "MediaStreamGraph.h"
#include "MediaStreamGraphImpl.h"
#include "mozilla/dom/WorkletThread.h"
#include "nsISupportsImpl.h"
#include "prthread.h"
#include "Tracing.h"

namespace mozilla {

static void Start(void* aArg) {
  GraphRunner* th = static_cast<GraphRunner*>(aArg);
  th->Run();
}

GraphRunner::GraphRunner(MediaStreamGraphImpl* aGraph)
    : mMonitor("GraphRunner::mMonitor"),
      mGraph(aGraph),
      mStateEnd(0),
      mStillProcessing(true),
      mThreadState(ThreadState::Wait),
      // Note that mThread needs to be initialized last, as it may pre-empt the
      // thread running this ctor and enter Run() with uninitialized members.
      mThread(PR_CreateThread(PR_SYSTEM_THREAD, &Start, this,
                              PR_PRIORITY_URGENT, PR_GLOBAL_THREAD,
                              PR_JOINABLE_THREAD, 0)) {
  MOZ_COUNT_CTOR(GraphRunner);
}

GraphRunner::~GraphRunner() {
  MOZ_COUNT_DTOR(GraphRunner);
  MOZ_ASSERT(mThreadState == ThreadState::Shutdown);
}

void GraphRunner::Shutdown() {
  {
    Monitor2AutoLock lock(mMonitor);
    MOZ_ASSERT(mThreadState == ThreadState::Wait);
    mThreadState = ThreadState::Shutdown;
    mMonitor.Signal();
  }
  // We need to wait for runner thread shutdown here for the sake of the
  // xpcomWillShutdown case, so that the main thread is not shut down before
  // cleanup messages are sent for objects destroyed in
  // CycleCollectedJSContext shutdown.
  PR_JoinThread(mThread);
  mThread = nullptr;
}

bool GraphRunner::OneIteration(GraphTime aStateEnd) {
  TRACE_AUDIO_CALLBACK();

  Monitor2AutoLock lock(mMonitor);
  MOZ_ASSERT(mThreadState == ThreadState::Wait);
  mStateEnd = aStateEnd;

#ifdef DEBUG
  if (auto audioDriver = mGraph->CurrentDriver()->AsAudioCallbackDriver()) {
    mAudioDriverThreadId = audioDriver->ThreadId();
  } else if (auto clockDriver =
                 mGraph->CurrentDriver()->AsSystemClockDriver()) {
    mClockDriverThread = clockDriver->Thread();
  } else {
    MOZ_CRASH("Unknown GraphDriver");
  }
#endif
  // Signal that mStateEnd was updated
  mThreadState = ThreadState::Run;
  mMonitor.Signal();
  // Wait for mStillProcessing to update
  do {
    mMonitor.Wait();
  } while (mThreadState == ThreadState::Run);

#ifdef DEBUG
  mAudioDriverThreadId = std::thread::id();
  mClockDriverThread = nullptr;
#endif

  return mStillProcessing;
}

void GraphRunner::Run() {
  PR_SetCurrentThreadName("GraphRunner");
  Monitor2AutoLock lock(mMonitor);
  while (true) {
    while (mThreadState == ThreadState::Wait) {
      mMonitor.Wait();  // Wait for mStateEnd to update or for shutdown
    }
    if (mThreadState == ThreadState::Shutdown) {
      break;
    }
    TRACE();
    mStillProcessing = mGraph->OneIterationImpl(mStateEnd);
    // Signal that mStillProcessing was updated
    mThreadState = ThreadState::Wait;
    mMonitor.Signal();
  }

  dom::WorkletThread::DeleteCycleCollectedJSContext();
}

bool GraphRunner::OnThread() { return PR_GetCurrentThread() == mThread; }

#ifdef DEBUG
bool GraphRunner::RunByGraphDriver(GraphDriver* aDriver) {
  if (!OnThread()) {
    return false;
  }

  if (auto audioDriver = aDriver->AsAudioCallbackDriver()) {
    return audioDriver->ThreadId() == mAudioDriverThreadId;
  }

  if (auto clockDriver = aDriver->AsSystemClockDriver()) {
    return clockDriver->Thread() == mClockDriverThread;
  }

  MOZ_CRASH("Unknown driver");
}
#endif

}  // namespace mozilla
