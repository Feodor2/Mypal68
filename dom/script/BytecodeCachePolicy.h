/**
 * @file BytecodeCachePolicy.h
 * @brief Bytecode cache policy for large scripts.
 *
 * Responsibilities:
 *  - Identify large scripts eligible for the hybrid eager cache path.
 *
 * Invariants:
 *  - The threshold uses the loader's existing source length units.
 *
 * Thread-safety:
 *  - The policy is pure and has no mutable state.
 *
 * Performance notes:
 *  - The predicate is evaluated only while considering cache encoding.
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_BytecodeCachePolicy_h
#define mozilla_dom_BytecodeCachePolicy_h

#include <cstddef>
#include <cstdint>

namespace mozilla::dom {

constexpr std::size_t kEagerBytecodeSourceLength = 4 * 1024 * 1024;

[[nodiscard]] constexpr bool SkipHybridBytecodeFetchThreshold(
    std::int32_t strategy, std::size_t sourceLength) noexcept {
  return strategy == -5 && sourceLength >= kEagerBytecodeSourceLength;
}

}  // namespace mozilla::dom

#endif  // mozilla_dom_BytecodeCachePolicy_h
