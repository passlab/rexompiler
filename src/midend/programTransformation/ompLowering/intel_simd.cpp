
#include <iostream>
#include <stack>
#include <queue>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "omp_lowering.h"

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

SgType *omp_simd_get_intel_type(SgType *type, SgBasicBlock *new_block) {
    SgType *vector_type;
        
    switch (type->variantT()) {
        case V_SgTypeInt: vector_type = buildOpaqueType("__m512i", new_block); break;
        case V_SgTypeFloat: vector_type = buildOpaqueType("__m512", new_block); break;
        case V_SgTypeDouble: vector_type = buildOpaqueType("__m512d", new_block); break;
        default: vector_type = type;
    }

    return vector_type;
}

std::string omp_simd_get_intel_func(VariantT op_type, SgType *type) {
    std::string instr = "_mm512_";

    switch (op_type) {
        case V_SgSIMDLoad: instr += "loadu_"; break;
        case V_SgSIMDBroadcast: instr += "set1_"; break;
        
        case V_SgSIMDScalarStore:
        case V_SgSIMDStore: instr += "storeu_"; break;
    
        case V_SgSIMDAddOp: instr += "add_"; break;
        case V_SgSIMDSubOp: instr += "sub_"; break;
        case V_SgSIMDMulOp: instr += "mul_"; break;
        case V_SgSIMDDivOp: instr += "div_"; break;
        
        default: {}
    }
    
    switch (type->variantT()) {
        case V_SgTypeInt: {
            if (op_type == V_SgSIMDLoad || op_type == V_SgSIMDStore || op_type == V_SgSIMDScalarStore)
                instr += "si512";
            else
                instr += "epi32";
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
        
        // The variable declaration
        if (isSgVarRefExp(lval) && (*i)->variantT() != V_SgSIMDScalarStore) {
            SgVarRefExp *var = static_cast<SgVarRefExp *>(lval);
            
            SgType *vector_type = omp_simd_get_intel_type(var->get_type(), new_block);
            SgName name = var->get_symbol()->get_name();
            
            SgVariableDeclaration *vd = buildVariableDeclaration(name, vector_type, NULL, new_block);
            
            if ((*i)->variantT() == V_SgSIMDBroadcast)
                insertStatementBefore(target, vd);
            else
                appendStatement(vd, new_block);
        }
        
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
                std::string func_name = omp_simd_get_intel_func((*i)->variantT(), va->get_type());
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                SgExprStatement *expr = buildAssignStatement(va, ld);
                appendStatement(expr, new_block);
            } break;
            
            case V_SgSIMDBroadcast: {
                SgVarRefExp *v_dest = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *v_src = static_cast<SgVarRefExp *>(rval);
                SgType *vector_type = omp_simd_get_intel_type(v_dest->get_type(), new_block);
                
                // Function call parameters
                SgExprListExp *parameters = buildExprListExp(v_src);
                
                // Build the function call and place it above the for loop
                std::string func_name = omp_simd_get_intel_func((*i)->variantT(), v_dest->get_type());
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                SgExprStatement *expr = buildAssignStatement(v_dest, ld);
                insertStatementBefore(target, expr);
            } break;
            
            case V_SgSIMDStore: {
                SgVarRefExp *v_src = static_cast<SgVarRefExp *>(rval);
                
                // Function call parameters
                SgAddressOfOp *addr = buildAddressOfOp(lval);
                SgExprListExp *parameters = buildExprListExp(addr, v_src);
                
                // Build the function call
                std::string func_name = omp_simd_get_intel_func((*i)->variantT(), v_src->get_type());
                
                SgExprStatement *fc = buildFunctionCallStmt(func_name, buildVoidType(), parameters, new_block);
                appendStatement(fc, new_block);
            } break;
            
            // Scalar store:
            // c = __vec
            // ->
            // float __c_buf[SIMD_LENGTH];
            // for (int i = 0; i<SIMD_LENGTH; i++)
            //     c += __c_buf[i]
            //
            case V_SgSIMDScalarStore: {
                SgVarRefExp *scalar = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *vec = static_cast<SgVarRefExp *>(rval);
                
                // Create the buffer
                std::string name = intelGenBufName();
                
                SgIntVal *length = buildIntVal(SIMD_LENGTH);
                SgExprListExp *index_list = buildExprListExp(length);
                SgType *type = buildArrayType(scalar->get_type(), length);
                
                SgVariableDeclaration *vd = buildVariableDeclaration(name, type, NULL, new_block);
                appendStatement(vd, new_block);
                
                SgVarRefExp *vd_ref = buildVarRefExp(name, new_block);
                
                // Store
                SgAddressOfOp *addr = buildAddressOfOp(vd_ref);
                SgExprListExp *parameters = buildExprListExp(addr, vec);
                
                std::string func_name = omp_simd_get_intel_func((*i)->variantT(), vec->get_type());
                
                SgExprStatement *fc = buildFunctionCallStmt(func_name, buildVoidType(), parameters, new_block);
                appendStatement(fc, new_block);
                
                // Now for the loop (get it?)
                // int __i
                std::string i_name = "__i";
                type = buildIntType();
                
                SgIntVal *val = buildIntVal(0);
                SgAssignInitializer *init = buildAssignInitializer(val);
                SgVariableDeclaration *index_var = buildVariableDeclaration(i_name, type, init, NULL);
                
                // __i < SIMD_LENGTH
                SgVarRefExp *index_ref = buildVarRefExp(i_name);
                SgLessThanOp *cmp_op = buildLessThanOp(index_ref, length);
                SgExprStatement *cmp_expr = buildExprStatement(cmp_op);
                
                // i++
                SgPlusPlusOp *inc = buildPlusPlusOp(index_ref);
                
                // c += __c_buf[i]
                SgPntrArrRefExp *pntr = buildPntrArrRefExp(vd_ref, index_ref);
                SgPlusAssignOp *assign = buildPlusAssignOp(scalar, pntr);
                SgExprStatement *body = buildExprStatement(assign);
                
                // The for loop
                SgForStatement *for_stmt = buildForStatement(index_var, cmp_expr, inc, body);
                appendStatement(for_stmt, new_block);
            } break;
            
            case V_SgSIMDAddOp:
            case V_SgSIMDSubOp:
            case V_SgSIMDMulOp:
            case V_SgSIMDDivOp: {
                SgVarRefExp *va = static_cast<SgVarRefExp *>(lval);
                SgType *vector_type = omp_simd_get_intel_type(va->get_type(), new_block);
                SgExprListExp *parameters = static_cast<SgExprListExp *>(rval);
                
                // Build the function call
                std::string func_type = omp_simd_get_intel_func((*i)->variantT(), va->get_type());
                
                SgExpression *ld = buildFunctionCallExp(func_type, vector_type, parameters, new_block);
                SgExprStatement *expr = buildAssignStatement(va, ld);
                appendStatement(expr, new_block);
            } break;
            
            default: {
                SgExprStatement *expr = buildAssignStatement(lval, rval);
                appendStatement(expr, new_block);
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

