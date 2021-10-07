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

extern SimdType simd_arch;

void omp_simd_write_intel(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block, int simd_length);
void omp_simd_write_arm(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block);

