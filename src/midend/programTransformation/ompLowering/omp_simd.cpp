
#include <iostream>
#include <stack>
#include <stdio.h>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "omp_lowering.h"

using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;

// TODO: We may eventually want this in a separate header
extern void omp_simd_write_intel(SgOmpSimdStatement *target, SgForStatement *for_loop, Rose_STL_Container<SgNode *> *ir_block);

////////////////////////////////////////////////////////////////////////////////////
// This is all the Pass-1 code

// For generating names
int name_pos = 0;

std::string simdGenName(bool is_ptr = false) {
    char str[5];
    sprintf(str, "%d", name_pos);
    
    std::string prefix = "__vec";
    if (is_ptr) prefix = "__ptr";
    
    std::string name = prefix + std::string(str);
    ++name_pos;
    return name;
}

SgPntrArrRefExp *omp_simd_convert_ptr(SgExpression *pntr_exp, SgBasicBlock *new_block) {
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
        SgVarRefExp *lastPntr;
        
        // Iterate down the stack
        while (stack.size() > 1) {
            SgExpression *index = stack.top();
            stack.pop();
            
            //SgType *baseType = buildFloatType();
            SgType *baseType = pntr->get_type()->findBaseType();
            SgType *type = buildPointerType(baseType);
            
            std::string name = simdGenName(true);
            SgVariableDeclaration *vd = buildVariableDeclaration(name, type, NULL, new_block);
            appendStatement(vd, new_block);
            
            SgVarRefExp *va = buildVarRefExp(name, new_block);
            lastPntr = va;
            
            SgPntrArrRefExp *pntr = buildPntrArrRefExp(copyExpression(lval), index);
            SgExprStatement *expr = buildAssignStatement(va, pntr);
            appendStatement(expr, new_block);
        }
        
        SgExpression *last_index = stack.top();
        stack.pop();
        
        array = buildPntrArrRefExp(lastPntr, last_index);
    }
    
    return array;
}

void omp_simd_build_ptr_assign(SgExpression *pntr_exp, SgBasicBlock *new_block, std::stack<std::string> *nameStack, SgType *type) {
    SgPntrArrRefExp *array = omp_simd_convert_ptr(pntr_exp, new_block);
    
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
    bool is_const = true;
    
    switch (node->variantT()) {
        case V_SgIntVal: {
            SgIntVal *val = static_cast<SgIntVal *>(node);
            
            if (type->variantT() == V_SgTypeInt) expr = buildIntVal(val->get_value());
            else if (type->variantT() == V_SgTypeDouble) expr = buildDoubleVal(val->get_value());
            else expr = buildFloatVal(val->get_value());
        } break;
        
        case V_SgFloatVal: {
            SgFloatVal *val = static_cast<SgFloatVal *>(node);
            
            if (type->variantT() == V_SgTypeInt) expr = buildIntVal(val->get_value());
            else if (type->variantT() == V_SgTypeDouble) expr = buildDoubleVal(val->get_value());
            else expr = buildFloatVal(val->get_value());
        } break;
        
        case V_SgDoubleVal: {
            SgDoubleVal *val = static_cast<SgDoubleVal *>(node);
            
            if (type->variantT() == V_SgTypeInt) expr = buildIntVal(val->get_value());
            else if (type->variantT() == V_SgTypeDouble) expr = buildDoubleVal(val->get_value());
            else expr = buildFloatVal(val->get_value());
        } break;
        
        case V_SgVarRefExp: {
            expr = copyExpression(node);
            is_const = false;
        } break;
        
        default: {}
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
        default: {}
    }
    
    // The rest of the assignment
    SgExprListExp *exprList = buildExprListExp(op);
    SgVarRefExp *va = buildVarRefExp(name, new_block);
    SgExprStatement *expr = buildAssignStatement(va, exprList);
    appendStatement(expr, new_block);
    
    nameStack->push(name);
}

void omp_simd_build_3addr(SgExpression *rval, SgBasicBlock *new_block, std::stack<std::string> *nameStack, SgType *type) {
    switch (rval->variantT()) {
        case V_SgAddOp:
        case V_SgSubtractOp:
        case V_SgMultiplyOp:
        case V_SgDivideOp: {
            // Build math
            SgBinaryOp *op = static_cast<SgBinaryOp *>(rval);
            omp_simd_build_3addr(op->get_lhs_operand(), new_block, nameStack, type);
            omp_simd_build_3addr(op->get_rhs_operand(), new_block, nameStack, type);
            omp_simd_build_math(new_block, nameStack, rval->variantT(), type);
        } break;
        
        case V_SgPntrArrRefExp: {
            omp_simd_build_ptr_assign(rval, new_block, nameStack, type);
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

// This runs the first pass of the SIMD lowering. This pass converts multi-dimensional arrays
// and then converts the statements to 3-address scalar code
//
// The main purpose of this function is to break each expression between the load and store
void omp_simd_pass1(SgForStatement *for_loop, SgBasicBlock *new_block) {

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
        
        // Expand +=
        if (isSgPlusAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgAddOp *add = buildAddOp(copyExpression(op->get_lhs_operand()), expr);
            op->set_rhs_operand(add);
            
        // -=
        } else if (isSgMinusAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgSubtractOp *add = buildSubtractOp(copyExpression(op->get_lhs_operand()), expr);
            op->set_rhs_operand(add);
            
        // *=
        } else if (isSgMultAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgMultiplyOp *add = buildMultiplyOp(copyExpression(op->get_lhs_operand()), expr);
            op->set_rhs_operand(add);
            
        // /=
        } else if (isSgDivAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgDivideOp *add = buildDivideOp(copyExpression(op->get_lhs_operand()), expr);
            op->set_rhs_operand(add);
        }
        
        // Copy the store expression and determine the proper type
        SgExpression *orig = static_cast<SgExpression *>(op->get_lhs_operand());
        SgExpression *dest;
        SgType *type;
        
        if (orig->variantT() == V_SgVarRefExp) {
            SgVarRefExp *var = static_cast<SgVarRefExp *>(copyExpression(orig));
            type = var->get_type();
            dest = var;
        } else {
            SgPntrArrRefExp *array = static_cast<SgPntrArrRefExp *>(copyExpression(orig));
            type = array->get_type();
            dest = omp_simd_convert_ptr(copyExpression(orig), new_block);
        }
        
        switch (type->variantT()) {
            case V_SgTypeInt: type = buildIntType(); break;
            case V_SgTypeFloat: type = buildFloatType(); break;
            case V_SgTypeDouble: type = buildDoubleType(); break;
            default: {}
        }
        
        // Build the rval (the expression)
        omp_simd_build_3addr(op->get_rhs_operand(), new_block, &nameStack, type);
        
        // Build the lval (the store/assignment)
        std::string name = nameStack.top();

        SgVarRefExp *var = buildVarRefExp(name, new_block);
        SgExprStatement *storeExpr = buildAssignStatement(dest, var);
        appendStatement(storeExpr, new_block);
    }
}

////////////////////////////////////////////////////////////////////////////////////
// Pass 2-> Convert to SIMD IR

bool omp_simd_is_load_operand(VariantT val) {
    switch (val) {
        case V_SgPntrArrRefExp:
        case V_SgVarRefExp:
        case V_SgIntVal: 
        case V_SgFloatVal: 
        case V_SgDoubleVal: return true;
        default: return false;
    }
    
    return false;
}

// The main pass2 loop
void omp_simd_pass2(SgBasicBlock *old_block, Rose_STL_Container<SgNode *> *ir_block) {
    Rose_STL_Container<SgStatement *> bodyList = old_block->get_statements();
    
    for (Rose_STL_Container<SgStatement *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        if ((*i)->variantT() != V_SgExprStatement) {
            continue;
        }
        
        SgExprStatement *expr_statement = static_cast<SgExprStatement *>(*i);
        if (!isSgBinaryOp(expr_statement->get_expression())) {
            continue;
        }
        
        // Build the IR
        // The left-value will be the variable reference, which will be translated to a vector variable declaration
        // The right-value is either a pointer reference or a math expression
        SgBinaryOp *assign_stmt = static_cast<SgBinaryOp *>(expr_statement->get_expression());
        SgExpression *lval = assign_stmt->get_lhs_operand();
        SgExpression *rval = assign_stmt->get_rhs_operand();
        
        // Load
        if (lval->variantT() == V_SgVarRefExp && rval->variantT() == V_SgPntrArrRefExp) {
            SgVarRefExp *lvar = static_cast<SgVarRefExp *>(lval);
            if (lvar->get_type()->variantT() == V_SgPointerType) {
                ir_block->push_back(deepCopy(assign_stmt));
            } else {
                SgSIMDLoad *ld = buildBinaryExpression<SgSIMDLoad>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(ld);
            }
            
        // This could be a broadcast or a scalar store
        } else if (lval->variantT() == V_SgVarRefExp && rval->variantT() == V_SgVarRefExp) {
            SgVarRefExp *lvar = static_cast<SgVarRefExp *>(lval);
            std::string name = lvar->get_symbol()->get_name().getString();
            
            if (name.rfind("__vec", 0) == 0) {
                SgSIMDBroadcast *ld = buildBinaryExpression<SgSIMDBroadcast>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(ld);
            } else {
                SgSIMDScalarStore *str = buildBinaryExpression<SgSIMDScalarStore>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(str);
            }
            
        // Broadcast
        // TODO: This is not the most elegent piece of code...
        } else if (lval->variantT() == V_SgVarRefExp &&
                    (/*rval->variantT() == V_SgVarRefExp ||*/ rval->variantT() == V_SgIntVal
                    || rval->variantT() == V_SgFloatVal || rval->variantT() == V_SgDoubleVal)) {
            SgSIMDBroadcast *ld = buildBinaryExpression<SgSIMDBroadcast>(deepCopy(lval), deepCopy(rval));
            ir_block->push_back(ld);
            
        // Store
        } else if (lval->variant() == V_SgPntrArrRefExp && rval->variantT() == V_SgVarRefExp) {
            SgSIMDStore *str = buildBinaryExpression<SgSIMDStore>(deepCopy(lval), deepCopy(rval));
            ir_block->push_back(str);
            
        // Math
        } else if (lval->variantT() == V_SgVarRefExp && rval->variantT() == V_SgExprListExp) {
            SgExprListExp *expr_list = static_cast<SgExprListExp *>(rval);
            SgExpression *first = expr_list->get_expressions().front();
            if (!isSgBinaryOp(first)) {
                continue;
            }
            
            SgVarRefExp *dest = static_cast<SgVarRefExp *>(lval);
            SgBinaryOp *math_stmt = static_cast<SgBinaryOp *>(first);
            lval = math_stmt->get_lhs_operand();
            rval = math_stmt->get_rhs_operand();
            
            if (!isSgVarRefExp(lval) || !isSgVarRefExp(rval)) {
                std::cout << "Error: invalid arguments in math expression." << std::endl;
                continue;
            }
            
            SgSIMDBinaryOp *math = NULL;
            SgExprListExp *parameters = buildExprListExp(deepCopy(lval), deepCopy(rval));
            
            switch (math_stmt->variantT()) {
                case V_SgAddOp: math = buildBinaryExpression<SgSIMDAddOp>(dest, parameters); break;
                case V_SgSubtractOp: math = buildBinaryExpression<SgSIMDSubOp>(dest, parameters); break;
                case V_SgMultiplyOp: math = buildBinaryExpression<SgSIMDMulOp>(dest, parameters); break;
                case V_SgDivideOp: math = buildBinaryExpression<SgSIMDDivOp>(dest, parameters); break;
                default: std::cout << "Error: Invalid math." << std::endl;
            }
            
            if (math != NULL) ir_block->push_back(math);
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

    // Now work on the body. Run the first two passes
    SgBasicBlock *new_block = SageBuilder::buildBasicBlock();
    Rose_STL_Container<SgNode *> *ir_block = new Rose_STL_Container<SgNode *>();
    
    omp_simd_pass1(for_loop, new_block);
    omp_simd_pass2(new_block, ir_block);
    
    // Uncomment to test the 3-address translation
    //SgStatement *loop_body = getLoopBody(for_loop);
    //replaceStatement(loop_body, new_block, true);
    
    // Set the new block, and convert to Intel intrinsics
    omp_simd_write_intel(target, for_loop, ir_block);
    
    replaceStatement(target, for_loop);
}

