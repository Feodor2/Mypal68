/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_FoldLinearArithConstants_h
#define jit_FoldLinearArithConstants_h

namespace js {
namespace jit {

class MIRGenerator;
class MIRGraph;

[[nodiscard]] bool FoldLinearArithConstants(MIRGenerator* mir, MIRGraph& graph);

} /* namespace jit */
} /* namespace js */

#endif /* jit_FoldLinearArithConstants_h */
