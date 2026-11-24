/**
 * @file modrm_assembler_test.cpp
 * @brief Validate real x86 JIT memory-operand encoding against reference bytes.
 *
 * Responsibilities:
 *  - Exercise the production assembler for boundary offsets and all registers.
 * Invariants:
 *  - Expected bytes follow the x86 ModRM, SIB and displacement encoding.
 * Thread-safety:
 *  - The test owns one assembler on a single thread.
 * Performance notes:
 *  - This correctness test is not a performance measurement.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "jit/x86-shared/BaseAssembler-x86-shared.h"

using js::jit::X86Encoding::BaseAssembler;
using js::jit::X86Encoding::RegisterID;

static size_t ReferenceBytes(uint8_t* bytes, int32_t offset, uint32_t base,
                             uint32_t destination) {
  const bool noDisp = offset == 0 && base != 5;
  const bool disp8 = !noDisp && offset >= -128 && offset <= 127;
  const uint32_t mode = noDisp ? 0 : (disp8 ? 1 : 2);
  size_t count = 0;
  bytes[count++] = 0x8b;
  bytes[count++] = static_cast<uint8_t>((mode << 6) | (destination << 3) | base);
  if (base == 4) {
    bytes[count++] = 0x24;
  }
  if (disp8) {
    bytes[count++] = static_cast<uint8_t>(offset);
  } else if (!noDisp) {
    const uint32_t value = static_cast<uint32_t>(offset);
    for (uint32_t shift = 0; shift < 32; shift += 8) {
      bytes[count++] = static_cast<uint8_t>(value >> shift);
    }
  }
  return count;
}

static bool CheckOperand(BaseAssembler& assembler, int32_t offset, uint32_t base,
                         uint32_t destination) {
  uint8_t expected[8] = {};
  const size_t expectedSize = ReferenceBytes(expected, offset, base, destination);
  const size_t before = assembler.size();
  assembler.movl_mr(offset, static_cast<RegisterID>(base),
                    static_cast<RegisterID>(destination));
  const size_t actualSize = assembler.size() - before;
  if (assembler.oom() || actualSize != expectedSize ||
      memcmp(assembler.buffer() + before, expected, expectedSize) != 0) {
    fprintf(stderr, "Mismatch: base=%u destination=%u offset=%ld\n", base,
            destination, static_cast<long>(offset));
    return false;
  }
  return true;
}

int main() {
  const int32_t wideOffsets[] = {INT32_MIN, -65536, 65536, INT32_MAX};
  BaseAssembler assembler;
  if (!assembler.reserve(262144)) {
    return 1;
  }

  uint32_t cases = 0;
  for (uint32_t base = 0; base < 8; ++base) {
    for (uint32_t destination = 0; destination < 8; ++destination) {
      for (int32_t offset = -129; offset <= 128; ++offset) {
        if (!CheckOperand(assembler, offset, base, destination)) {
          return 2;
        }
        ++cases;
      }
      for (int32_t offset : wideOffsets) {
        if (!CheckOperand(assembler, offset, base, destination)) {
          return 2;
        }
        ++cases;
      }
    }
  }

  printf("MODRM_ASSEMBLER_TEST_OK cases=%u\n", cases);
  return 0;
}
