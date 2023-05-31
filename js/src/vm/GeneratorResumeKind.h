/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef vm_GeneratorResumeKind_h
#define vm_GeneratorResumeKind_h

#include <stdint.h>  // uint8_t

namespace js {

enum class GeneratorResumeKind : uint8_t { Next, Throw, Return };

}  // namespace js

#endif /* vm_GeneratorResumeKind_h */
