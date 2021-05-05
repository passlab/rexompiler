
#include <iostream>
#include <stack>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "omp_lowering.h"
#include "omp_simd.h"

using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;

////////////////////////////////////////////////////////////////////////////////////
// The final conversion step- Convert to Intel intrinsics

int simd_len = 16;
int loop_increment = 16;

// For generating names
int buf_pos = 0;
int mask_pos = 0;

std::string intel_gen_buf() {
    char str[5];
    sprintf(str, "%d", buf_pos);
    
    std::string name = "__buf" + std::string(str);
    ++buf_pos;
    return name;
}

std::string intel_gen_mask() {
    char str[5];
    sprintf(str, "%d", mask_pos);
    
    std::string name = "__mask" + std::string(str);
    ++mask_pos;
    return name;
}

// If half_type == true, we return the 256-bit version
// This is needed for scalar stores
SgType *intel_simd_type(SgType *type, SgScopeStatement *new_block, bool half_type = false) {
    SgType *vector_type;
    
    int len = simd_len;
    if (half_type) len /= 2;
    
    if (len == 16) {
        switch (type->variantT()) {
            case V_SgTypeInt: vector_type = buildOpaqueType("__m512i", new_block); break;
            case V_SgTypeFloat: vector_type = buildOpaqueType("__m512", new_block); break;
            case V_SgTypeDouble: vector_type = buildOpaqueType("__m512d", new_block); break;
            default: vector_type = type;
        }
    } else {
        switch (type->variantT()) {
            case V_SgTypeInt: vector_type = buildOpaqueType("__m256i", new_block); break;
            case V_SgTypeFloat: vector_type = buildOpaqueType("__m256", new_block); break;
            case V_SgTypeDouble: vector_type = buildOpaqueType("__m256d", new_block); break;
            default: vector_type = type;
        }
    }

    return vector_type;
}

std::string intel_simd_func(OpType op_type, SgType *type, bool half_type = false) {
    int len = simd_len;
    if (half_type) len /= 2;
    
    std::string instr = "_mm256_";
    if (len == 16) instr = "_mm512_";

    switch (op_type) {
        case Load: instr += "loadu_"; break;
        case Broadcast: instr += "set1_"; break;
        case BroadcastZero: instr += "setzero_"; break;
        case Gather: instr += "mask_i32gather_"; break;
        
        case ScalarStore:
        case Store: instr += "storeu_"; break;
        case Scatter: instr += "i32scatter_"; break;
    
        case HAdd: instr += "hadd_"; break;
        case Add: instr += "add_"; break;
        case Sub: instr += "sub_"; break;
        case Div: instr += "div_"; break;
        
        case Mul: {
            if (type->variantT() == V_SgTypeInt) instr += "mullo_";
            else instr += "mul_";
        } break;
        
        case Extract: {
            switch (type->variantT()) {
                case V_SgTypeInt: instr += "extracti32x8_"; break;
                case V_SgTypeFloat: instr += "extractf32x8_"; break;
                case V_SgTypeDouble: instr += "extractf64x4_"; break;
                default: {}
            }
        } break;
        
        default: {}
    }
    
    switch (type->variantT()) {
        case V_SgTypeInt: {
            if (op_type == BroadcastZero && len == 8) {
                instr += "si256";
            } else if (op_type == Load || op_type == Store || op_type == ScalarStore) {
                if (len == 16) instr += "si512";
                else instr += "si256";
            } else {
                instr += "epi32";
            }
        } break;
        
        case V_SgTypeFloat: instr += "ps"; break;
        case V_SgTypeDouble: instr += "pd"; break;
        
        default: {}
    }
    
    return instr;
}

// Generates a SIMD load statement for Intel
SgAssignInitializer *intel_write_load(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
        
    SgVarRefExp *va = static_cast<SgVarRefExp *>(lval);
    SgType *vector_type = intel_simd_type(va->get_type(), target->get_scope());
    SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(rval);
    
    // Check the loop increment
    // TODO: Is this the best location for this?
    if (va->get_type()->variantT() == V_SgTypeDouble) {
        loop_increment = simd_len / 2;
    }
    
    // Build function call parameters
    SgExprListExp *parameters;
    
    if (va->get_type()->variantT() == V_SgTypeInt) {
        SgAddressOfOp *addr = buildAddressOfOp(array);
        SgPointerType *ptr_type = buildPointerType(vector_type);
        SgCastExp *cast = buildCastExp(addr, ptr_type);
        parameters = buildExprListExp(cast);
    } else {
        SgAddressOfOp *addr = buildAddressOfOp(array);
        parameters = buildExprListExp(addr);
    }

    // Build the function call
    std::string func_name = intel_simd_func(Load, va->get_type());
    
    SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
    return buildAssignInitializer(ld);
}

// ==============================================================================================================
// Generates a SIMD broadcast statement on Intel architecture
//
SgAssignInitializer *intel_write_broadcast(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
    
    SgVarRefExp *v_dest = static_cast<SgVarRefExp *>(lval);
    SgVarRefExp *v_src = static_cast<SgVarRefExp *>(rval);
    SgType *vector_type = intel_simd_type(v_dest->get_type(), target->get_scope());
    
    // Function call parameters
    SgExprListExp *parameters = buildExprListExp(v_src);
    
    // Build the function call and place it above the for loop
    std::string func_name = intel_simd_func(Broadcast, v_dest->get_type());
    
    SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
    return buildAssignInitializer(ld);
}

// ===========================================================================================================
// Generates a SIMD gather statement
//
SgAssignInitializer *intel_write_gather(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
    
    SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
    SgPntrArrRefExp *element = static_cast<SgPntrArrRefExp *>(rval);
    SgPntrArrRefExp *mask_pntr = static_cast<SgPntrArrRefExp *>(element->get_rhs_operand());
    
    // Load the array indexes first
    std::string mask_name = intel_gen_mask();
    SgType *mask_type = intel_simd_type(mask_pntr->get_type(), target->get_scope());
    SgType *vector_type = intel_simd_type(dest->get_type(), target->get_scope());
    
    SgAddressOfOp *addr = buildAddressOfOp(mask_pntr);
    SgPointerType *ptr_type = buildPointerType(mask_type);
    SgCastExp *cast = buildCastExp(addr, ptr_type);
    
    std::string func_name = intel_simd_func(Load, mask_pntr->get_type());
    SgExprListExp *parameters = buildExprListExp(cast);
    SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
    SgAssignInitializer *local_init = buildAssignInitializer(ld);
    
    SgVariableDeclaration *mask_vd = buildVariableDeclaration(mask_name, mask_type, local_init, new_block);
    appendStatement(mask_vd, new_block);
    
    // Generate the two mask statements
    std::string mask1 = intel_gen_mask();
    std::string mask2 = intel_gen_mask();
    std::string kmask = intel_gen_mask();
    
    SgType *kmask_type;
    if (simd_len == 16) kmask_type = buildOpaqueType("__mmask16", new_block);
    else kmask_type = buildOpaqueType("__mmask8", new_block);
    
    SgVariableDeclaration *mask1_vd = buildVariableDeclaration(mask1, kmask_type, NULL, new_block);
    SgVariableDeclaration *mask2_vd = buildVariableDeclaration(mask2, kmask_type, NULL, new_block);
    
    appendStatement(mask1_vd, new_block);
    appendStatement(mask2_vd, new_block);
    
    func_name = "_kxnor_mask16";
    if (simd_len == 8) func_name = "_kxnor_mask8";
    
    parameters = buildExprListExp(buildVarRefExp(mask1, new_block), buildVarRefExp(mask2, new_block));
    ld = buildFunctionCallExp(func_name, kmask_type, parameters, new_block);
    local_init = buildAssignInitializer(ld);
    
    SgVariableDeclaration *kmask_vd = buildVariableDeclaration(kmask, kmask_type, local_init, new_block);
    appendStatement(kmask_vd, new_block);
    
    // Create the empty register
    std::string zero_name = intel_gen_buf();
    
    func_name = intel_simd_func(BroadcastZero, dest->get_type());
    ld = buildFunctionCallExp(func_name, vector_type, NULL, new_block);
    local_init = buildAssignInitializer(ld);
    
    SgVariableDeclaration *zero_vd = buildVariableDeclaration(zero_name, vector_type, local_init, new_block);
    appendStatement(zero_vd, new_block);
    
    // Now for the gather statement
    SgVarRefExp *mask_ref = buildVarRefExp(mask_name, new_block);
    SgVarRefExp *base_ref = static_cast<SgVarRefExp *>(element->get_lhs_operand());
    SgVarRefExp *zero_ref = buildVarRefExp(zero_name, new_block);
    SgVarRefExp *kmask_ref = buildVarRefExp(kmask, new_block);
    
    int scale = 4;
    if (dest->get_type()->variantT() == V_SgTypeDouble) {
        loop_increment = simd_len / 2;
        scale = 8;
        
        // If we have a double, we also need to do an extraction of the mask
        SgType *extract_type;
        std::string extract_name = "";
        std::string mask_name2 = mask_name + "2";
        
        if (simd_len == 16) {
            extract_type = buildOpaqueType("__m256i", new_block);
            extract_name = "_mm512_extracti32x8_epi32";
        } else {
            extract_type = buildOpaqueType("__m128i", new_block);
            extract_name = "_mm256_extractf128_si256";
        }
        
        parameters = buildExprListExp(mask_ref, buildIntVal(0));
        ld = buildFunctionCallExp(extract_name, extract_type, parameters, new_block);
        local_init = buildAssignInitializer(ld);
        mask_vd = buildVariableDeclaration(mask_name2, extract_type, local_init, new_block);
        appendStatement(mask_vd, new_block);
        
        mask_ref = buildVarRefExp(mask_name2, new_block);
    }
    
    parameters = buildExprListExp(zero_ref, kmask_ref, mask_ref, base_ref, buildIntVal(scale));
    
    func_name = intel_simd_func(Gather, dest->get_type());
    ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
    return buildAssignInitializer(ld);
}

// ==========================================================================================
// Generates a SIMD store statement
//
void intel_write_store(SgBinaryOp *op, SgBasicBlock *new_block) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
    
    SgVarRefExp *v_src = static_cast<SgVarRefExp *>(rval);
                
    // Function call parameters
    SgAddressOfOp *addr = buildAddressOfOp(lval);
    SgExprListExp *parameters = buildExprListExp(addr, v_src);
    
    // Build the function call
    std::string func_name = intel_simd_func(Store, v_src->get_type());
    
    SgExprStatement *fc = buildFunctionCallStmt(func_name, buildVoidType(), parameters, new_block);
    appendStatement(fc, new_block);
}

// ============================================================================================
/// Generates a SIMD scatter statement
//
void intel_write_scatter(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
    
    SgVarRefExp *dest = static_cast<SgVarRefExp *>(rval);
    SgPntrArrRefExp *element = static_cast<SgPntrArrRefExp *>(lval);
    SgPntrArrRefExp *mask_pntr = static_cast<SgPntrArrRefExp *>(element->get_rhs_operand());
    
    // Load the mask first
    std::string mask_name = intel_gen_mask();
    SgType *mask_type = intel_simd_type(mask_pntr->get_type(), target->get_scope());
    SgType *vector_type = intel_simd_type(dest->get_type(), target->get_scope());
    
    SgAddressOfOp *addr = buildAddressOfOp(mask_pntr);
    SgPointerType *ptr_type = buildPointerType(mask_type);
    SgCastExp *cast = buildCastExp(addr, ptr_type);
    
    std::string func_name = intel_simd_func(Load, mask_pntr->get_type());
    SgExprListExp *parameters = buildExprListExp(cast);
    SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
    SgAssignInitializer *local_init = buildAssignInitializer(ld);
    
    SgVariableDeclaration *mask_vd = buildVariableDeclaration(mask_name, mask_type, local_init, new_block);
    appendStatement(mask_vd, new_block);
    
    // Now for the scatter statement
    SgVarRefExp *mask_ref = buildVarRefExp(mask_name, new_block);
    SgVarRefExp *base_ref = static_cast<SgVarRefExp *>(element->get_lhs_operand());
    
    int scale = 4;
    if (dest->get_type()->variantT() == V_SgTypeDouble) {
        loop_increment = simd_len / 2;
        scale = 8;
        
        // If we have a double, we also need to do an extraction of the mask
        SgType *extract_type;
        std::string extract_name = "";
        std::string mask_name2 = mask_name + "2";
        
        if (simd_len == 16) {
            extract_type = buildOpaqueType("__m256i", new_block);
            extract_name = "_mm512_extracti32x8_epi32";
        } else {
            extract_type = buildOpaqueType("__m128i", new_block);
            extract_name = "_mm256_extractf128_si256";
        }
        
        parameters = buildExprListExp(mask_ref, buildIntVal(0));
        ld = buildFunctionCallExp(extract_name, extract_type, parameters, new_block);
        local_init = buildAssignInitializer(ld);
        mask_vd = buildVariableDeclaration(mask_name2, extract_type, local_init, new_block);
        appendStatement(mask_vd, new_block);
        
        mask_ref = buildVarRefExp(mask_name2, new_block);
    }
    
    func_name = intel_simd_func(Scatter, dest->get_type());
    parameters = buildExprListExp(base_ref, mask_ref, dest, buildIntVal(scale));
    SgExprStatement *fc = buildFunctionCallStmt(func_name, buildVoidType(), parameters, new_block);
    appendStatement(fc, new_block);
}

// ==================================================================================================
// Generates an Intel partial-store statement
//
void intel_write_partial_store(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block) {
    SgVarRefExp *var = static_cast<SgVarRefExp *>(op->get_lhs_operand());
    
    // First, create the vector outside the loop and zero it
    SgType *vector_type = intel_simd_type(var->get_type(), target->get_scope());
    SgName name = var->get_symbol()->get_name();
    
    std::string func_name = intel_simd_func(BroadcastZero, var->get_type());
    
    SgExpression *ld = buildFunctionCallExp(func_name, vector_type, NULL, new_block);
    SgAssignInitializer *local_init = buildAssignInitializer(ld);
    
    SgVariableDeclaration *vd = buildVariableDeclaration(name, vector_type, local_init, new_block);
    insertStatementBefore(target, vd);
}

// ============================================================================================
// Generates an Intel scalar-store statement
//
// Scalar store:
//
//__m256 __sub1 = _mm512_extractf32x8_ps(__vec6, 0);
//__m256 __sub2 = _mm512_extractf32x8_ps(__vec6, 1);
//__sub2 = _mm256_add_ps(__sub1, __sub2);
//__sub2 = _mm256_hadd_ps(__sub2,__sub2);
//__sub2 = _mm256_hadd_ps(__sub2,__sub2);
//float __buf0[8];
//_mm256_storeu_ps(&__buf0,__sub2);
//temp = __buf0[1] + __buf0[5];
//
void intel_write_scalar_store(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
    
    SgVarRefExp *scalar = static_cast<SgVarRefExp *>(lval);
    SgVarRefExp *vec = static_cast<SgVarRefExp *>(rval);
    std::vector<SgStatement *> to_insert;
    
    // Create the types
    SgType *vector_type = intel_simd_type(scalar->get_type(), target->get_scope(), true);
    
    SgAssignInitializer *local_init;
    std::string vec1 = intel_gen_buf();
    std::string vec2 = intel_gen_buf();
    
    SgExprListExp *hadd_params;
    
    // Extract
    if (simd_len == 16) {
        std::string extract_name = intel_simd_func(Extract, vec->get_type());
        
        SgIntVal *val = buildIntVal(0);
        SgExprListExp *parameters = buildExprListExp(vec, val);
        SgExpression *fc1 = buildFunctionCallExp(extract_name, vector_type, parameters, new_block);
        
        local_init = buildAssignInitializer(fc1);
        SgVariableDeclaration *vd1 = buildVariableDeclaration(vec1, vector_type, local_init, new_block);
        to_insert.push_back(vd1);
        
        val = buildIntVal(1);
        parameters = buildExprListExp(vec, val);
        SgExpression *fc2 = buildFunctionCallExp(extract_name, vector_type, parameters, new_block);
        
        local_init = buildAssignInitializer(fc2);
        SgVariableDeclaration *vd2 = buildVariableDeclaration(vec2, vector_type, local_init, new_block);
        to_insert.push_back(vd2);
        
        // Add the two sub vectors
        SgVarRefExp *sub1 = buildVarRefExp(vec1, new_block);
        SgVarRefExp *sub2 = buildVarRefExp(vec2, new_block);
        
        parameters = buildExprListExp(sub1, sub2);
        std::string func_name = intel_simd_func(Add, vec->get_type(), true);
        SgExpression *fc3 = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
        SgExprStatement *expr = buildAssignStatement(sub2, fc3);
        to_insert.push_back(expr);
    } else if (simd_len == 8) {
        local_init = buildAssignInitializer(vec);
        
        SgVariableDeclaration *vd2 = buildVariableDeclaration(vec2, vector_type, local_init, new_block);
        to_insert.push_back(vd2);
    }
    
    // Perform two horizontal adds
    SgVarRefExp *sub2 = buildVarRefExp(vec2, new_block);
    hadd_params = buildExprListExp(sub2, sub2);
    
    std::string func_name = intel_simd_func(HAdd, vec->get_type(), true);
    SgExpression *fc4 = buildFunctionCallExp(func_name, vector_type, hadd_params, new_block);
    SgExprStatement *expr = buildAssignStatement(sub2, fc4);
    to_insert.push_back(expr);
    
    if (vec->get_type()->variantT() != V_SgTypeDouble) {
        expr = buildAssignStatement(sub2, fc4);
        to_insert.push_back(expr);
    }
    
    // Create the buffer
    std::string name = intel_gen_buf();
    
    int buf_length = 8;
    int pos2 = 6;
    if (vec->get_type()->variantT() == V_SgTypeDouble) {
        buf_length = 4;
        pos2 = 2;
    }
    
    SgIntVal *length = buildIntVal(buf_length);
    SgType *type = buildArrayType(scalar->get_type(), length);
    
    SgVariableDeclaration *vd = buildVariableDeclaration(name, type, NULL, new_block);
    to_insert.push_back(vd);
    
    SgVarRefExp *vd_ref = buildVarRefExp(name, new_block);
    SgExprListExp *parameters;
    
    // Store
    if (vec->get_type()->variantT() == V_SgTypeInt) {
        SgAddressOfOp *addr = buildAddressOfOp(vd_ref);
        SgPointerType *ptr_type = buildPointerType(vector_type);
        SgCastExp *cast = buildCastExp(addr, ptr_type);
        parameters = buildExprListExp(cast, sub2);
    } else {
        SgAddressOfOp *addr = buildAddressOfOp(vd_ref);
        parameters = buildExprListExp(addr, sub2);
    }
    
    func_name = intel_simd_func(Store, vec->get_type(), true);
    SgExprStatement *fc = buildFunctionCallStmt(func_name, buildVoidType(), parameters, new_block);
    to_insert.push_back(fc);
    
    // Scalar store
    // temp = __buf0[1] + __buf0[6];
    SgPntrArrRefExp *pntr1 = buildPntrArrRefExp(vd_ref, buildIntVal(0));
    SgPntrArrRefExp *pntr2 = buildPntrArrRefExp(vd_ref, buildIntVal(pos2));
    SgAddOp *add = buildAddOp(pntr1, pntr2);
    SgAssignOp *assign = buildAssignOp(scalar, add);
    
    expr = buildExprStatement(assign);
    to_insert.push_back(expr);
    
    // Now, add it all to the end of the loop
    // We have to add backwards to put them in the right order
    for (int i = to_insert.size() - 1; i >= 0; i--) {
        insertStatementAfter(target, to_insert.at(i));
    }
}

// ===========================================================================================================
// Writes an Intel SIMD math statement
//
SgAssignInitializer *intel_write_math(SgBinaryOp *op, SgOmpSimdStatement *target, SgBasicBlock *new_block, VariantT math_type) {
    SgExpression *lval = op->get_lhs_operand();
    SgExpression *rval = op->get_rhs_operand();
    
    SgVarRefExp *va = static_cast<SgVarRefExp *>(lval);
    std::string name = va->get_symbol()->get_name().getString();
    
    SgType *vector_type = intel_simd_type(va->get_type(), target->get_scope());
    SgExprListExp *parameters = static_cast<SgExprListExp *>(rval);
    
    // Build the function call
    OpType x86Type = None;
    switch (math_type) {
        case V_SgSIMDAddOp: x86Type = Add; break;
        case V_SgSIMDSubOp: x86Type = Sub; break;
        case V_SgSIMDMulOp: x86Type = Mul; break;
        case V_SgSIMDDivOp: x86Type = Div; break;
        default: {}
    }
    
    std::string func_type = intel_simd_func(x86Type, va->get_type());
    SgExpression *ld = buildFunctionCallExp(func_type, vector_type, parameters, new_block);
    
    if (name.rfind("__part", 0) == 0) {
        SgExprStatement *assign = buildAssignStatement(va, ld);
        appendStatement(assign, new_block);
        return NULL;
    } else {
        return buildAssignInitializer(ld);
    }
}

// =======================================================================================================================================
// Write the Intel intrinsics
void omp_simd_write_intel(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block, int simd_length) {
    // Set the simd_len variable
    if (simd_length == 0) {
        simd_len = 16;
    } else {
        simd_len = simd_length;
    }
    
    // Setup the for loop
    SgBasicBlock *new_block = SageBuilder::buildBasicBlock();
    
    SgStatement *loop_body = getLoopBody(for_loop);
    replaceStatement(loop_body, new_block, true);
    
    loop_increment = simd_len;
    
    // Translate the IR
    for (Rose_STL_Container<SgNode *>::iterator i = ir_block->begin(); i != ir_block->end(); i++) {
        if (!isSgBinaryOp(*i)) {
            continue;
        }
        
        SgBinaryOp *op = static_cast<SgBinaryOp *>(*i);
        SgExpression *lval = op->get_lhs_operand();
        SgExpression *rval = op->get_rhs_operand();
        
        SgAssignInitializer *init = NULL;
        
        switch ((*i)->variantT()) {
            case V_SgSIMDLoad: {
                init = intel_write_load(op, target, new_block);
            } break;
            
            case V_SgSIMDBroadcast: {
                init = intel_write_broadcast(op, target, new_block);
            } break;
            
            case V_SgSIMDGather: {
                init = intel_write_gather(op, target, new_block);
            } break;
            
            case V_SgSIMDStore: {
                intel_write_store(op, new_block);
            } break;
            
            case V_SgSIMDScatter: {
                intel_write_scatter(op, target, new_block);
            } break;
            
            case V_SgSIMDPartialStore: {
                intel_write_partial_store(op, target, new_block);
            } break;
            
            case V_SgSIMDScalarStore: {
                intel_write_scalar_store(op, target, new_block);
            } break;
            
            case V_SgSIMDAddOp:
            case V_SgSIMDSubOp:
            case V_SgSIMDMulOp:
            case V_SgSIMDDivOp: {
                init = intel_write_math(op, target, new_block, (*i)->variantT());
            } break;
            
            default: {
                init = buildAssignInitializer(rval);
            }
        }
        
        // The variable declaration
        if ((*i)->variantT() != V_SgSIMDScalarStore && (*i)->variantT() != V_SgSIMDPartialStore) {
            if (isSgVarRefExp(lval)) {
                SgVarRefExp *var = static_cast<SgVarRefExp *>(lval);
                
                SgType *vector_type = intel_simd_type(var->get_type(), target->get_scope());
                SgName name = var->get_symbol()->get_name();
                
                if (name.getString().rfind("__part", 0) != 0) {
                    SgVariableDeclaration *vd = buildVariableDeclaration(name, vector_type, init, new_block);
                    
                    if ((*i)->variantT() == V_SgSIMDBroadcast)
                        insertStatementBefore(target, vd);
                    else
                        appendStatement(vd, new_block);
                }
            }
        }
    }
    
    // Update the loop increment
    SgExpression *inc = for_loop->get_increment();

    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(inc, V_SgExpression);
    SgIntVal *inc_amount = isSgIntVal(nodeList.at(2));
    ROSE_ASSERT(inc_amount != NULL);
    inc_amount->set_value(loop_increment);
}

