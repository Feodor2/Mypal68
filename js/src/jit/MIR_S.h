// Wasm SIMD.
//
// See comment in WasmIonCompile.cpp for a justification for these nodes.

// (v128, v128, v128) -> v128 effect-free operation.
class MWasmTernarySimd128 : public MTernaryInstruction,
                            public NoTypePolicy::Data {
  wasm::SimdOp simdOp_;

  MWasmTernarySimd128(MDefinition* v0, MDefinition* v1, MDefinition* v2,
                      wasm::SimdOp simdOp)
      : MTernaryInstruction(classOpcode, v0, v1, v2), simdOp_(simdOp) {
    setMovable();
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmTernarySimd128)
  TRIVIAL_NEW_WRAPPERS
  NAMED_OPERANDS((0, v0), (1, v1), (2, v2))

  AliasSet getAliasSet() const override { return AliasSet::None(); }
  bool congruentTo(const MDefinition* ins) const override {
    return congruentIfOperandsEqual(ins);
  }
  MDefinition* foldsTo(TempAllocator& alloc) override;

  // If the control mask of a bitselect allows the operation to be specialized
  // as a shuffle and it is profitable to specialize it on this platform, return
  // true and the appropriate shuffle mask.
  bool specializeBitselectConstantMaskAsShuffle(int8_t shuffle[16]);
  // Checks if more relaxed version of lane select can be used. It returns true
  // if a bit mask input expected to be all 0s or 1s for entire 8-bit lanes,
  // false otherwise.
  bool canRelaxBitselect();

  wasm::SimdOp simdOp() const { return simdOp_; }

  ALLOW_CLONE(MWasmTernarySimd128)
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

  // Checks if pmaddubsw operation is supported.
  bool canPmaddubsw();

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

class MWasmLoadLaneSimd128
    : public MVariadicInstruction,  // memoryBase is nullptr on some platforms
      public NoTypePolicy::Data {
  wasm::MemoryAccessDesc access_;
  uint32_t laneSize_;
  uint32_t laneIndex_;
  uint32_t memoryBaseIndex_;

  MWasmLoadLaneSimd128(const wasm::MemoryAccessDesc& access, uint32_t laneSize,
                       uint32_t laneIndex, uint32_t memoryBaseIndex)
      : MVariadicInstruction(classOpcode),
        access_(access),
        laneSize_(laneSize),
        laneIndex_(laneIndex),
        memoryBaseIndex_(memoryBaseIndex) {
    MOZ_ASSERT(!access_.isAtomic());
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmLoadLaneSimd128)
  NAMED_OPERANDS((0, base), (1, value));

  static MWasmLoadLaneSimd128* New(TempAllocator& alloc,
                                   MDefinition* memoryBase, MDefinition* base,
                                   const wasm::MemoryAccessDesc& access,
                                   uint32_t laneSize, uint32_t laneIndex,
                                   MDefinition* value) {
    uint32_t nextIndex = 2;
    uint32_t memoryBaseIndex = memoryBase ? nextIndex++ : UINT32_MAX;

    MWasmLoadLaneSimd128* load = new (alloc)
        MWasmLoadLaneSimd128(access, laneSize, laneIndex, memoryBaseIndex);
    if (!load->init(alloc, nextIndex)) {
      return nullptr;
    }

    load->initOperand(0, base);
    load->initOperand(1, value);
    if (memoryBase) {
      load->initOperand(memoryBaseIndex, memoryBase);
    }

    return load;
  }

  const wasm::MemoryAccessDesc& access() const { return access_; }
  uint32_t laneSize() const { return laneSize_; }
  uint32_t laneIndex() const { return laneIndex_; }
  bool hasMemoryBase() const { return memoryBaseIndex_ != UINT32_MAX; }
  MDefinition* memoryBase() const {
    MOZ_ASSERT(hasMemoryBase());
    return getOperand(memoryBaseIndex_);
  }

  AliasSet getAliasSet() const override {
    return AliasSet::Load(AliasSet::WasmHeap);
  }
};

class MWasmStoreLaneSimd128 : public MVariadicInstruction,
                              public NoTypePolicy::Data {
  wasm::MemoryAccessDesc access_;
  uint32_t laneSize_;
  uint32_t laneIndex_;
  uint32_t memoryBaseIndex_;

  explicit MWasmStoreLaneSimd128(const wasm::MemoryAccessDesc& access,
                                 uint32_t laneSize, uint32_t laneIndex,
                                 uint32_t memoryBaseIndex)
      : MVariadicInstruction(classOpcode),
        access_(access),
        laneSize_(laneSize),
        laneIndex_(laneIndex),
        memoryBaseIndex_(memoryBaseIndex) {
    MOZ_ASSERT(!access_.isAtomic());
    setResultType(MIRType::Simd128);
  }

 public:
  INSTRUCTION_HEADER(WasmStoreLaneSimd128)
  NAMED_OPERANDS((0, base), (1, value))

  static MWasmStoreLaneSimd128* New(TempAllocator& alloc,
                                    MDefinition* memoryBase, MDefinition* base,
                                    const wasm::MemoryAccessDesc& access,
                                    uint32_t laneSize, uint32_t laneIndex,
                                    MDefinition* value) {
    uint32_t nextIndex = 2;
    uint32_t memoryBaseIndex = memoryBase ? nextIndex++ : UINT32_MAX;

    MWasmStoreLaneSimd128* store = new (alloc)
        MWasmStoreLaneSimd128(access, laneSize, laneIndex, memoryBaseIndex);
    if (!store->init(alloc, nextIndex)) {
      return nullptr;
    }

    store->initOperand(0, base);
    store->initOperand(1, value);
    if (memoryBase) {
      store->initOperand(memoryBaseIndex, memoryBase);
    }

    return store;
  }

  const wasm::MemoryAccessDesc& access() const { return access_; }
  uint32_t laneSize() const { return laneSize_; }
  uint32_t laneIndex() const { return laneIndex_; }
  bool hasMemoryBase() const { return memoryBaseIndex_ != UINT32_MAX; }
  MDefinition* memoryBase() const {
    MOZ_ASSERT(hasMemoryBase());
    return getOperand(memoryBaseIndex_);
  }

  AliasSet getAliasSet() const override {
    return AliasSet::Store(AliasSet::WasmHeap);
  }
};

// End Wasm SIMD
