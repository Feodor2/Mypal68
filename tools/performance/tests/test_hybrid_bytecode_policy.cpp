/**
 * @file test_hybrid_bytecode_policy.cpp
 * @brief Check the experimental bytecode fetch threshold boundaries.
 *
 * Responsibilities:
 *  - Verify the policy for small, boundary, large, and other strategies.
 *
 * Invariants:
 *  - Only strategy -5 may bypass the fetch threshold using this predicate.
 *
 * Thread-safety:
 *  - The test has no shared mutable state.
 *
 * Performance notes:
 *  - Checks are evaluated at compile time.
 */

#include "dom/script/BytecodeCachePolicy.h"

using mozilla::dom::kEagerBytecodeSourceLength;
using mozilla::dom::SkipHybridBytecodeFetchThreshold;

static_assert(!SkipHybridBytecodeFetchThreshold(-5, 0));
static_assert(!SkipHybridBytecodeFetchThreshold(-5, 1024));
static_assert(!SkipHybridBytecodeFetchThreshold(-5, kEagerBytecodeSourceLength - 1));
static_assert(SkipHybridBytecodeFetchThreshold(-5, kEagerBytecodeSourceLength));
static_assert(SkipHybridBytecodeFetchThreshold(-5, kEagerBytecodeSourceLength + 1));
static_assert(!SkipHybridBytecodeFetchThreshold(0, kEagerBytecodeSourceLength));
static_assert(!SkipHybridBytecodeFetchThreshold(-3, kEagerBytecodeSourceLength));
static_assert(!SkipHybridBytecodeFetchThreshold(-4, kEagerBytecodeSourceLength));

int main()
{
  return 0;
}
