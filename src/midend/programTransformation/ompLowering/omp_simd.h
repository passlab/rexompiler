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

public:
    explicit OmpSimdCompiler(bool isArm);

    // Functions
    bool omp_simd_pass1();
    void omp_simd_pass2();
    
    void omp_simd_build_3addr(SgExpression *rval, SgType *type);
    char omp_simd_get_reduction_mod(SgVarRefExp *var);
    void omp_simd_build_math(VariantT op_type, SgType *type);
    void omp_simd_build_scalar_assign(SgExpression *node, SgType *type);
    void omp_simd_build_ptr_assign(SgExpression *pntr_exp, SgType *type);
    SgPntrArrRefExp *omp_simd_convert_ptr(SgExpression *pntr_exp);
    bool isStridedLoadStore(SgExpression *pntr_exp);
    
    bool omp_simd_is_load_operand(VariantT val);
    int omp_simd_get_simdlen(bool safelen);
    int omp_simd_get_length();
    
    // Helper functions
    SgBasicBlock *getBlock();
    SgUpirLoopParallelStatement *getTarget();
    SgForStatement *getForLoop();
    Rose_STL_Container<SgNode *> *getIR();
    bool isArm();
    void setTarget(SgUpirLoopParallelStatement *target);
    void setForLoop(SgForStatement *for_loop);
    void addIR(SgNode *ir);
    
private:
    SgBasicBlock *new_block;
    SgUpirLoopParallelStatement *target = nullptr;
    SgForStatement *for_loop = nullptr;
    Rose_STL_Container<SgNode *> *ir_block;
    bool arm = false;
    
    std::stack<std::string> nameStack;
};

//
// Final translation functions
//

extern SimdType simd_arch;

void omp_simd_write_intel(SgUpirLoopParallelStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block, int simd_length);
void omp_simd_write_arm(SgUpirLoopParallelStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block);

