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

