// Wasm SIMD.
//
// See comment in WasmIonCompile.cpp for a justification for these nodes.
// (v128, v128, v128) -> v128 effect-free operation.
class MWasmBitselectSimd128 : public MTernaryInstruction,
                              public NoTypePolicy::Data {
  MWasmBitselectSimd128(MDefinition* lhs, MDefinition* rhs,
                        MDefinition* control)
      : MTernaryInstruction(classOpcode, lhs, rhs, control) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmBitselectSimd128)
  TRIVIAL_NEW_WRAPPERS
  NAMED_OPERANDS((0, lhs), (1, rhs), (2, control))

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return congruentIfOperandsEqual(ins);
  }

  ALLOW_CLONE(MWasmBitselectSimd128)
};

// (v128, v128) -> v128 effect-free operations.
class MWasmBinarySimd128 : public MBinaryInstruction,
                           public NoTypePolicy::Data {
  wasm::SimdOp simdOp_;

  MWasmBinarySimd128(MDefinition* lhs, MDefinition* rhs, bool commutative,
                     wasm::SimdOp simdOp)
      : MBinaryInstruction(classOpcode, lhs, rhs), simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
    if (commutative) {
      setCommutative();
    }
  }

 public:
  INSTRUCTION_HEADER(WasmBinarySimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmBinarySimd128()->simdOp() == simdOp_ &&
           congruentIfOperandsEqual(ins);
  }
  MDefinition* foldsTo(TempAllocator& alloc) override;

  wasm::SimdOp simdOp() const { return simdOp_; }

  // Platform-dependent specialization.
  bool specializeForConstantRhs();

  ALLOW_CLONE(MWasmBinarySimd128)
};

// (v128, const) -> v128 effect-free operations.
class MWasmBinarySimd128WithConstant : public MUnaryInstruction,
                                       public NoTypePolicy::Data {
  SimdConstant rhs_;
  wasm::SimdOp simdOp_;

  MWasmBinarySimd128WithConstant(MDefinition* lhs, const SimdConstant& rhs,
                                 wasm::SimdOp simdOp)
      : MUnaryInstruction(classOpcode, lhs), rhs_(rhs), simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmBinarySimd128WithConstant)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmBinarySimd128WithConstant()->simdOp() == simdOp_ &&
           congruentIfOperandsEqual(ins) &&
           rhs_.bitwiseEqual(ins->toWasmBinarySimd128WithConstant()->rhs());
  }

  wasm::SimdOp simdOp() const { return simdOp_; }
  MDefinition* lhs() const { return input(); }
  const SimdConstant& rhs() const { return rhs_; }

  ALLOW_CLONE(MWasmBinarySimd128WithConstant)
};

// (v128, i32) -> v128 effect-free shift operations.
class MWasmShiftSimd128 : public MBinaryInstruction, public NoTypePolicy::Data {
  wasm::SimdOp simdOp_;

  MWasmShiftSimd128(MDefinition* lhs, MDefinition* rhs, wasm::SimdOp simdOp)
      : MBinaryInstruction(classOpcode, lhs, rhs), simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmShiftSimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmShiftSimd128()->simdOp() == simdOp_ &&
           congruentIfOperandsEqual(ins);
  }

  wasm::SimdOp simdOp() const { return simdOp_; }

  ALLOW_CLONE(MWasmShiftSimd128)
};

// (v128, v128, mask) -> v128 effect-free operation.
class MWasmShuffleSimd128 : public MBinaryInstruction,
                            public NoTypePolicy::Data {
  SimdConstant control_;

  MWasmShuffleSimd128(MDefinition* lhs, MDefinition* rhs, SimdConstant control)
      : MBinaryInstruction(classOpcode, lhs, rhs), control_(control) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmShuffleSimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmShuffleSimd128()->control().bitwiseEqual(control_) &&
           congruentIfOperandsEqual(ins);
  }

  SimdConstant control() const { return control_; }

  ALLOW_CLONE(MWasmShuffleSimd128)
};

// (v128, scalar, imm) -> v128 effect-free operations.
class MWasmReplaceLaneSimd128 : public MBinaryInstruction,
                                public NoTypePolicy::Data {
  uint32_t laneIndex_;
  wasm::SimdOp simdOp_;

  MWasmReplaceLaneSimd128(MDefinition* lhs, MDefinition* rhs,
                          uint32_t laneIndex, wasm::SimdOp simdOp)
      : MBinaryInstruction(classOpcode, lhs, rhs),
        laneIndex_(laneIndex),
        simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmReplaceLaneSimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmReplaceLaneSimd128()->simdOp() == simdOp_ &&
           ins->toWasmReplaceLaneSimd128()->laneIndex() == laneIndex_ &&
           congruentIfOperandsEqual(ins);
  }

  uint32_t laneIndex() const { return laneIndex_; }
  wasm::SimdOp simdOp() const { return simdOp_; }

  ALLOW_CLONE(MWasmReplaceLaneSimd128)
};

// (v128) -> v128 effect-free operations.
class MWasmUnarySimd128 : public MUnaryInstruction, public NoTypePolicy::Data {
  wasm::SimdOp simdOp_;

  MWasmUnarySimd128(MDefinition* src, wasm::SimdOp simdOp)
      : MUnaryInstruction(classOpcode, src), simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmUnarySimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmUnarySimd128()->simdOp() == simdOp_ &&
           congruentIfOperandsEqual(ins);
  }

  wasm::SimdOp simdOp() const { return simdOp_; }

  ALLOW_CLONE(MWasmUnarySimd128)
};

// (scalar) -> v128 effect-free operations.
class MWasmScalarToSimd128 : public MUnaryInstruction,
                             public NoTypePolicy::Data {
  wasm::SimdOp simdOp_;

  MWasmScalarToSimd128(MDefinition* src, wasm::SimdOp simdOp)
      : MUnaryInstruction(classOpcode, src), simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmScalarToSimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmScalarToSimd128()->simdOp() == simdOp_ &&
           congruentIfOperandsEqual(ins);
  }
  MDefinition* foldsTo(TempAllocator& alloc) override;

  wasm::SimdOp simdOp() const { return simdOp_; }

  ALLOW_CLONE(MWasmScalarToSimd128)
};

// (v128, imm) -> scalar effect-free operations.
class MWasmReduceSimd128 : public MUnaryInstruction, public NoTypePolicy::Data {
  wasm::SimdOp simdOp_;
  uint32_t imm_;

  MWasmReduceSimd128(MDefinition* src, wasm::SimdOp simdOp, MIRType outType,
                     uint32_t imm)
      : MUnaryInstruction(classOpcode, src), simdOp_(simdOp), imm_(imm) {
    setMovable();
    setResultType(outType);
  }

 public:
  INSTRUCTION_HEADER(WasmReduceSimd128)
  TRIVIAL_NEW_WRAPPERS

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return ins->toWasmReduceSimd128()->simdOp() == simdOp_ &&
           ins->toWasmReduceSimd128()->imm() == imm_ &&
           congruentIfOperandsEqual(ins);
  }
  MDefinition* foldsTo(TempAllocator& alloc) override;

  uint32_t imm() const { return imm_; }
  wasm::SimdOp simdOp() const { return simdOp_; }

  ALLOW_CLONE(MWasmReduceSimd128)
};
// End Wasm SIMD
