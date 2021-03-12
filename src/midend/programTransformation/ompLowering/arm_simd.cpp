#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "omp_lowering.h"

using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;

////////////////////////////////////////////////////////////////////////////////////
// The final conversion step- Convert to Arm SVE intrinsics

// Write the Arm intrinsics
void omp_simd_write_arm(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block) {
    // Setup the for loop
    SgBasicBlock *new_block = SageBuilder::buildBasicBlock();
    
    SgStatement *loop_body = getLoopBody(for_loop);
    replaceStatement(loop_body, new_block, true);
    
    // Create the predicate variable
    SgExprStatement *test_stmt = static_cast<SgExprStatement *>(for_loop->get_test());
    SgBinaryOp *test_op = static_cast<SgBinaryOp *>(test_stmt->get_expression());
    SgExpression *max_val = test_op->get_rhs_operand();
    
    SgIntVal *start = buildIntVal(0);
    SgExprListExp *parameters = buildExprListExp(start, max_val);
    
    SgType *pred_type = buildOpaqueType("svbool_t", new_block);
    SgExpression *predicate = buildFunctionCallExp("svwhilelt_b32", pred_type, parameters, new_block);
    SgAssignInitializer *init = buildAssignInitializer(predicate);
    
    SgVariableDeclaration *vd = buildVariableDeclaration("pg", pred_type, init, new_block);
    insertStatementBefore(target, vd);
    
    // Translate the IR
    for (Rose_STL_Container<SgNode *>::iterator i = ir_block->begin(); i != ir_block->end(); i++) {
        if (!isSgBinaryOp(*i)) {
            continue;
        }
        
        SgBinaryOp *op = static_cast<SgBinaryOp *>(*i);
        SgExpression *lval = op->get_lhs_operand();
        SgExpression *rval = op->get_rhs_operand();
        
        SgVarRefExp *pred_ref = buildVarRefExp("pg", new_block);
        init = NULL;
        
        switch ((*i)->variantT()) {
            // The regular vector load
            case V_SgSIMDLoad: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                SgType *vector_type = buildOpaqueType("svfloat32_t", new_block);
                SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(rval);
                
                SgAddressOfOp *addr = buildAddressOfOp(array);
                SgExprListExp *parameters = buildExprListExp(pred_ref, addr);
                
                SgExpression *ld = buildFunctionCallExp("svld1", vector_type, parameters, new_block);
                init = buildAssignInitializer(ld);
            } break;
            
            // Build the broadcast
            case V_SgSIMDBroadcast: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *src = static_cast<SgVarRefExp *>(rval);
                SgType *vector_type = buildOpaqueType("svfloat32_t", new_block);
                
                SgExprListExp *parameters = buildExprListExp(src);
                std::string func_name = "svdup_f32";
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                init = buildAssignInitializer(ld);
            } break;
            
            // The regular vector store
            case V_SgSIMDStore: {
                SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(lval);
                SgVarRefExp *src = static_cast<SgVarRefExp *>(rval);
                
                SgAddressOfOp *addr = buildAddressOfOp(array);
                SgExprListExp *parameters = buildExprListExp(pred_ref, addr, src);
                
                SgExprStatement *str = buildFunctionCallStmt("svst1", buildVoidType(), parameters, new_block);
                appendStatement(str, new_block);
            } break;
            
            case V_SgSIMDPartialStore: {
                init = NULL;
            } break;
            
            case V_SgSIMDScalarStore: {
                init = NULL;
            } break;
            
            case V_SgSIMDAddOp:
            case V_SgSIMDSubOp:
            case V_SgSIMDMulOp:
            case V_SgSIMDDivOp: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                std::string name = dest->get_symbol()->get_name().getString();
                SgType *vector_type = buildOpaqueType("svfloat32_t", new_block);
                
                SgExprListExp *parameters = static_cast<SgExprListExp *>(rval);
                parameters->prepend_expression(pred_ref);
                
                std::string func_name = "";
                switch ((*i)->variantT()) {
                    case V_SgSIMDAddOp: func_name = "svadd_f32_m"; break;
                    case V_SgSIMDSubOp: func_name = "svsub_f32_m"; break;
                    case V_SgSIMDMulOp: func_name = "svmul_f32_m"; break;
                    case V_SgSIMDDivOp: func_name = "svdiv_f32_m"; break;
                }
                
                SgExpression *fc = buildFunctionCallExp(func_name, vector_type, parameters, new_block);
                
                if (name.rfind("__part", 0) == 0) {
                
                } else {
                    init = buildAssignInitializer(fc);
                }
            } break;
            
            default: {
                std::cout << "Invalid or unknown IR" << std::endl;
            }
        }
        
        // Add the statement
        if ((*i)->variantT() != V_SgSIMDScalarStore && (*i)->variantT() != V_SgSIMDPartialStore) {
            if (isSgVarRefExp(lval)) {
                SgVarRefExp *var = static_cast<SgVarRefExp *>(lval);
                
                SgType *vector_type = buildOpaqueType("svfloat32_t", new_block);
                SgName name = var->get_symbol()->get_name();
                
                if (name.getString().rfind("__part", 0) != 0) {
                    SgVariableDeclaration *vd = buildVariableDeclaration(name, vector_type, init, new_block);
                    
                    if ((*i)->variantT() == V_SgSIMDBroadcast) {
                        insertStatementBefore(target, vd);
                    } else {
                        appendStatement(vd, new_block);
                    }
                }
            }
        }
    }
    
    // At the end of each loop, we need to update the predicate
    SgExpression *loop_var = test_op->get_lhs_operand();
    SgVarRefExp *pred_var = buildVarRefExp("pg", new_block);
    
    parameters = buildExprListExp(loop_var, max_val);
    predicate = buildFunctionCallExp("svwhilelt_b32", pred_type, parameters, new_block);
    
    SgExprStatement *pred_update = buildAssignStatement(pred_var, predicate);
    appendStatement(pred_update, new_block);
    
    // Update the loop increment
    SgVarRefExp *inc = buildVarRefExp("i", for_loop);
    SgExpression *inc_fc = buildFunctionCallExp("svcntw", buildIntType(), NULL, for_loop);
    
    SgPlusAssignOp *assign = buildPlusAssignOp(inc, inc_fc);
    for_loop->set_increment(assign);
}

