/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <type_traits>
#include "mozilla/dom/quota/ForwardDecls.h"

using namespace mozilla;

static_assert(std::is_same_v<OkOrErr, Result<Ok, QMResult>>);
