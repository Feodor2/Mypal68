/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef frontend_AsyncEmitter_h
#define frontend_AsyncEmitter_h

#include "mozilla/Attributes.h"  // MOZ_STACK_CLASS

#include "frontend/TryEmitter.h"  // TryEmitter

namespace js {
namespace frontend {

struct BytecodeEmitter;

// Class for emitting Bytecode associated with the AsyncFunctionGenerator.
//
// Usage:
//
//   For an async function, the params have to be handled separately,
//   because the body may have pushed an additional var environment, changing
//   the number of hops required to reach the |.generator| variable. In order
//   to handle this, we can't reuse the same TryCatch emitter.
//
//   Simple case - For a function without expression parameters:
//   `async function f(<params>) {<body>}`,
//     AsyncEmitter ae(this);
//
//     ae.prepareForParamsWithoutExpression();
//     // Emit Params.
//     ...
//     ae.paramsEpilogue(); // We need to emit the epilogue before the extra
//     VarScope emitExtraBodyVarScope();
//
//     // Emit new scope
//     ae.prepareForBody();
//
//     // Emit body of the Function.
//
//     ae.emitEnd();
//
//   Complex case - For a function with expression parameters:
//   `async function f(<expression>) {<body>}`,
//     AsyncEmitter ae(this);
//
//     ae.prepareForParamsWithExpression();
//
//     // Emit Params.
//     ...
//     ae.paramsEpilogue(); // We need to emit the epilogue before the extra
//                          // VarScope
//     emitExtraBodyVarScope();
//
//     // Emit new scope
//     ae.prepareForBody();
//
//     // Emit body of the Function.
//     ...
//     ae.emitEnd();
//
//
//   Async Module case - For a module with `await` in the top level:
//     AsyncEmitter ae(this);
//     ae.prepareForModule(); // prepareForModule is used to setup the generator
//                            // for the async module.
//     switchToMain();
//     ...
//
//     // Emit new scope
//     ae.prepareForBody();
//
//     // Emit body of the Script.
//
//     ae.emitEnd();
//

class MOZ_STACK_CLASS AsyncEmitter {
 private:
  BytecodeEmitter* bce_;

  // try-catch block for async function parameter and body.
  mozilla::Maybe<TryEmitter> rejectTryCatch_;

#ifdef DEBUG
  // The state of this emitter.
  //
  //    +-------+
  //    | Start |-+
  //    +-------+ |
  //              |
  //   +----------+
  //   |
  //   | [Parameters with Expression]
  //   |   prepareForParamsWithExpression    +------------+
  //   +-------------------------------------| Parameters |-->+
  //   |                                     +------------+   |
  //   |                                                      |
  //   | [Parameters Without Expression]                      |
  //   |   prepareForParamsWithoutExpression +------------+   |
  //   +-------------------------------------| Parameters |-->+
  //   |                                     +------------+   |
  //   | [Modules]                                            |
  //   |   prepareForModule  +----------------+               |
  //   +-------------------->| ModulePrologue |--+            |
  //                         +----------------+  |            |
  //                                             |            |
  //                                             |            |
  //   +-----------------------------------------+            |
  //   |                                                      |
  //   |                                                      |
  //   V                     +------------+  paramsEpilogue   |
  //   +<--------------------| PostParams |<------------------+
  //   |                     +------------+
  //   |
  //   | [Script body]
  //   |   prepareForBody    +---------+
  //   +-------------------->|  Body   |--------+
  //                         +---------+        |  <emit script body>
  //   +----------------------------------------+
  //   |
  //   |  emitEnd             +-----+
  //   +--------------------->| End |
  //                          +-----+

  enum class State {
    // The initial state.
    Start,

    Parameters,

    ModulePrologue,

    PostParams,

    Body,

    End,
  };

  State state_ = State::Start;
#endif

  [[nodiscard]] bool emitRejectCatch();
  [[nodiscard]] bool emitFinalYield();

 public:
  explicit AsyncEmitter(BytecodeEmitter* bce) : bce_(bce){};

  [[nodiscard]] bool prepareForParamsWithoutExpression();
  [[nodiscard]] bool prepareForParamsWithExpression();
  [[nodiscard]] bool prepareForModule();
  [[nodiscard]] bool emitParamsEpilogue();
  [[nodiscard]] bool prepareForBody();
  [[nodiscard]] bool emitEnd();
};

} /* namespace frontend */
} /* namespace js */

#endif /* frontend_AsyncEmitter_h */
