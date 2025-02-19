/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_IonCompileTask_h
#define jit_IonCompileTask_h

#include "mozilla/LinkedList.h"

#include "jit/MIRGenerator.h"

#include "js/Utility.h"
#include "vm/HelperThreadTask.h"

namespace js {
namespace jit {

class CodeGenerator;
class WarpSnapshot;

// IonCompileTask represents a single off-thread Ion compilation task.
class IonCompileTask final : public HelperThreadTask,
                             public mozilla::LinkedListElement<IonCompileTask> {
  MIRGenerator& mirGen_;

  // If off thread compilation is successful, the final code generator is
  // attached here. Code has been generated, but not linked (there is not yet
  // an IonScript). This is heap allocated, and must be explicitly destroyed,
  // performed by FinishOffThreadTask().
  CodeGenerator* backgroundCodegen_ = nullptr;

  WarpSnapshot* snapshot_ = nullptr;

 public:
  explicit IonCompileTask(MIRGenerator& mirGen, WarpSnapshot* snapshot);

  JSScript* script() { return mirGen_.outerInfo().script(); }
  MIRGenerator& mirGen() { return mirGen_; }
  TempAllocator& alloc() { return mirGen_.alloc(); }
  WarpSnapshot* snapshot() { return snapshot_; }

  size_t sizeOfExcludingThis(mozilla::MallocSizeOf mallocSizeOf);
  void trace(JSTracer* trc);

  CodeGenerator* backgroundCodegen() const { return backgroundCodegen_; }
  void setBackgroundCodegen(CodeGenerator* codegen) {
    backgroundCodegen_ = codegen;
  }

  ThreadType threadType() override { return THREAD_TYPE_ION; }
  void runTask();
  void runHelperThreadTask(AutoLockHelperThreadState& locked) override;
};

class IonFreeTask : public HelperThreadTask {
 public:
  explicit IonFreeTask(IonCompileTask* task) : task_(task) {}
  IonCompileTask* compileTask() { return task_; }

  ThreadType threadType() override { return THREAD_TYPE_ION_FREE; }
  void runHelperThreadTask(AutoLockHelperThreadState& locked) override;

 private:
  IonCompileTask* task_;
};

void AttachFinishedCompilations(JSContext* cx);
void FinishOffThreadTask(JSRuntime* runtime, IonCompileTask* task,
                         const AutoLockHelperThreadState& lock);
void FreeIonCompileTask(IonCompileTask* task);

}  // namespace jit
}  // namespace js

#endif /* jit_IonCompileTask_h */
