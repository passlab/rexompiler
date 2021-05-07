#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "omp_lowering.h"
#include "omp_simd.h"

using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;

////////////////////////////////////////////////////////////////////////////////////
// The final conversion step- Convert to Arm SVE intrinsics

// Global variables to for naming control
int pg_pos = 0;

// Returns the corresponding function based on a given type
std::string arm_get_func(SgType *input, OpType type) {
    switch (input->variantT()) {
        case V_SgTypeInt: {
            switch (type) {
                case Add: return "svadd_s32_m";
                case Sub: return "svsub_s32_m";
                case Mul: return "svmul_s32_m";
                case Div: return "svdiv_s32_m";
                case Broadcast: return "svdup_s32";
                default: {}
            }
        } break;
        
        case V_SgTypeFloat: {
            switch (type) {
                case Add: return "svadd_f32_m";
                case Sub: return "svsub_f32_m";
                case Mul: return "svmul_f32_m";
                case Div: return "svdiv_f32_m";
                case Broadcast: return "svdup_f32";
                default: {}
            }
        } break;
        
        case V_SgTypeDouble: {
            switch (type) {
                case Add: return "svadd_f64_m";
                case Sub: return "svsub_f64_m";
                case Mul: return "svmul_f64_m";
                case Div: return "svdiv_f64_m";
                case Broadcast: return "svdup_f64";
                default: {}
            }
        } break;
        
        default: return "";
    }

    return "";
}

// Returns the corresponding vector type for a given scalar type
SgType *arm_get_type(SgType *input, SgBasicBlock *new_block) {
    switch (input->variantT()) {
        case V_SgTypeInt: return buildOpaqueType("svint32_t", new_block);
        case V_SgTypeFloat: return buildOpaqueType("svfloat32_t", new_block);
        case V_SgTypeDouble: return buildOpaqueType("svfloat64_t", new_block);
        
        default: return input;
    }
    
    return input;
}

// Write the Arm intrinsics
void omp_simd_write_arm(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block) {
    // Setup the for loop
    SgBasicBlock *new_block = SageBuilder::buildBasicBlock();
    
    SgStatement *loop_body = getLoopBody(for_loop);
    replaceStatement(loop_body, new_block, true);
    
    // Create the predicate variable
    // Determine the name of the predicate variable
    char str[5];
    sprintf(str, "%d", pg_pos);
    
    std::string prefix = "__pg";
    std::string pg_name = prefix + std::string(str);
    ++pg_pos;
    
    // Determine the proper function
    std::string pred_func_name = "svwhilelt_b32";
    std::string pred_count_name = "svcntw";
    
    SgNode *first_node = ir_block->at(0);
    if (!isSgBinaryOp(first_node)) return;
    SgBinaryOp *first = static_cast<SgBinaryOp *>(first_node);
    
    if (first->get_type()->variantT() == V_SgTypeDouble) {
        pred_func_name = "svwhilelt_b64";
        pred_count_name = "svcntd";
    }

    // Get for loop information
    SgExprStatement *test_stmt = static_cast<SgExprStatement *>(for_loop->get_test());
    SgBinaryOp *test_op = static_cast<SgBinaryOp *>(test_stmt->get_expression());
    SgExpression *max_val = test_op->get_rhs_operand();
    
    SgIntVal *start = buildIntVal(0);
    SgExprListExp *parameters = buildExprListExp(start, max_val);
    
    SgType *pred_type = buildOpaqueType("svbool_t", new_block);
    SgExpression *predicate = buildFunctionCallExp(pred_func_name, pred_type, parameters, new_block);
    SgAssignInitializer *init = buildAssignInitializer(predicate);
    
    SgVariableDeclaration *vd = buildVariableDeclaration(pg_name, pred_type, init, new_block);
    insertStatementBefore(target, vd);
    
    // Translate the IR
    for (Rose_STL_Container<SgNode *>::iterator i = ir_block->begin(); i != ir_block->end(); i++) {
        if (!isSgBinaryOp(*i)) {
            continue;
        }
        
        SgBinaryOp *op = static_cast<SgBinaryOp *>(*i);
        SgExpression *lval = op->get_lhs_operand();
        SgExpression *rval = op->get_rhs_operand();
        
        SgVarRefExp *pred_ref = buildVarRefExp(pg_name, new_block);
        SgAssignInitializer *init = NULL;
        
        switch ((*i)->variantT()) {
            // The regular vector load
            case V_SgSIMDLoad: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                SgType *vector_type = arm_get_type(dest->get_type(), new_block);
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
                SgType *vector_type = arm_get_type(dest->get_type(), new_block);
                
                SgExprListExp *parameters = buildExprListExp(src);
                std::string func_name = arm_get_func(dest->get_type(), Broadcast);
                
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
                SgType *vector_type = arm_get_type(dest->get_type(), new_block);
                
                SgExprListExp *parameters = static_cast<SgExprListExp *>(rval);
                parameters->prepend_expression(pred_ref);
                
                std::string func_name = "";
                switch ((*i)->variantT()) {
                    case V_SgSIMDAddOp: func_name = arm_get_func(dest->get_type(), Add); break;
                    case V_SgSIMDSubOp: func_name = arm_get_func(dest->get_type(), Sub); break;
                    case V_SgSIMDMulOp: func_name = arm_get_func(dest->get_type(), Mul); break;
                    case V_SgSIMDDivOp: func_name = arm_get_func(dest->get_type(), Div); break;
                    default: {}
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
                
                SgType *vector_type = arm_get_type(var->get_type(), new_block);
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
    SgVarRefExp *pred_var = buildVarRefExp(pg_name, new_block);
    
    parameters = buildExprListExp(loop_var, max_val);
    predicate = buildFunctionCallExp(pred_func_name, pred_type, parameters, new_block);
    
    SgExprStatement *pred_update = buildAssignStatement(pred_var, predicate);
    appendStatement(pred_update, new_block);
    
    // Update the loop increment
    SgVarRefExp *inc = buildVarRefExp("i", for_loop);
    SgExpression *inc_fc = buildFunctionCallExp(pred_count_name, buildIntType(), NULL, for_loop);
    
    SgPlusAssignOp *assign = buildPlusAssignOp(inc, inc_fc);
    for_loop->set_increment(assign);
}

