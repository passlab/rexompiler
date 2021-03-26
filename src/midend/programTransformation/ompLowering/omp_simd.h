#pragma once

enum SimdType {
    Nothing,
    Addr3,
    Intel_AVX512,
    Arm_SVE2
};

enum OpType {
    None,
    Load,
    Broadcast,
    BroadcastZero,
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

void omp_simd_write_intel(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block);
void omp_simd_write_arm(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block);

