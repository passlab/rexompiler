
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
// This is all the Pass-1 code

// For generating names
int name_pos = 0;

std::string simdGenName() {
    std::string name = "__vec" + std::to_string(name_pos);
    ++name_pos;
    return name;
}

void omp_simd_build_ptr_assign(SgExpression *pntr_exp, SgBasicBlock *new_block, std::stack<std::string> *nameStack,
                                std::vector<std::string> loop_indexes, SgType *type) {
    // Perform a check and convert a multi-dimensional array to a 1-D array reference
    SgPntrArrRefExp *pntr = static_cast<SgPntrArrRefExp *>(pntr_exp);
    SgExpression *lval = pntr->get_lhs_operand();
    
    // A stack is used to hold the index expressions
    std::stack<SgExpression *> stack;
    SgPntrArrRefExp *array;
    
    // Iterate down the tree and store the index expressions
    while (lval->variantT() == V_SgPntrArrRefExp) {
        SgExpression *current_rval = copyExpression(pntr->get_rhs_operand());
        stack.push(current_rval);
        
        pntr = static_cast<SgPntrArrRefExp *>(lval);
        lval = pntr->get_lhs_operand();
    }
    
    SgExpression *current_rval = copyExpression(pntr->get_rhs_operand());
    stack.push(current_rval);
    
    // There will always be at least one value on the stack; if there's only
    // one, we can just copy the expression and be done it with it. Otherwise,
    // we have to build a reference
    if (stack.size() == 1) {
        array = static_cast<SgPntrArrRefExp *>(copyExpression(pntr));
    } else {
        SgAddOp *topAdd = buildAddOp();
        int index_pos = loop_indexes.size() - 1;
        
        while (stack.size() > 1) {
            SgExpression *index = stack.top();
            stack.pop();
            
            std::string max_var = loop_indexes.at(index_pos);
            --index_pos;
            
            SgVarRefExp *maxVar = buildVarRefExp(max_var, new_block);
            
            SgMultiplyOp *mul = buildMultiplyOp(index, maxVar);
            
            // If the left-hand expression is not NULL, we need to build another layer
            // and shift the left to the right
            if (topAdd->get_lhs_operand() != nullptr) {
                SgExpression *orig = copyExpression(topAdd->get_lhs_operand());
                SgAddOp *oldAdd = buildAddOp(mul, orig);
                
                topAdd = buildAddOp(NULL, oldAdd);
            } else {
                topAdd->set_lhs_operand(mul);
            }
        }
        
        // Pop the final index and add to the math expression
        SgExpression *last_index = stack.top();
        stack.pop();
        
        if (topAdd->get_lhs_operand() == nullptr) {
            topAdd->set_lhs_operand(last_index);
        } else {
            topAdd->set_rhs_operand(last_index);
        }
        
        array = buildPntrArrRefExp(copyExpression(lval), topAdd);
    }
    
    // Now that we are done, build the assignment
    std::string name = simdGenName();
    nameStack->push(name);
    
    SgVariableDeclaration *vd = buildVariableDeclaration(name, type, NULL, new_block);
    appendStatement(vd, new_block);

    SgVarRefExp *va = buildVarRefExp(name, new_block);
    SgExprStatement *expr = buildAssignStatement(va, array);
    appendStatement(expr, new_block);
}

void omp_simd_build_scalar_assign(SgExpression *node, SgBasicBlock *new_block, std::stack<std::string> *nameStack, SgType *type) {
    std::string name = simdGenName();
    nameStack->push(name);

    // Build the assignment
    SgExpression *expr;
    
    switch (node->variantT()) {
        case V_SgIntVal: {
            SgIntVal *val = static_cast<SgIntVal *>(node);
            expr = buildFloatVal(val->get_value());
        } break;
        
        case V_SgFloatVal: {
            SgFloatVal *val = static_cast<SgFloatVal *>(node);
            expr = buildFloatVal(val->get_value());
        } break;
        
        case V_SgDoubleVal: {
            SgDoubleVal *val = static_cast<SgDoubleVal *>(node);
            expr = buildDoubleVal(val->get_value());
        } break;
        
        case V_SgVarRefExp: {
            expr = copyExpression(node);
        } break;
    }
    
    // Build the variable declaration
    SgVariableDeclaration *vd = buildVariableDeclaration(name, type, NULL, new_block);
    appendStatement(vd, new_block);
    
    // Build the assignment
    SgVarRefExp *va = buildVarRefExp(name, new_block);
    SgExprStatement *assign = buildAssignStatement(va, expr);
    appendStatement(assign, new_block);
}

// Builds the math operations
void omp_simd_build_math(SgBasicBlock *new_block, std::stack<std::string> *nameStack, VariantT op_type, SgType *type) {
    std::string name = simdGenName();
    
    SgVariableDeclaration *vd = buildVariableDeclaration(name, type, NULL, new_block);
    appendStatement(vd, new_block);
    
    // Build the assignment
    // The variables
    std::string name1 = nameStack->top();
    nameStack->pop();
    
    std::string name2 = nameStack->top();
    nameStack->pop();
    
    SgVarRefExp *var1 = buildVarRefExp(name1, new_block);
    SgVarRefExp *var2 = buildVarRefExp(name2, new_block);
    
    // The operators
    SgBinaryOp *op;
    
    switch (op_type) {
        case V_SgAddOp: op = buildAddOp(var1, var2); break;
        case V_SgSubtractOp: op = buildSubtractOp(var1, var2); break;
        case V_SgMultiplyOp: op = buildMultiplyOp(var1, var2); break;
        case V_SgDivideOp: op = buildDivideOp(var1, var2); break;
    }
    
    // The rest of the assignment
    SgExprListExp *exprList = buildExprListExp(op);
    SgVarRefExp *va = buildVarRefExp(name, new_block);
    SgExprStatement *expr = buildAssignStatement(va, exprList);
    appendStatement(expr, new_block);
    
    nameStack->push(name);
}

void omp_simd_build_3addr(SgExpression *rval, SgBasicBlock *new_block, std::stack<std::string> *nameStack,
                            std::vector<std::string> loop_indexes, SgType *type) {
    switch (rval->variantT()) {
        case V_SgAddOp:
        case V_SgSubtractOp:
        case V_SgMultiplyOp:
        case V_SgDivideOp: {
            // Build math
            SgBinaryOp *op = static_cast<SgBinaryOp *>(rval);
            omp_simd_build_3addr(op->get_lhs_operand(), new_block, nameStack, loop_indexes, type);
            omp_simd_build_3addr(op->get_rhs_operand(), new_block, nameStack, loop_indexes, type);
            omp_simd_build_math(new_block, nameStack, rval->variantT(), type);
        } break;
        
        case V_SgPntrArrRefExp: {
            omp_simd_build_ptr_assign(rval, new_block, nameStack, loop_indexes, type);
        } break;
        
        case V_SgVarRefExp:
        case V_SgIntVal: 
        case V_SgFloatVal: 
        case V_SgDoubleVal: {
            omp_simd_build_scalar_assign(rval, new_block, nameStack, type);
        } break;
        
        case V_SgCastExp: {
            SgCastExp *cast = static_cast<SgCastExp *>(rval);
            omp_simd_build_scalar_assign(cast->get_operand(), new_block, nameStack, type);
        } break;

        default: {
            // TODO: Invalid node
        }
    }
}

// Get the maximum variable from the for loop test expression
// This is needed for multi-dimension loop transformation
std::string omp_simd_max_var(SgStatement *test_stmt) {
    SgExpression *test_expr = static_cast<SgExprStatement *>(test_stmt)->get_expression();

    if (!isSgBinaryOp(test_expr)) {
        std::cout << "Invalid test (for-loop max_var)" << std::endl;
        return "";
    }
    
    SgBinaryOp *op = static_cast<SgBinaryOp *>(test_expr);
    SgExpression *rval = op->get_rhs_operand();
    
    if (isSgBinaryOp(rval)) {
        op = static_cast<SgBinaryOp *>(rval);
        if (!isSgVarRefExp(op->get_lhs_operand())) {
            return "";
        }
        
        SgVarRefExp *var_ref = static_cast<SgVarRefExp *>(op->get_lhs_operand());
        return var_ref->get_symbol()->get_name();
    } else if (isSgVarRefExp(rval)) {
        SgVarRefExp *var_ref = static_cast<SgVarRefExp *>(rval);
        return var_ref->get_symbol()->get_name();
    }

    return "";
}

// This runs the first pass of the SIMD lowering. This pass converts multi-dimensional arrays
// and then converts the statements to 3-address scalar code
//
// The main purpose of this function is to break each expression between the load and store
void omp_simd_pass1(SgForStatement *for_loop, SgBasicBlock *new_block) {
    
    // Get the size from the for loop (we will need this for converting multi-dimensional loops)
    SgStatement *stmt = for_loop;
    std::vector<std::string> loop_indexes;
    
    do {
        SgForStatement *for_loop2 = static_cast<SgForStatement *>(stmt);
        std::string index = omp_simd_max_var(for_loop2->get_test());
        loop_indexes.push_back(index);
        
        stmt = getEnclosingScope(stmt);
    
        if (stmt->variantT() == V_SgBasicBlock) {
            stmt = getEnclosingScope(stmt);
        }
        
    } while (stmt->variantT() == V_SgForStatement);

    // Get the loop body
    SgStatement *loop_body = getLoopBody(for_loop);
    Rose_STL_Container<SgNode *> bodyList = NodeQuery::querySubTree(loop_body, V_SgExprStatement);
    
    for (Rose_STL_Container<SgNode *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        SgExpression *expr = static_cast<SgExprStatement *>((*i))->get_expression();

        Rose_STL_Container<SgNode *> exprBody = NodeQuery::querySubTree(expr, V_SgExpression);
        SgNode *line = exprBody.front();
        
        if (!isSgBinaryOp(line)) {
            std::cout << "Invalid assignment." << std::endl;
            continue;
        }
        
        // debug
        // TODO: remove
        //printAST(line);
        
        SgBinaryOp *op = static_cast<SgBinaryOp *>(line);
        std::stack<std::string> nameStack;
        
        // Copy the store expression and determine the proper type
        SgExpression *orig = static_cast<SgExpression *>(op->get_lhs_operand());
        SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(copyExpression(orig));
        SgType *type = array->get_type();
        
        switch (type->variantT()) {
            case V_SgTypeInt: type = buildIntType(); break;
            case V_SgTypeFloat: type = buildFloatType(); break;
            case V_SgTypeDouble: type = buildDoubleType(); break;
        }
        
        // Build the rval (the expression)
        omp_simd_build_3addr(op->get_rhs_operand(), new_block, &nameStack, loop_indexes, type);
        
        // Build the lval (the store/assignment)
        std::string name = nameStack.top();

        SgVarRefExp *var = buildVarRefExp(name, new_block);
        SgExprStatement *storeExpr = buildAssignStatement(array, var);
        appendStatement(storeExpr, new_block);
    }
}

////////////////////////////////////////////////////////////////////////////////////
// This is all the Pass-2 code
// Pass-2 converts the scalar functions to SIMD IR

void omp_simd_build_ir(SgBasicBlock *new_block, SgBinaryOp *assign_stmt) {
    // Determine the type first
    SgType *type = assign_stmt->get_type();
    
    SgExpression *lval = assign_stmt->get_lhs_operand();
    if (isSgPntrArrRefExp(lval)) {
        // Final store
    } else {
        SgVarRefExp *old_var = static_cast<SgVarRefExp *>(lval);
        std::string name = old_var->get_symbol()->get_name();
        
        SgVarRefExp *vector_var = buildVarRefExp(name, new_block);
        SgExprStatement *expr;

        SgExpression *rval = assign_stmt->get_rhs_operand();
        if (rval == nullptr) return;
        
        switch (rval->variantT()) {
            case V_SgPntrArrRefExp: {
                SgSIMDLoad *ld = new SgSIMDLoad(vector_var, assign_stmt->get_rhs_operand(), type, NULL);
                expr = buildExprStatement(ld);
                appendStatement(expr, new_block);
            } break;

            // Ordinarily, this should be only one long
            /*case V_SgExprListExp: {
                SgExprListExp *list = static_cast<SgExprListExp *>(rval);
                Rose_STL_Container<SgExpression *> expressions = list->get_expressions();

                for (Rose_STL_Container<SgExpression *>::iterator i = expressions.begin(); i != expressions.end(); i++) {
                    if (!isSgBinaryOp(*i)) continue;
                    SgBinaryOp *op1 = static_cast<SgBinaryOp *>(*i);
                    SgSIMDBinaryOp *op = NULL;
                    
                    switch ((*i)->variantT()) {
                        case V_SgAddOp: op = new SgSIMDAddOp(NULL, NULL); break;
                        //case V_SgMultiplyOp: op = new SgSIMDMultiplyOp(NULL, NULL); break;
                    }

                    if (op == NULL) continue;

                    op->set_lhs_operand(deepCopy(op1->get_lhs_operand()));
                    op->set_rhs_operand(deepCopy(op1->get_rhs_operand()));
                    
                    expr = buildAssignStatement(vector_var, op);
                    appendStatement(expr, new_block);
                }
            } break;*/
        }

        /*if (expr != nullptr)
            appendStatement(expr, new_block);*/
    }
}

void omp_simd_pass2(SgBasicBlock *old_block, SgBasicBlock *new_block) {
    Rose_STL_Container<SgStatement *> bodyList = old_block->get_statements();
    
    for (Rose_STL_Container<SgStatement *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        switch ((*i)->variantT()) {
            case V_SgVariableDeclaration: {
                SgStatement *vd = deepCopy(*i);
                appendStatement(vd, new_block);
            } break;
            
            case V_SgExprStatement: {
                SgExprStatement *expr = static_cast<SgExprStatement *>(*i);
                SgBinaryOp *assign_stmt = static_cast<SgBinaryOp *>(expr->get_expression());
                omp_simd_build_ir(new_block, assign_stmt);
            } break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
// The final conversion step (Intel)

SgType *omp_simd_get_intel_type(SgType *type, SgBasicBlock *new_block) {
    SgType *vector_type;
        
    switch (type->variantT()) {
        case V_SgTypeInt: vector_type = buildOpaqueType("__m256i", new_block); break;
        case V_SgTypeFloat: vector_type = buildOpaqueType("__m256", new_block); break;
        case V_SgTypeDouble: vector_type = buildOpaqueType("__m256d", new_block); break;
    }

    return vector_type;
}

void omp_simd_write_intel(SgBasicBlock *old_block, SgBasicBlock *new_block) {
    Rose_STL_Container<SgStatement *> bodyList = old_block->get_statements();

    for (Rose_STL_Container<SgStatement *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        switch ((*i)->variantT()) {
            // With variable declarations, we have to convert to the appropriate type
            case V_SgVariableDeclaration: {
                SgVariableDeclaration *old_vd = static_cast<SgVariableDeclaration *>(*i);
                SgVariableDeclaration *vd = deepCopy(old_vd);

                Rose_STL_Container<SgInitializedName *> vars = vd->get_variables();
                for (Rose_STL_Container<SgInitializedName *>::iterator j = vars.begin(); j != vars.end(); j++) {
                    SgType *vector_type = omp_simd_get_intel_type((*j)->get_type(), new_block);
                    (*j)->set_type(vector_type);
                }
                
                appendStatement(vd, new_block);
            } break;
            
            case V_SgExprStatement: {
                SgExprStatement *expr = static_cast<SgExprStatement *>(*i);
                SgBinaryOp *simd_stmt = static_cast<SgBinaryOp *>(expr->get_expression());
                
                switch (simd_stmt->variantT()) {
                    //SIMD Load
                    case V_SgSIMDLoad: {
                        SgSIMDLoad *ld = static_cast<SgSIMDLoad *>(simd_stmt);
                        //SgType *vector_type = omp_simd_get_intel_type(ld->get_type(), new_block);
                        SgType *vector_type = buildOpaqueType("__m256", new_block);
                        
                        SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(ld->get_rhs_operand());
                        SgAddressOfOp *addr = buildAddressOfOp(array);
                        SgExprListExp *parameters = buildExprListExp(addr);
                        
                        SgVarRefExp *va = static_cast<SgVarRefExp *>(deepCopy(ld->get_lhs_operand()));
                        
                        /*if (new_block == NULL) std::cout << "NULL!!" << std::endl;
                        SgExpression *ld_fc = buildFunctionCallExp("_mm256_loadu_ps", vector_type, NULL, new_block);*/
                        
                        SgFunctionCallExp *ld_fc = buildFunctionCallExp("foo2", vector_type, NULL, new_block);
                        
                        SgExprStatement *expr = buildAssignStatement(va, ld_fc);
                        appendStatement(expr, new_block);
                    } break;
                }
            } break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
// The entry point to the SIMD analyzer

void OmpSupport::transOmpSimd(SgNode *node, SgSourceFile *file) {
    // Insert the needed headers
    insertHeader(file, "immintrin.h", true, true);
    
    // Make sure the tree is correct
    SgOmpSimdStatement *target = isSgOmpSimdStatement(node);
    ROSE_ASSERT(target != NULL);

    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);

    SgStatement *body =  target->get_body();
    ROSE_ASSERT(body != NULL);

    SgForStatement *for_loop = isSgForStatement(body);
    ROSE_ASSERT(for_loop != NULL);
    SageInterface::forLoopNormalization(for_loop);

    // Update the loop increment
    /*SgExpression *inc = for_loop->get_increment();

    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(inc, V_SgExpression);
    SgIntVal *inc_amount = isSgIntVal(nodeList.at(2));
    ROSE_ASSERT(inc_amount != NULL);
    inc_amount->set_value(8);*/

    // Now work on the body
    SgBasicBlock *new_block = SageBuilder::buildBasicBlock();
    SgBasicBlock *new_block2 = SageBuilder::buildBasicBlock();
    //SgBasicBlock *new_block3 = SageBuilder::buildBasicBlock();
    
    omp_simd_pass1(for_loop, new_block);
    omp_simd_pass2(new_block, new_block2);
    //omp_simd_write_intel(new_block2, new_block3);
    
    //printAST(new_block);
    
    // debug
    // TODO remove
    SgStatement *loop_body = getLoopBody(for_loop);
    replaceStatement(loop_body, new_block, true);       // Change to block3
}

