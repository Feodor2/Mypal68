/**
 * @file MouseDeviceCountCache.h
 * @brief Cache mouse enumeration with generation-based invalidation.
 *
 * Responsibilities:
 *  - Reuse a successful mouse interface count until invalidation.
 * Invariants:
 *  - An enumeration started before invalidation cannot populate the new generation.
 *  - Failed enumerations are returned without being cached.
 *  - Generation saturation permanently disables caching instead of wrapping.
 * Thread-safety:
 *  - State transitions use sequentially consistent atomic compare-and-swap.
 * Performance notes:
 *  - Cache hits do not enumerate devices or allocate memory.
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef mozilla_widget_MouseDeviceCountCache_h__
#define mozilla_widget_MouseDeviceCountCache_h__

#include <atomic>
#include <stdint.h>
#include "mozilla/Atomics.h"

namespace mozilla {
namespace widget {

static_assert(std::atomic<uint64_t>::is_always_lock_free,
              "Mouse device count cache requires lock-free 64-bit atomics");
static_assert(alignof(std::atomic<uint64_t>) >= 8,
              "Mouse device count cache requires naturally aligned atomic state");

struct MouseDeviceCountResult {
  uint32_t count;
  bool cacheable;
};

class MouseDeviceCountCache final {
 public:
  constexpr MouseDeviceCountCache() : mState(kInvalidCount) {}
#ifdef MYPAL_MOUSE_CACHE_TEST
  explicit constexpr MouseDeviceCountCache(uint64_t aInitialState) : mState(aInitialState) {}
#endif

  template <typename Enumerate>
  uint32_t Get(Enumerate&& aEnumerate) {
    for (;;) {
      const uint64_t before = mState;
      const uint32_t cached = static_cast<uint32_t>(before);
      if (cached != kInvalidCount) {
        return cached;
      }

      const MouseDeviceCountResult result = aEnumerate();
      if (!result.cacheable || result.count == kInvalidCount || before == kSaturated) {
        return result.count;
      }

      const uint64_t desired = (before & kGenerationMask) | result.count;
      if (mState.compareExchange(before, desired)) {
        return result.count;
      }
    }
  }

  void Invalidate() {
    for (;;) {
      const uint64_t before = mState;
      const uint32_t generation = static_cast<uint32_t>(before >> 32);
      const uint64_t next = generation == UINT32_MAX
                                ? kSaturated
                                : (uint64_t(generation + 1) << 32) | kInvalidCount;
      if (mState.compareExchange(before, next)) {
        return;
      }
    }
  }

 private:
  static constexpr uint32_t kInvalidCount = UINT32_MAX;
  static constexpr uint64_t kGenerationMask = UINT64_C(0xffffffff00000000);
  static constexpr uint64_t kSaturated = UINT64_MAX;
  Atomic<uint64_t, SequentiallyConsistent> mState;
};

}  // namespace widget
}  // namespace mozilla
#endif  // mozilla_widget_MouseDeviceCountCache_h__
