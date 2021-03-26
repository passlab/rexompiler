
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

#define SIMD_LENGTH 16

// For generating names
int buf_pos = 0;

std::string intelGenBufName() {
    char str[5];
    sprintf(str, "%d", buf_pos);
    
    std::string name = "__buf" + std::string(str);
    ++buf_pos;
    return name;
}

// If half_type == true, we return the 256-bit version
// This is needed for scalar stores
SgType *omp_simd_get_intel_type(SgType *type, SgBasicBlock *new_block, bool half_type = false) {
    SgType *vector_type;
       
    if (half_type) {
        switch (type->variantT()) {
            case V_SgTypeInt: vector_type = buildOpaqueType("__m256i", new_block); break;
            case V_SgTypeFloat: vector_type = buildOpaqueType("__m256", new_block); break;
            case V_SgTypeDouble: vector_type = buildOpaqueType("__m256d", new_block); break;
            default: vector_type = type;
        }
    } else {
        switch (type->variantT()) {
            case V_SgTypeInt: vector_type = buildOpaqueType("__m512i", new_block); break;
            case V_SgTypeFloat: vector_type = buildOpaqueType("__m512", new_block); break;
            case V_SgTypeDouble: vector_type = buildOpaqueType("__m512d", new_block); break;
            default: vector_type = type;
        }
    }

    return vector_type;
}

std::string omp_simd_get_intel_func(OpType op_type, SgType *type, bool half_type = false) {
    std::string instr = "_mm512_";
    if (half_type) instr = "_mm256_";

    switch (op_type) {
        case Load: instr += "loadu_"; break;
        case Broadcast: instr += "set1_"; break;
        case BroadcastZero: instr += "setzero_"; break;
        
        case ScalarStore:
        case Store: instr += "storeu_"; break;
    
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
            if (op_type == Load || op_type == Store || op_type == ScalarStore) {
                if (half_type) instr += "si256";
                else instr += "si512";
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

// Write the Intel intrinsics
void omp_simd_write_intel(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block) {
    // Setup the for loop
    SgBasicBlock *new_block = SageBuilder::buildBasicBlock();
    
    SgStatement *loop_body = getLoopBody(for_loop);
    replaceStatement(loop_body, new_block, true);
    
    int loop_increment = SIMD_LENGTH;
    
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
                SgVarRefExp *va = static_cast<SgVarRefExp *>(lval);
                SgType *vector_type = omp_simd_get_intel_type(va->get_type(), new_block);
                SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(rval);
                
                // Check the loop increment
                // TODO: Is this the best location for this?
                if (va->get_type()->variantT() == V_SgTypeDouble) {
                    loop_increment = SIMD_LENGTH / 2;
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
                std::string func_name = omp_simd_get_intel_func(Load, va->get_type());
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                init = buildAssignInitializer(ld);
            } break;
            
            case V_SgSIMDBroadcast: {
                SgVarRefExp *v_dest = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *v_src = static_cast<SgVarRefExp *>(rval);
                SgType *vector_type = omp_simd_get_intel_type(v_dest->get_type(), new_block);
                
                // Function call parameters
                SgExprListExp *parameters = buildExprListExp(v_src);
                
                // Build the function call and place it above the for loop
                std::string func_name = omp_simd_get_intel_func(Broadcast, v_dest->get_type());
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                init = buildAssignInitializer(ld);
            } break;
            
            case V_SgSIMDStore: {
                SgVarRefExp *v_src = static_cast<SgVarRefExp *>(rval);
                
                // Function call parameters
                SgAddressOfOp *addr = buildAddressOfOp(lval);
                SgExprListExp *parameters = buildExprListExp(addr, v_src);
                
                // Build the function call
                std::string func_name = omp_simd_get_intel_func(Store, v_src->get_type());
                
                SgExprStatement *fc = buildFunctionCallStmt(func_name, buildVoidType(), parameters, new_block);
                appendStatement(fc, new_block);
            } break;
            
            case V_SgSIMDPartialStore: {
                SgVarRefExp *var = static_cast<SgVarRefExp *>(lval);
                
                // First, create the vector outside the loop and zero it
                SgType *vector_type = omp_simd_get_intel_type(var->get_type(), new_block);
                SgName name = var->get_symbol()->get_name();
                
                std::string func_name = omp_simd_get_intel_func(BroadcastZero, var->get_type());
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, NULL, new_block);
                SgAssignInitializer *local_init = buildAssignInitializer(ld);
                
                SgVariableDeclaration *vd = buildVariableDeclaration(name, vector_type, local_init, new_block);
                insertStatementBefore(target, vd);
            } break;
            
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
            // TODO: This needs abstracting for types and function names
            //
            case V_SgSIMDScalarStore: {
                SgVarRefExp *scalar = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *vec = static_cast<SgVarRefExp *>(rval);
                std::vector<SgStatement *> to_insert;
                
                // Create the types
                SgAssignInitializer *local_init;
                SgType *vector_type = omp_simd_get_intel_type(scalar->get_type(), new_block, true);
                std::string vec1 = intelGenBufName();
                std::string vec2 = intelGenBufName();
                
                // Extract
                std::string extract_name = omp_simd_get_intel_func(Extract, vec->get_type());
                
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
                std::string func_name = omp_simd_get_intel_func(Add, vec->get_type(), true);
                SgExpression *fc3 = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                SgExprStatement *expr = buildAssignStatement(sub2, fc3);
                to_insert.push_back(expr);
                
                // Perform two horizontal adds
                parameters = buildExprListExp(sub2, sub2);
                
                func_name = omp_simd_get_intel_func(HAdd, vec->get_type(), true);
                SgExpression *fc4 = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                expr = buildAssignStatement(sub2, fc4);
                to_insert.push_back(expr);
                
                if (vec->get_type()->variantT() != V_SgTypeDouble) {
                    expr = buildAssignStatement(sub2, fc4);
                    to_insert.push_back(expr);
                }
                
                // Create the buffer
                std::string name = intelGenBufName();
                
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
                
                func_name = omp_simd_get_intel_func(Store, vec->get_type(), true);
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
                
            } break;
            
            case V_SgSIMDAddOp:
            case V_SgSIMDSubOp:
            case V_SgSIMDMulOp:
            case V_SgSIMDDivOp: {
                SgVarRefExp *va = static_cast<SgVarRefExp *>(lval);
                std::string name = va->get_symbol()->get_name().getString();
                
                SgType *vector_type = omp_simd_get_intel_type(va->get_type(), new_block);
                SgExprListExp *parameters = static_cast<SgExprListExp *>(rval);
                
                // Build the function call
                OpType x86Type = None;
                switch ((*i)->variantT()) {
                    case V_SgSIMDAddOp: x86Type = Add; break;
                    case V_SgSIMDSubOp: x86Type = Sub; break;
                    case V_SgSIMDMulOp: x86Type = Mul; break;
                    case V_SgSIMDDivOp: x86Type = Div; break;
                    default: {}
                }
                
                std::string func_type = omp_simd_get_intel_func(x86Type, va->get_type());
                SgExpression *ld = buildFunctionCallExp(func_type, vector_type, parameters, new_block);
                
                if (name.rfind("__part", 0) == 0) {
                    SgExprStatement *assign = buildAssignStatement(va, ld);
                    appendStatement(assign, new_block);
                } else {
                    init = buildAssignInitializer(ld);
                }
            } break;
            
            default: {
                init = buildAssignInitializer(rval);
            }
        }
        
        // The variable declaration
        if ((*i)->variantT() != V_SgSIMDScalarStore && (*i)->variantT() != V_SgSIMDPartialStore) {
            if (isSgVarRefExp(lval)) {
                SgVarRefExp *var = static_cast<SgVarRefExp *>(lval);
                
                SgType *vector_type = omp_simd_get_intel_type(var->get_type(), new_block);
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

