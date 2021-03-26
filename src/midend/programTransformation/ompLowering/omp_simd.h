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

