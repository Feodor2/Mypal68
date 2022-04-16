// Copyright (c) 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/rand_util.h"

#include <math.h>

#include <limits>

#include "base/basictypes.h"
#include "base/logging.h"
#include "mozilla/RandomNum.h"

namespace base {

int RandInt(int min, int max) {
  DCHECK(min <= max);

  uint64_t range = static_cast<int64_t>(max) - min + 1;
  mozilla::Maybe<uint64_t> number = mozilla::RandomUint64();
  MOZ_RELEASE_ASSERT(number.isSome());
  int result = min + static_cast<int>(number.value() % range);
  DCHECK(result >= min && result <= max);
  return result;
}

}  // namespace base
