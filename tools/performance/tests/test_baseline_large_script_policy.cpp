/**
 * @file test_baseline_large_script_policy.cpp
 * @brief Verify experimental large-function warm-up boundaries.
 *
 * Responsibilities:
 *  - Check classification and transition at exact threshold values.
 *
 * Invariants:
 *  - Normal functions and top-level scripts do not gain a new delay.
 *  - A large function becomes eligible after the extended warm-up.
 *
 * Thread-safety:
 *  - The tested policy is pure.
 *
 * Performance notes:
 *  - This checks the policy, not generated code or page performance.
 */
#include "jit/BaselineLargeScriptPolicy.h"

using js::jit::DeferLargeBaselineAtWarmUp;

static_assert(!DeferLargeBaselineAtWarmUp(false, 1000001, 1), "top-level");
static_assert(!DeferLargeBaselineAtWarmUp(true, 1000000, 1), "size boundary");
static_assert(DeferLargeBaselineAtWarmUp(true, 1000001, 1), "large function");
static_assert(DeferLargeBaselineAtWarmUp(true, 1000001, 10000), "warm-up boundary");
static_assert(!DeferLargeBaselineAtWarmUp(true, 1000001, 10001), "becomes eligible");

int main() { return 0; }
