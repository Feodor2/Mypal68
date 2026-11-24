/**
 * @file BaselineLargeScriptPolicy.h
 * @brief Define experimental warm-up policy for very large functions.
 *
 * Responsibilities:
 *  - Identify functions whose Baseline compilation may be costly.
 *  - Provide a bounded warm-up threshold shared by JIT and C++ paths.
 *
 * Invariants:
 *  - Only function bytecode above the size threshold is deferred.
 *  - A deferred function remains eligible for later compilation.
 *
 * Thread-safety:
 *  - Constants and the classification function have no mutable state.
 *
 * Performance notes:
 *  - The thresholds are experimental and require page-level measurement.
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_BaselineLargeScriptPolicy_h
#define jit_BaselineLargeScriptPolicy_h

#include <stddef.h>
#include <stdint.h>

namespace js {
namespace jit {

constexpr size_t kLargeBaselineBytecode = 1000000;
constexpr uint32_t kLargeBaselineWarmUp = 10000;

constexpr bool DeferLargeBaseline(bool isFunction, size_t bytecodeSize) noexcept {
  return isFunction && bytecodeSize > kLargeBaselineBytecode;
}

constexpr bool DeferLargeBaselineAtWarmUp(bool isFunction, size_t bytecodeSize,
                                          uint32_t warmUpCount) noexcept {
  return DeferLargeBaseline(isFunction, bytecodeSize) &&
         warmUpCount <= kLargeBaselineWarmUp;
}

}  // namespace jit
}  // namespace js

#endif
