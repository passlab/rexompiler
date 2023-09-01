#include <iostream>
#include <vector>
#include <string>

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
int vi_pos = 0;
int arm_buf_pos = 0;

// For maintaining declarations
std::vector<std::string> arm_partial_broadcasts;

std::string arm_gen_buf() {
    char str[5];
    sprintf(str, "%d", arm_buf_pos);
    
    std::string name = "__buf" + std::string(str);
    ++arm_buf_pos;
    return name;
}

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

//
// This is specific to the loop unrolling.
// If we find this specific sequence, we very likely have an index altered by the loopUnrolling
// from an OMP unroll clause. In that case, we need to adjust the base with the proper loop
// increment value.
//
void arm_normalize_offset(SgPntrArrRefExp *array, SgExpression *inc_fc) {
    SgAddOp *add = isSgAddOp(array->get_rhs_operand());
    if (!add) return;
    
    SgMultiplyOp *mul = isSgMultiplyOp(add->get_rhs_operand());
    if (!mul) {
        SgAddOp *add2 = isSgAddOp(add->get_rhs_operand());
        if (!add2) return;
        
        mul = isSgMultiplyOp(add2->get_rhs_operand());
        if (!mul) return;
    }
    
    //SgIntVal *inc = isSgIntVal(mul->get_lhs_operand());
    //if (!inc) return;
    
    //SgExpression *inc_fc = buildFunctionCallExp(pred_count_name, buildIntType(), NULL, for_loop);
    mul->set_lhs_operand(inc_fc);
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
    bool pg64 = false;
    
    SgNode *first_node = ir_block->at(0);
    if (!isSgBinaryOp(first_node)) return;
    SgBinaryOp *first = static_cast<SgBinaryOp *>(first_node);
    
    if (first->get_type()->variantT() == V_SgTypeDouble) {
        pred_func_name = "svwhilelt_b64";
        pred_count_name = "svcntd";
        pg64 = true;
    } else if (first->get_type()->variantT() == V_SgPointerType) {
        SgPointerType *pt = static_cast<SgPointerType *>(first->get_type());
        if (pt->get_base_type()->variantT() == V_SgTypeDouble) {
            pred_func_name = "svwhilelt_b64";
            pred_count_name = "svcntd";
            pg64 = true;
        }
    }

    // Get for loop information
    SgExprStatement *test_stmt = static_cast<SgExprStatement *>(for_loop->get_test());
    SgBinaryOp *test_op = static_cast<SgBinaryOp *>(test_stmt->get_expression());
    SgExpression *max_val = test_op->get_rhs_operand();
    
    SgExpression *start;
    //if (pg64) {
        start = buildCastExp(buildIntVal(0), buildUnsignedLongType());
    //} else {
        //start = buildIntVal(0);
    //}
    max_val = buildCastExp(max_val, buildUnsignedLongType());
    SgExprListExp *parameters = buildExprListExp(start, max_val);
    
    SgType *pred_type = buildOpaqueType("svbool_t", new_block);
    SgExpression *predicate = buildFunctionCallExp(pred_func_name, pred_type, parameters, target->get_scope());
    SgAssignInitializer *init = buildAssignInitializer(predicate);
    
    SgVariableDeclaration *vd = buildVariableDeclaration(pg_name, pred_type, init, new_block);
    Rose_STL_Container<SgNode *> bodyList = NodeQuery::querySubTree(getEnclosingScope(target), V_SgVariableDeclaration);
    bool insertFound = false;
    for (Rose_STL_Container<SgNode *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        SgVariableDeclaration *var = isSgVariableDeclaration(*i);
        std::string name = var->get_variables().at(0)->get_name();
        if (name.rfind("_lt_var_", 0) == 0 && name != "_lt_var_inc") {
            insertStatementAfter(var, vd);
            insertFound = true;
            break;
        }
    }
    if (!insertFound) insertStatementBefore(target, vd);
    
    // Build this for safe keeping
    SgExpression *inc_fc = buildFunctionCallExp(pred_count_name, buildIntType(), NULL, target->get_scope());
    
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
                
                arm_normalize_offset(array, inc_fc);
                
                SgAddressOfOp *addr = buildAddressOfOp(array);
                SgExprListExp *parameters = buildExprListExp(pred_ref, addr);
                
                SgExpression *ld = buildFunctionCallExp("svld1", vector_type, parameters, target->get_scope());
                init = buildAssignInitializer(ld);
            } break;
            
            // Build the broadcast
            case V_SgSIMDBroadcast: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *src = static_cast<SgVarRefExp *>(rval);
                SgType *vector_type = arm_get_type(dest->get_type(), new_block);
                
                SgExprListExp *parameters = buildExprListExp(src);
                std::string func_name = arm_get_func(dest->get_type(), Broadcast);
                
                SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, target->get_scope());
                init = buildAssignInitializer(ld);
            } break;
            
            // Gather load
            case V_SgSIMDGather: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                SgPntrArrRefExp *element = static_cast<SgPntrArrRefExp *>(rval);
                SgPntrArrRefExp *mask_pntr = static_cast<SgPntrArrRefExp *>(element->get_rhs_operand());
                
                std::string vindex_name = "__vindex" + std::to_string(vi_pos);
                ++vi_pos;
                SgType *mask_type = arm_get_type(mask_pntr->get_type(), new_block);
                SgType *vector_type = arm_get_type(dest->get_type(), new_block);
                
                // Load the array indicies
                SgAddressOfOp *addr = buildAddressOfOp(mask_pntr);
                SgExprListExp *parameters = buildExprListExp(pred_ref, addr);
                
                SgExpression *ld = buildFunctionCallExp("svld1", mask_type, parameters, target->get_scope());
                SgAssignInitializer *local_init = buildAssignInitializer(ld);
                
                SgVariableDeclaration *mask_vd = buildVariableDeclaration(vindex_name, mask_type, local_init, new_block);
                appendStatement(mask_vd, new_block);
                
                // The actual gather instruction
                SgVarRefExp *mask_ref = buildVarRefExp(vindex_name, new_block);
                SgVarRefExp *base_ref = static_cast<SgVarRefExp *>(element->get_lhs_operand());
                
                parameters = buildExprListExp(pred_ref, base_ref, mask_ref);
                ld = buildFunctionCallExp("svld1_gather_index", vector_type, parameters, target->get_scope());
                init = buildAssignInitializer(ld);
            } break;
            
            // The regular vector store
            case V_SgSIMDStore: {
                SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(lval);
                SgVarRefExp *src = static_cast<SgVarRefExp *>(rval);
                
                arm_normalize_offset(array, inc_fc);
                
                SgAddressOfOp *addr = buildAddressOfOp(array);
                SgExprListExp *parameters = buildExprListExp(pred_ref, addr, src);
                
                SgExprStatement *str = buildFunctionCallStmt("svst1", buildVoidType(), parameters, target->get_scope());
                appendStatement(str, new_block);
            } break;
            
            // Partial store (save partial sums to a register)
            // Basically, all we do is create a zero'ed register outside the for-loop
            case V_SgSIMDPartialStore: {
                SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *srcVar = static_cast<SgVarRefExp *>(rval);
                
                SgType *vector_type = arm_get_type(dest->get_type(), new_block);
                std::string dest_name = dest->get_symbol()->get_name();
                
                if (std::find(arm_partial_broadcasts.begin(), arm_partial_broadcasts.end(), dest_name) != arm_partial_broadcasts.end()) {
                    // Found
                } else {
                    SgExpression *val;
                    switch (dest->get_type()->variantT()) {
                        case V_SgTypeFloat: val = buildFloatVal(0); break;
                        case V_SgTypeDouble: val = buildDoubleVal(0); break; 
                        default: val = buildIntVal(0);
                    }
                    
                    SgExprListExp *parameters = buildExprListExp(val);
                    std::string func_name = arm_get_func(dest->get_type(), Broadcast);
                    
                    SgExpression *ld = buildFunctionCallExp(func_name, vector_type, parameters, target->get_scope());
                    SgAssignInitializer *local_init = buildAssignInitializer(ld);
                    
                    SgVariableDeclaration *vd = buildVariableDeclaration(dest_name, vector_type, local_init, new_block);
                    //insertStatementBefore(target, vd);
                    prependStatement(vd, getEnclosingScope(target));
                    
                    arm_partial_broadcasts.push_back(dest_name);
                }
                
                init = buildAssignInitializer(srcVar);
            } break;
            
            // Scalar store:
            //
            // __pg0 = svptrue_b64();
            // result = svaddv_f64(__pg0, __part0);
            //
            case V_SgSIMDScalarStore: {
                SgVarRefExp *scalar = static_cast<SgVarRefExp *>(lval);
                SgVarRefExp *vec = static_cast<SgVarRefExp *>(rval);
                
                // Reset the predicate
                //predicate = buildFunctionCallExp("svptrue_b32", pred_type, NULL, new_block);
                switch (scalar->get_type()->variantT()) {
                    case V_SgTypeInt:
                    case V_SgTypeFloat: {
                        predicate = buildFunctionCallExp("svptrue_b32", pred_type, NULL, target->get_scope());
                    } break;
                    
                    case V_SgTypeDouble: {
                        predicate = buildFunctionCallExp("svptrue_b64", pred_type, NULL, target->get_scope());
                    } break;
                    
                    default: {}
                }
                
                SgVarRefExp *pred_var = buildVarRefExp(pg_name, new_block);
                SgExprStatement *pred_update = buildAssignStatement(pred_var, predicate);
                insertStatementAfter(target, pred_update);
                
                // result = svaddv(__pg0, __part0);
                SgExprListExp *parameters = buildExprListExp(pred_var, vec);
                SgFunctionCallExp *reductionCall = buildFunctionCallExp("svaddv",
                                                    scalar->get_type(), parameters, target->get_scope());
                SgPlusAssignOp *scalar_add = buildPlusAssignOp(scalar, reductionCall);
                SgExprStatement *empty = buildExprStatement(scalar_add);
                insertStatementAfter(pred_update, empty);
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
                
                SgExpression *fc = buildFunctionCallExp(func_name, vector_type, parameters, target->get_scope());
                
                if (name.rfind("__part", 0) == 0) {
                    SgExprStatement *assign = buildAssignStatement(dest, fc);
                    appendStatement(assign, new_block);
                } else {
                    init = buildAssignInitializer(fc);
                }
            } break;
            
            default: {
                init = buildAssignInitializer(rval);
            }
        }
        
        // Add the statement
        if ((*i)->variantT() != V_SgSIMDScalarStore) {
            if (isSgVarRefExp(lval)) {
                SgVarRefExp *var = static_cast<SgVarRefExp *>(lval);
                
                SgType *vector_type = arm_get_type(var->get_type(), new_block);
                SgName name = var->get_symbol()->get_name();
                
                if (name.getString().rfind("__part", 0) != 0) {
                    SgVariableDeclaration *vd = buildVariableDeclaration(name, vector_type, init, new_block);
                    
                    if ((*i)->variantT() == V_SgSIMDBroadcast) {
                        //insertStatementBefore(target, vd);
                        prependStatement(vd, getEnclosingScope(target));
                    } else {
                        appendStatement(vd, new_block);
                    }
                } else {
                    SgExprStatement *expr = buildAssignStatement(var, init);
                    appendStatement(expr, new_block);
                }
            }
        }
    }
    
    // At the end of each loop, we need to update the predicate
    SgExpression *loop_var = test_op->get_lhs_operand();
    SgVarRefExp *pred_var = buildVarRefExp(pg_name, new_block);
    
    if (isSgCastExp(max_val)) {
        loop_var = buildCastExp(loop_var, buildUnsignedLongType());
        //buildCastExp(buildIntVal(0), buildUnsignedLongType());
    }
    
    parameters = buildExprListExp(loop_var, max_val);
    predicate = buildFunctionCallExp(pred_func_name, pred_type, parameters, target->get_scope());
    
    SgExprStatement *pred_update = buildAssignStatement(pred_var, predicate);
    appendStatement(pred_update, new_block);
    
    // Update the loop increment
    //SgExpression *inc_fc = buildFunctionCallExp(pred_count_name, buildIntType(), NULL, for_loop);
    SgBinaryOp *inc = static_cast<SgBinaryOp *>(for_loop->get_increment());
    SgMultiplyOp *mul = buildMultiplyOp(inc->get_rhs_operand(), inc_fc);
    inc->set_rhs_operand(mul);
}

