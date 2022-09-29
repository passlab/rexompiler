#pragma once

enum SimdType {
    Nothing,
    Addr3,
    ArmAddr3,
    Intel_AVX512,
    Arm_SVE2
};

enum OpType {
    None,
    Load,
    Broadcast,
    BroadcastZero,
    Gather,
    ExplicitGather,
    Scatter,
    ScalarStore,
    Store,
    HAdd,
    Add,
    Sub,
    Mul,
    Div,
    Extract
};

//
// A class to hold SIMD operations
// The goal is to eventually migrate the SIMD compiler to this
//
struct OmpSimdCompiler {

    // Functions
    bool isStridedLoadStore(SgExpression *pntr_exp);

    // Metadata variables
    SgForStatement *for_loop = nullptr;
};

//
// Final translation functions
//

extern SimdType simd_arch;

void omp_simd_write_intel(SgUpirLoopParallelStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block, int simd_length);
void omp_simd_write_arm(SgUpirLoopParallelStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block);

