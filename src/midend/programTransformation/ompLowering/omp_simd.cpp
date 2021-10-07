
#include <iostream>
#include <stack>
#include <stdio.h>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "omp_lowering.h"
#include "omp_simd.h"

using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;

SimdType simd_arch = Intel_AVX512;

////////////////////////////////////////////////////////////////////////////////////
// This is all the Pass-1 code

// For generating names
int name_pos = 0;

std::string simdGenName(int type = 0) {
    char str[5];
    sprintf(str, "%d", name_pos);
    
    std::string prefix = "__vec";
    if (type == 1) prefix = "__ptr";
    else if (type == 2) prefix = "__part";
    
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
        SgVarRefExp *lastPntr = NULL;
        
        // Iterate down the stack
        while (stack.size() > 1) {
            SgExpression *index = stack.top();
            stack.pop();
            
            //SgType *baseType = buildFloatType();
            SgType *baseType = pntr->get_type()->findBaseType();
            SgType *type = buildPointerType(baseType);
            
            std::string name = simdGenName(1);
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
    if (node->variantT() == V_SgVarRefExp) {
        SgVarRefExp *ref = static_cast<SgVarRefExp *>(node);
        std::string name = ref->get_symbol()->get_name().getString();
        
        if (name.rfind("__part", 0) == 0) {
            nameStack->push(name);
            return;
        }
    }

    std::string name = simdGenName();
    nameStack->push(name);

    // Build the assignment
    SgExpression *expr = NULL;
    
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
        } break;
        
        default: {//expr = copyExpression(node);
            expr = buildIntVal(0);}
    }
    
    std::cout << std::endl;
    
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
    SgBinaryOp *op = NULL;
    
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

// This scans an OMP SIMD statement for a reduction clause containing a variable matching that
// of the parameter.
// If it is found, the modifier (operator) is converted to a char for easier processing, and
// returned
//
char omp_simd_get_reduction_mod(SgOmpSimdStatement *target, SgVarRefExp *var) {
    SgOmpClausePtrList clauses = target->get_clauses();
    for (size_t i = 0; i<clauses.size(); i++) {
        if (clauses.at(i)->variantT() != V_SgOmpReductionClause)
            continue;
    
        SgOmpReductionClause *rc = static_cast<SgOmpReductionClause *>(clauses.at(i));
        SgExpressionPtrList vars = rc->get_variables()->get_expressions();
        bool found = false;
        
        for (size_t j = 0; j<vars.size(); j++) {
            if (vars.at(j)->variantT() != V_SgVarRefExp)
                continue;
                
            SgVarRefExp *v_current = static_cast<SgVarRefExp *>(vars.at(j));
            if (v_current->get_symbol()->get_name() == var->get_symbol()->get_name()) {
                found = true;
                break;
            }
        }
        
        if (!found) continue;
        
        SgOmpClause::omp_reduction_identifier_enum omp_op = rc->get_identifier();
        switch (omp_op) {
            case SgOmpClause::e_omp_reduction_plus: return '+';
            case SgOmpClause::e_omp_reduction_minus: return '-';
            case SgOmpClause::e_omp_reduction_mul: return '*';
            default: return 0;
        }
    }
    
    return 0;
}

// This runs the first pass of the SIMD lowering. This pass converts multi-dimensional arrays
// and then converts the statements to 3-address scalar code
//
// The main purpose of this function is to break each expression between the load and store
bool omp_simd_pass1(SgOmpSimdStatement *target, SgForStatement *for_loop, SgBasicBlock *new_block, bool isArm = false) {

    // Get the loop body
    SgStatement *loop_body = getLoopBody(for_loop);
    Rose_STL_Container<SgNode *> bodyList = NodeQuery::querySubTree(loop_body, V_SgExprStatement);
    
    for (Rose_STL_Container<SgNode *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        SgExpression *expr = static_cast<SgExprStatement *>((*i))->get_expression();

        Rose_STL_Container<SgNode *> exprBody = NodeQuery::querySubTree(expr, V_SgExpression);
        SgNode *line = exprBody.front();
        
        if (!isSgBinaryOp(line)) {
            std::cout << "Invalid assignment." << std::endl;
            return false;
        }
        
        SgBinaryOp *op = static_cast<SgBinaryOp *>(line);
        std::stack<std::string> nameStack;
        
        // Copy the store expression and determine the proper type
        SgExpression *orig = static_cast<SgExpression *>(op->get_lhs_operand());
        SgExpression *dest;
        SgType *type;
        
        char reduction_mod = 0;
        bool need_partial = false;
        bool armReduction = false;
        
        // If we have a variable, we need to indicate a partial sum variable
        // These are prefixed with __part, and in this step, they are simply assigned
        // Like this: __part0 = scalar;
        if (orig->variantT() == V_SgVarRefExp) {
            SgVarRefExp *var = static_cast<SgVarRefExp *>(copyExpression(orig));
            type = var->get_type();
            
            // Make sure we have a valid reduction clause
            reduction_mod = omp_simd_get_reduction_mod(target, var);
            if (reduction_mod == 0) {
                return false;
            } else {
                if (reduction_mod == '+' && isArm) {
                    dest = orig;
                    armReduction = true;
                } else {
                    need_partial = true;
                    dest = var;
                }
            }
            
        // Otherwise, we just have a conventional store
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
        
        std::string partial_vec = "";
        if (need_partial) {
            partial_vec = simdGenName(2);
            SgVariableDeclaration *vd = buildVariableDeclaration(partial_vec, type, NULL, new_block);
            appendStatement(vd, new_block);
        }
        
        SgExpression *lhs = copyExpression(op->get_lhs_operand());
        if (partial_vec != "") lhs = buildVarRefExp(partial_vec, new_block);
        
        // Expand +=
        if (isSgPlusAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgAddOp *add = buildAddOp(lhs, expr);
            op->set_rhs_operand(add);
            
        // -=
        } else if (isSgMinusAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgSubtractOp *add = buildSubtractOp(lhs, expr);
            op->set_rhs_operand(add);
            
        // *=
        } else if (isSgMultAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgMultiplyOp *add = buildMultiplyOp(lhs, expr);
            op->set_rhs_operand(add);
            
        // /=
        } else if (isSgDivAssignOp(op)) {
            SgExpression *expr = static_cast<SgExpression *>(op->get_rhs_operand());
            SgDivideOp *add = buildDivideOp(lhs, expr);
            op->set_rhs_operand(add);
        }
        
        if (op->get_rhs_operand()->variantT() == V_SgAddOp && armReduction) {
            SgAddOp *addOP = static_cast<SgAddOp *>(op->get_rhs_operand());
            omp_simd_build_3addr(addOP->get_rhs_operand(), new_block, &nameStack, type);
            
            std::string name = nameStack.top();
            SgVarRefExp *var = buildVarRefExp(name, new_block);
            SgExprStatement *storeExpr = buildExprStatement(buildBinaryExpression<SgPlusAssignOp>(dest, var));
            appendStatement(storeExpr, new_block);
            
            return true;
        }
        
        // Build the rval (the expression)
        omp_simd_build_3addr(op->get_rhs_operand(), new_block, &nameStack, type);
        
        // Build the lval (the store/assignment)
        std::string name = nameStack.top();
        
        // If application, build the reduction assignment
        if (need_partial) {
            std::string dest_vec = name;
            name = partial_vec;
            
            SgVarRefExp *va = buildVarRefExp(partial_vec, new_block);
            SgVarRefExp *vec = buildVarRefExp(dest_vec, new_block);
            
            SgExprListExp *exprList = buildExprListExp(vec);
            SgExprStatement *assign = buildAssignStatement(va, vec);
            appendStatement(assign, new_block);
        }

        SgVarRefExp *var = buildVarRefExp(name, new_block);
        SgExprStatement *storeExpr = buildAssignStatement(dest, var);
        appendStatement(storeExpr, new_block);
    }
    
    return true;
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
                SgPntrArrRefExp *pntr = static_cast<SgPntrArrRefExp *>(rval);
                if (pntr->get_rhs_operand()->variantT() == V_SgPntrArrRefExp) {
                    SgSIMDGather *ld = buildBinaryExpression<SgSIMDGather>(deepCopy(lval), deepCopy(rval));
                    ir_block->push_back(ld);
                } else {
                    SgSIMDLoad *ld = buildBinaryExpression<SgSIMDLoad>(deepCopy(lval), deepCopy(rval));
                    ir_block->push_back(ld);
                }
            }
            
        // This could be a broadcast or a scalar store
        } else if (lval->variantT() == V_SgVarRefExp && rval->variantT() == V_SgVarRefExp) {
            SgVarRefExp *lvar = static_cast<SgVarRefExp *>(lval);
            std::string name = lvar->get_symbol()->get_name().getString();
            
            // If the variable name starts with __part, we store to a partial sums register
            if (name.rfind("__part", 0) == 0) {
                SgSIMDPartialStore *str = buildBinaryExpression<SgSIMDPartialStore>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(str);
                
            // If the variable name starts with __vec, we broadcast
            } else if (name.rfind("__vec", 0) == 0) {
                SgSIMDBroadcast *ld = buildBinaryExpression<SgSIMDBroadcast>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(ld);
                
            // Otherwise, we have a scalar store
            } else {
                if (expr_statement->get_expression()->variantT() == V_SgPlusAssignOp) {
                    SgSIMDSVAddV *addv = buildBinaryExpression<SgSIMDSVAddV>(deepCopy(lval), deepCopy(rval));
                    ir_block->push_back(addv);
                } else {
                    SgSIMDScalarStore *str = buildBinaryExpression<SgSIMDScalarStore>(deepCopy(lval), deepCopy(rval));
                    ir_block->push_back(str);
                }
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
            SgPntrArrRefExp *pntr = static_cast<SgPntrArrRefExp *>(lval);
            
            if (pntr->get_rhs_operand()->variantT() == V_SgPntrArrRefExp) {
                SgSIMDScatter *str = buildBinaryExpression<SgSIMDScatter>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(str);
            } else {
                SgSIMDStore *str = buildBinaryExpression<SgSIMDStore>(deepCopy(lval), deepCopy(rval));
                ir_block->push_back(str);
            }
            
        // Math
        } else if (lval->variantT() == V_SgVarRefExp && rval->variantT() == V_SgExprListExp) {
            SgExprListExp *expr_list = static_cast<SgExprListExp *>(rval);
            SgExpression *first = expr_list->get_expressions().front();
            if (!isSgBinaryOp(first)) {
                //SgSIMDPartialStore *str = buildBinaryExpression<SgSIMDPartialStore>(deepCopy(lval), deepCopy(rval));
                //ir_block->push_back(str);
                //ir_block->push_back(deepCopy(*i));
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
            
            // First, check to see if its a reduction assignment
            std::string name = dest->get_symbol()->get_name().getString();
            if (name.rfind("__part", 0) == 0) {
                SgSIMDPartialStore *str = buildBinaryExpression<SgSIMDPartialStore>(deepCopy(lval), NULL);
                ir_block->push_back(str);
            }
            
            // Then, build math
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
        } else {
            std::cout << "??" << std::endl;
        }
    }
}

// Scans an OMP SIMD statement for a simdlen clause
// If none is provided, we return -1, which means the compiler should use the default length
// If we return -2, an error has occurred
int omp_simd_get_simdlen(SgOmpSimdStatement *target, bool safelen) {
    SgOmpClausePtrList clauses = target->get_clauses();
    for (size_t i = 0; i<clauses.size(); i++) {
        if (safelen) {
            if (clauses.at(i)->variantT() != V_SgOmpSafelenClause)
                continue;
        } else {
            if (clauses.at(i)->variantT() != V_SgOmpSimdlenClause)
                continue;
        }
        
        SgOmpExpressionClause *sl_clause = static_cast<SgOmpExpressionClause *>(clauses.at(i));
        SgExpression *sl_expr = sl_clause->get_expression();
        
        if (sl_expr->variantT() != V_SgIntVal) {
            std::cout << "Error: SIMDLEN value must currently only be an integer." << std::endl;
            return -1;
        }
        
        int val = static_cast<SgIntVal *>(sl_expr)->get_value();
        if (val < 0) {
            std::cout << "Error: SIMDLEN and SAFELEN >= 0" << std::endl;
            return -1;
        }
        
        return val;
    }
    
    return -1;
}

// Determines the SIMD length we should use
// Returning 0 means to use default
//
// This is platform-specific. Currently, I haven't decided what to with Arm yet. For intel:
// <= 4 -> SSE (Not sure why someone would want to use this...)
// > 4 <= 8 -> AVX2
// > 8 <= 16 -> AVX-512
// For now, ignore anything greater than 16. At some point, we may want to implement some kind of lowering
int omp_simd_get_length(SgOmpSimdStatement *target) {
    int simdlen = omp_simd_get_simdlen(target, false);
    int safelen = omp_simd_get_simdlen(target, true);
    
    if (simdlen < 0 && safelen < 0) {
        return 0;
    }
    
    if (simdlen >= safelen && safelen != -1) {
        simdlen = safelen;
    }
    
    if (simd_arch == Intel_AVX512) {
        if (simdlen <= 4) return 4;
        else if (simdlen > 4 && simdlen <= 8) return 8;
        return 16;
    } else if (simd_arch == Arm_SVE2) {
        return 0;
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// The entry point to the SIMD analyzer

void OmpSupport::transOmpSimd(SgNode *node, SgSourceFile *file) {
    if (simd_arch == Nothing) {
        return;
    }

    // Insert the needed headers
    //insertHeader(file, "immintrin.h", true, true);
    //insertHeader(file, "arm_sve.h", true, true);
    
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
    
    bool isArm = false;
    if (simd_arch == ArmAddr3 || simd_arch == Arm_SVE2) isArm = true;
    
    if (!omp_simd_pass1(target, for_loop, new_block, isArm)) {
        delete ir_block;
        return;
    }
    
    omp_simd_pass2(new_block, ir_block);
    
    // Output the final result
    if (simd_arch == Addr3 || simd_arch == ArmAddr3) {
        SgStatement *loop_body = getLoopBody(for_loop);
        replaceStatement(loop_body, new_block, true);
    } else {
        if (simd_arch == Intel_AVX512) {
            int simd_length = omp_simd_get_length(target);
            if (simd_length > 0) {
                std::cout << "Using SIMD Length of: " << simd_length << std::endl;
            }
            
            insertHeader(file, "immintrin.h", true, true);
            omp_simd_write_intel(target, for_loop, ir_block, simd_length);
        } else if (simd_arch == Arm_SVE2) {
            insertHeader(file, "arm_sve.h", true, true);
            omp_simd_write_arm(target, for_loop, ir_block);
        }
        
        replaceStatement(target, for_loop);
    }
}

