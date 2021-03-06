/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IPCMessageUtils.h"
#include "mozilla/CheckedInt.h"

namespace IPC {

bool ByteLengthIsValid(uint32_t aNumElements, size_t aElementSize,
                       int* aByteLength) {
  auto length = mozilla::CheckedInt<int>(aNumElements) * aElementSize;
  if (!length.isValid()) {
    return false;
  }
  *aByteLength = length.value();
  return true;
}

}  // namespace IPC
