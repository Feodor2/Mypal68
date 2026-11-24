/**
 * @file mouse_count_cache_test.cpp
 * @brief Verify mouse-count cache invalidation and stale enumeration races.
 *
 * Responsibilities:
 *  - Exercise the production cache with deterministic enumeration callbacks.
 * Invariants:
 *  - Invalidations reject results computed by an earlier generation.
 *  - Failed enumeration results are never cached.
 * Thread-safety:
 *  - The race test synchronizes two threads with owned Windows events.
 * Performance notes:
 *  - Tests do not benchmark the cache or query hardware.
 */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "MouseDeviceCountCache.h"

using mozilla::widget::MouseDeviceCountCache;
using mozilla::widget::MouseDeviceCountResult;

static bool TestCacheAndFailure() {
  MouseDeviceCountCache cache;
  unsigned calls = 0;
  auto success = [&calls]() -> MouseDeviceCountResult {
    ++calls;
    return {3, true};
  };
  if (cache.Get(success) != 3 || cache.Get(success) != 3 || calls != 1) {
    return false;
  }
  cache.Invalidate();
  if (cache.Get(success) != 3 || calls != 2) {
    return false;
  }
  cache.Invalidate();
  auto failure = [&calls]() -> MouseDeviceCountResult {
    ++calls;
    return {0, false};
  };
  if (cache.Get(failure) != 0 || cache.Get(success) != 3 || calls != 4) {
    return false;
  }
  return true;
}

static bool TestCompetingFill() {
  MouseDeviceCountCache cache;
  unsigned outerCalls = 0;
  unsigned innerCalls = 0;
  auto outer = [&]() -> MouseDeviceCountResult {
    ++outerCalls;
    auto inner = [&]() -> MouseDeviceCountResult {
      ++innerCalls;
      return {7, true};
    };
    if (cache.Get(inner) != 7) {
      return {0, false};
    }
    return {99, true};
  };
  return cache.Get(outer) == 7 && outerCalls == 1 && innerCalls == 1;
}

static bool TestGenerationSaturation() {
  MouseDeviceCountCache cache(UINT64_C(0xfffffffeffffffff));
  cache.Invalidate();
  unsigned calls = 0;
  auto enumerate = [&calls]() -> MouseDeviceCountResult {
    ++calls;
    return {calls, true};
  };
  return cache.Get(enumerate) == 1 && cache.Get(enumerate) == 2 && calls == 2;
}

struct RaceContext {
  MouseDeviceCountCache cache;
  HANDLE entered;
  HANDLE resume;
  unsigned calls;
  DWORD result;
  bool waitFailed;
};

static DWORD WINAPI EnumerateOnWorker(void* aArgument) {
  RaceContext* context = static_cast<RaceContext*>(aArgument);
  auto enumerate = [context]() -> MouseDeviceCountResult {
    ++context->calls;
    if (context->calls == 1) {
      SetEvent(context->entered);
      if (WaitForSingleObject(context->resume, 5000) != WAIT_OBJECT_0) {
        context->waitFailed = true;
        return {0, false};
      }
      return {1, true};
    }
    return {2, true};
  };
  context->result = context->cache.Get(enumerate);
  return 0;
}

static bool TestInvalidationDuringEnumeration() {
  RaceContext context{};
  context.entered = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  context.resume = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  if (!context.entered || !context.resume) {
    if (context.entered) CloseHandle(context.entered);
    if (context.resume) CloseHandle(context.resume);
    return false;
  }
  HANDLE worker = CreateThread(nullptr, 0, EnumerateOnWorker, &context, 0, nullptr);
  if (!worker) {
    CloseHandle(context.entered);
    CloseHandle(context.resume);
    return false;
  }

  const bool entered = WaitForSingleObject(context.entered, 5000) == WAIT_OBJECT_0;
  if (entered) {
    context.cache.Invalidate();
  }
  SetEvent(context.resume);
  const bool finished = WaitForSingleObject(worker, INFINITE) == WAIT_OBJECT_0;
  CloseHandle(worker);
  CloseHandle(context.entered);
  CloseHandle(context.resume);
  return entered && finished && !context.waitFailed && context.calls == 2 && context.result == 2;
}

int main() {
  if (!TestCacheAndFailure() || !TestCompetingFill() || !TestGenerationSaturation() ||
      !TestInvalidationDuringEnumeration()) {
    fputs("MOUSE_COUNT_CACHE_TEST_FAILED\n", stderr);
    return 1;
  }
  puts("MOUSE_COUNT_CACHE_TEST_OK");
  return 0;
}
