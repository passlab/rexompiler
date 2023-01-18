
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "Outliner.hh"
#include "omp_lowering.h"
#include "RoseAst.h"
#include <sstream>
#include "rex_llvm.h"

using namespace std;
using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

extern std::vector<SgFunctionDeclaration* >* target_outlined_function_list;
extern std::vector<SgDeclarationStatement *>* target_outlined_struct_list;

//! Translate omp task
void OmpSupport::transOmpTask(SgNode* node) {
    ROSE_ASSERT(node != NULL);
    SgOmpTaskStatement* target = isSgOmpTaskStatement(node);
    ROSE_ASSERT (target != NULL);
    
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
  
    SgGlobal* g_scope = SageInterface::getGlobalScope(body);
    ROSE_ASSERT(g_scope != NULL);
    
    // Make sure the rex_kmp.h header is included
    SgSourceFile *file = getEnclosingSourceFile(target);
    insertHeader(file, "rex_kmp.h", false);
    
    ////////////////////////////////////////////////
    //
    // First, we need to query arguments.
    // Start with the original function
    //
    std::vector<std::string> originalVarRefs;
    std::map<std::string, SgType *> varRefTypeMap;
    SgFunctionDeclaration *originalDec = getEnclosingFunctionDeclaration(target);
    //for (SgInitializedName *arg : originalDec->get_args()) {
    for (size_t i = 2; i<originalDec->get_args().size(); i++) {
        SgInitializedName *arg = originalDec->get_args().at(i);
        std::string name = arg->get_name();
        originalVarRefs.push_back(name);
        varRefTypeMap[name] = arg->get_type();
        /*if (isSgPointerType(arg->get_type())) {
            varRefTypeMap[name] = arg->get_type()->findBaseType();
        }*/
    }
    
    // Now get the shared variables
    std::vector<std::string> sharedVarRefs = originalVarRefs;
    /*for (SgOmpClause *clause : target->get_clauses()) {
        if (clause->variantT() != V_SgOmpSharedClause) continue;
        
        SgOmpSharedClause *shared = static_cast<SgOmpSharedClause *>(clause);
        for (SgExpression *expr : shared->get_variables()->get_expressions()) {
            if (expr->variantT() != V_SgVarRefExp) continue;
            SgVarRefExp *varRef = static_cast<SgVarRefExp *>(expr);
            std::string name = varRef->get_symbol()->get_name();
            sharedVarRefs.push_back(name);
            varRefTypeMap[name] = varRef->get_type();
        }
    }*/
    
    //
    // Create the needed structures for this
    //
    // The shareds structure
    SgClassDeclaration *strPshareds = buildStructDeclaration("shar", g_scope);
    SgClassDefinition *strPsharedsDef = buildClassDefinition(strPshareds);
    
    for (std::string varName : sharedVarRefs) {
        //SgPointerType *type = buildPointerType(varRefTypeMap[varName]);
        //SgVariableDeclaration *varD = buildVariableDeclaration(varName, type, NULL, strPsharedsDef);
        SgVariableDeclaration *varD = buildVariableDeclaration(varName, varRefTypeMap[varName], NULL, strPsharedsDef);
        appendStatement(varD, strPsharedsDef);
    }
    
    SgTypedefDeclaration *tyPshareds = buildTypedefDeclaration("pshareds", buildPointerType(strPshareds->get_type()), g_scope, true);
    tyPshareds->set_declaration(strPshareds);
    
    // The task structure
    SgClassDeclaration *taskStruct = buildStructDeclaration("task", g_scope);
    SgClassDefinition *taskStructDef = buildClassDefinition(taskStruct);
    
    SgType *sharedType = buildOpaqueType("pshareds", g_scope);
    SgVariableDeclaration *sharedPtr = buildVariableDeclaration("shareds", sharedType, NULL, taskStructDef);
    appendStatement(sharedPtr, taskStructDef);
    
    SgPointerType *dummyPtrType = buildPointerType(buildVoidType());
    SgVariableDeclaration *dummyPtr = buildVariableDeclaration("dummy", dummyPtrType, NULL, taskStructDef);
    appendStatement(dummyPtr, taskStructDef);
    
    SgTypedefDeclaration *taskTypedef = buildTypedefDeclaration("ptask", buildPointerType(taskStruct->get_type()), g_scope, true);
    taskTypedef->set_declaration(taskStruct);
    
    // Insert them all
    SgStatement *firstStatement = getFirstStatement(g_scope);
    insertStatementAfter(firstStatement, strPshareds);
    insertStatementAfter(strPshareds, tyPshareds);
    insertStatementAfter(tyPshareds, taskStruct);
    insertStatementAfter(taskStruct, taskTypedef);
    
    // Insert the function forward declarations
    // TODO: This should probably be in a header somewhere
    
    // We don't actually use this outlined function, but for some reason we need it to make sure
    // we have all the correct parameters
    //
    AttachedPreprocessingInfoType save_buf1, save_buf2;
    cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
    cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;
    std::string wrapper_name;
    ASTtools::VarSymSet_t syms;
    ASTtools::VarSymSet_t pdSyms3; // store all variables which should be passed by reference
    SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, wrapper_name, syms, pdSyms3, true);
    std::string outlinedName = outlined_func->get_name();
    
    SgFunctionDefinition *funcDef = outlined_func->get_definition();
    ROSE_ASSERT(funcDef != NULL);
    
    /*auto of_arg1 = outlined_func->get_args().at(0);
    auto of_arg2 = outlined_func->get_args().at(1);
    outlined_func->get_args().clear();
    outlined_func->get_args().push_back(of_arg1);
    outlined_func->get_args().push_back(of_arg2);*/
    
    funcDef->get_body()->get_statements().clear();
    
    /*
    // TODO: DELETE
    SgFunctionDefinition *funcDef = getEnclosingFunctionDefinition(node);
    ROSE_ASSERT(funcDef != NULL);
    SgFunctionDeclaration *outlined_func = funcDef->get_declaration();
    ROSE_ASSERT(outlined_func != NULL);
    // END DELETE
    */
    
    // Start with the body
    // First line: int *n = task->shareds->n;
    
    SgVarRefExp *taskRef = buildOpaqueVarRefExp("task", g_scope);
    SgVarRefExp *sharedsRef = buildOpaqueVarRefExp("shareds", g_scope);
    SgArrowExp *arrow2, *arrow1;
    
    for (std::string varName : originalVarRefs) {
        SgVarRefExp *nRef = buildOpaqueVarRefExp(varName, g_scope);
        
        arrow2 = buildArrowExp(taskRef, sharedsRef);
        arrow1 = buildArrowExp(arrow2, nRef);
        
        std::string varName1 = varName + "1";
        SgAssignInitializer *init = buildAssignInitializer(arrow1, buildPointerType(buildIntType()));
        SgVariableDeclaration *n = buildVariableDeclaration(varName1, buildPointerType(buildIntType()), init, funcDef);
        funcDef->append_statement(n);
    }
    
    // Now the body
    SgExprStatement *exprStmt = static_cast<SgExprStatement *>(body);
    SgAssignOp *expr = static_cast<SgAssignOp *>(exprStmt->get_expression());
    SgExpression *rhs = deepCopy(expr->get_rhs_operand());
    
    std::function<void(SgExpression*, SgScopeStatement*)> fix_names;
    fix_names = [rhs, &fix_names](SgExpression *input, SgScopeStatement *g_scope) {
        if (input->variantT() == V_SgVarRefExp) {
            SgVarRefExp *ref = isSgVarRefExp(input);
            std::string name = ref->get_symbol()->get_name() + "p__1";
            SgVarRefExp *newRef = buildVarRefExp(name, g_scope);
            replaceExpression(ref, newRef);
        
        } else if (isSgPointerDerefExp(input)) {
            SgPointerDerefExp *deref = isSgPointerDerefExp(input);
            if (deref->get_operand()->variantT() == V_SgPointerDerefExp) {
                SgExpression *exp = deepCopy(deref->get_operand());
                replaceExpression(deref, exp);
                fix_names(exp, g_scope);
            } else {
                fix_names(deref->get_operand(), g_scope);
            }
              
        } else if (isSgBinaryOp(input)) {
            SgBinaryOp *op = static_cast<SgBinaryOp *>(input);
            fix_names(op->get_lhs_operand(), g_scope);
            fix_names(op->get_rhs_operand(), g_scope);
            
        } else if (isSgUnaryOp(input)) {
            SgUnaryOp *op = static_cast<SgUnaryOp *>(input);
            fix_names(op->get_operand(), g_scope);
            
        } else if (isSgFunctionCallExp(input)) {
            SgFunctionCallExp *fc = isSgFunctionCallExp(input);
            auto args = fc->get_args();
            for (auto exp : args->get_expressions()) {
                fix_names(exp, g_scope);
            }
        }
    };
    fix_names(rhs, g_scope);
    
    SgVarRefExp *lhs;
    if (expr->get_lhs_operand()->variantT() == V_SgPointerDerefExp) {
        SgPointerDerefExp *deref = static_cast<SgPointerDerefExp *>(expr->get_lhs_operand());
        // TODO: We need better control
        deref = static_cast<SgPointerDerefExp *>(deref->get_operand_i());
        lhs = static_cast<SgVarRefExp *>(deref->get_operand_i());
    } else {
        lhs = static_cast<SgVarRefExp *>(expr->get_lhs_operand());
    }
    ROSE_ASSERT(lhs != NULL);
    std::string name = lhs->get_symbol()->get_name() + "p__";
    SgVarRefExp *destRef = buildOpaqueVarRefExp(name, g_scope);
    
    arrow2 = buildArrowExp(taskRef, sharedsRef);
    arrow1 = buildArrowExp(arrow2, destRef);
    SgPointerDerefExp *deref = buildPointerDerefExp(arrow1);
    
    SgExprStatement *assignBody = buildAssignStatement(deref, rhs);
    funcDef->append_statement(assignBody);
    
    //target_outlined_function_list->push_back(isSgFunctionDeclaration(outlined_func));
    
    ///////////////////////////////////
    // Now build the body
    ///////////////////////////////////
    //
    SgBasicBlock *block = buildBasicBlock();
    SageInterface::replaceStatement(target, block, true);
    //ROSE_ASSERT(outlined_func->get_args().size() > 2);
    
    // int gtid = *__global_tid;
    SgInitializedName *arg1 = outlined_func->get_args().at(0);
    SgVarRefExp *arg1Ref = buildOpaqueVarRefExp(arg1->get_name(), g_scope);
    
    SgPointerDerefExp *gtidDeref = buildPointerDerefExp(arg1Ref);
    SgAssignInitializer *gtidInit = buildAssignInitializer(gtidDeref, buildIntType());
    SgVariableDeclaration *gtid = buildVariableDeclaration("gtid", buildIntType(), gtidInit, g_scope);
    block->append_statement(gtid);
    
    // if (__kmpc_single(NULL, gtid)) { ... }
    SgVarRefExp *nullRef = buildOpaqueVarRefExp("NULL", g_scope);
    SgVarRefExp *gtidRef = buildOpaqueVarRefExp("gtid", g_scope);
    SgExprListExp *parameters = buildExprListExp(nullRef, gtidRef);
    SgFunctionCallExp *fcSingle = buildFunctionCallExp("__kmpc_single", buildIntType(), parameters, g_scope);
    
    SgBasicBlock *trueBlock = buildBasicBlock();
    SgIfStmt *ifStmt = buildIfStmt(fcSingle, trueBlock, NULL);
    block->append_statement(ifStmt);
    
    // ptask task;
    // pshareds psh;
    SgVariableDeclaration *taskDef = buildVariableDeclaration("task", buildOpaqueType("ptask", g_scope), NULL, g_scope);
    SgVariableDeclaration *pshDef = buildVariableDeclaration("psh", buildOpaqueType("pshareds", g_scope), NULL, g_scope);
    trueBlock->append_statement(taskDef);
    trueBlock->append_statement(pshDef);
    
    // task = (ptask)__kmpc_omp_task_alloc(NULL, gtid, 1, sizeof(task) * 4,
    //                      sizeof(psh) * 2, &OUT__1__3690__fib__14__);
    taskRef = buildVarRefExp("task", g_scope);
    
    // TODO: This needs to be changed. It should be sizeof the types
    SgSizeOfOp *taskSizeOf = buildSizeOfOp(buildOpaqueType("task", g_scope));
    SgMultiplyOp *taskSizeMul = buildMultiplyOp(taskSizeOf, buildIntVal(4));
    
    SgSizeOfOp *sharSizeOf = buildSizeOfOp(buildOpaqueType("psh", g_scope));
    SgMultiplyOp *sharSizeMul = buildMultiplyOp(sharSizeOf, buildIntVal(4));
    
    SgFunctionRefExp *outlinedRef = buildFunctionRefExp(outlined_func->get_name(), g_scope);
    SgAddressOfOp *outlinedAddr = buildAddressOfOp(outlinedRef);
    parameters = buildExprListExp(nullRef, gtidRef, buildIntVal(1), taskSizeMul, sharSizeMul, outlinedAddr);
    
    SgFunctionCallExp *fcTaskAlloc = buildFunctionCallExp("__kmpc_omp_task_alloc", buildPointerType(buildVoidType()), parameters, g_scope);
    SgCastExp *taskCastExp = buildCastExp(fcTaskAlloc, buildOpaqueType("ptask", g_scope));
    SgExprStatement *taskAllocAssign = buildAssignStatement(taskRef, taskCastExp);
    trueBlock->append_statement(taskAllocAssign);
    
    // task->shareds->n = n;
    // task->shareds->x = x;
    arrow2 = buildArrowExp(taskRef, sharedsRef);
    for (std::string varName : sharedVarRefs) {
        SgVarRefExp *varRef = buildOpaqueVarRefExp(varName, g_scope);
        
        arrow1 = buildArrowExp(arrow2, varRef);
        SgExprStatement *arrowAssign = buildAssignStatement(arrow1, varRef);
        trueBlock->append_statement(arrowAssign);
    }
    
    // __kmpc_omp_task(NULL, gtid, task);
    parameters = buildExprListExp(nullRef, gtidRef, taskRef);
    SgExprStatement *ompTask = buildFunctionCallStmt("__kmpc_omp_task", buildVoidType(), parameters, g_scope);
    trueBlock->append_statement(ompTask);
    
    // Move everything inside the body of the if statement
    std::vector<SgStatement *> toMove;
    SgStatement *nextStmt = SageInterface::getNextStatement(block);
    while (nextStmt != nullptr) {
        toMove.push_back(nextStmt);
        nextStmt = SageInterface::getNextStatement(nextStmt);
    }
    
    for (SgStatement *stmt : toMove) {
        trueBlock->append_statement(deepCopy(stmt));
        removeStatement(stmt);
    }
    
    // __kmpc_omp_taskwait(NULL, gtid);
    // __kmpc_end_single(NULL, gtid);
    parameters = buildExprListExp(nullRef, gtidRef);
    SgExprStatement *taskWait = buildFunctionCallStmt("__kmpc_omp_taskwait", buildVoidType(), parameters, g_scope);
    trueBlock->append_statement(taskWait);
    
    parameters = buildExprListExp(nullRef, gtidRef);
    SgExprStatement *endSingle = buildFunctionCallStmt("__kmpc_end_single", buildVoidType(), parameters, g_scope);
    trueBlock->append_statement(endSingle);
    
    // __kmpc_barrier(NULL, gtid);
    // This goes outside the if-statement just after it
    parameters = buildExprListExp(nullRef, gtidRef);
    SgExprStatement *barrierFc = buildFunctionCallStmt("__kmpc_barrier", buildVoidType(), parameters, g_scope);
    block->append_statement(barrierFc);
    
    //
    // Check for return statements
    //
    SgFunctionDefinition *parentDec = getEnclosingFunctionDefinition(block);
    Rose_STL_Container<SgNode *> bodyList = NodeQuery::querySubTree(parentDec->get_body(), V_SgStatement);
    for (Rose_STL_Container<SgNode *>::iterator i = bodyList.begin(); i != bodyList.end(); i++) {
        SgStatement *stmt = static_cast<SgStatement *>((*i));
        if (stmt->variantT() != V_SgIfStmt) {
            continue;
        }
        
        Rose_STL_Container<SgNode *> ifStmtBody = NodeQuery::querySubTree(stmt, V_SgStatement);   
        for (Rose_STL_Container<SgNode *>::iterator j = ifStmtBody.begin(); j != ifStmtBody.end(); j++) {
            SgStatement *stmt2 = static_cast<SgStatement *>((*j));
            if (stmt2->variantT() != V_SgReturnStmt) {
                continue;
            }
                
            SgReturnStmt *ret = buildReturnStmt();
            replaceStatement(stmt2, ret);
        } 
    }
    //*/
    
    //SgReturnStmt *ret = buildReturnStmt();
    //replaceStatement(isSgStatement(node), buildReturnStmt());
}

