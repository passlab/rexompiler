
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

extern std::vector<SgFunctionDeclaration* >* outlined_function_list;

//! Translate omp task
void OmpSupport::transOmpTask(SgNode* node) {
    ROSE_ASSERT(node != NULL);
    SgOmpTaskStatement* target = isSgOmpTaskStatement(node);
    ROSE_ASSERT (target != NULL);
    
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
  
    SgGlobal* g_scope = SageInterface::getGlobalScope(body);
    ROSE_ASSERT(g_scope != NULL);
    
    // We don't actually use this outlined function, but for some reason we need it to make sure
    // we have all the correct parameters
    //
    // TODO: Can we do something more efficient?
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
    
    funcDef->get_body()->get_statements().clear();
    
    // Start with the body
    // First line: int n = task->shareds->n;
    SgVarRefExp *taskRef = buildVarRefExp("task", g_scope);
    SgVarRefExp *sharedsRef = buildVarRefExp("shareds", g_scope);
    SgVarRefExp *nRef = buildOpaqueVarRefExp("n", g_scope);
    
    SgArrowExp *arrow2 = buildArrowExp(sharedsRef, nRef);
    SgArrowExp *arrow1 = buildArrowExp(taskRef, arrow2);
    
    SgAssignInitializer *init = buildAssignInitializer(arrow1, buildIntType());
    SgVariableDeclaration *n = buildVariableDeclaration("n", buildIntType(), init, funcDef);
    funcDef->append_statement(n);
    
    // Now the body
    SgExprStatement *exprStmt = static_cast<SgExprStatement *>(body);
    SgAssignOp *expr = static_cast<SgAssignOp *>(exprStmt->get_expression());
    SgExpression *rhs = deepCopy(expr->get_rhs_operand());
    
    SgVarRefExp *lhs;
    if (expr->get_lhs_operand()->variantT() == V_SgPointerDerefExp) {
        SgPointerDerefExp *deref = static_cast<SgPointerDerefExp *>(expr->get_lhs_operand());
        lhs = static_cast<SgVarRefExp *>(deref->get_operand_i());
    } else {
        lhs = static_cast<SgVarRefExp *>(expr->get_lhs_operand());
    }
    ROSE_ASSERT(lhs != NULL);
    std::string name = lhs->get_symbol()->get_name();
    SgVarRefExp *destRef = buildOpaqueVarRefExp(name, g_scope);
    
    arrow2 = buildArrowExp(sharedsRef, destRef);
    arrow1 = buildArrowExp(taskRef, arrow2);
    SgPointerDerefExp *deref = buildPointerDerefExp(arrow1);
    
    SgExprStatement *assignBody = buildAssignStatement(deref, rhs);
    funcDef->append_statement(assignBody);
    
    outlined_function_list->push_back(isSgFunctionDeclaration(outlined_func));
    
    ///////////////////////////////////
    // Now build the body
    ///////////////////////////////////
    //
    SgBasicBlock *block = buildBasicBlock();
    SageInterface::replaceStatement(target, block, true);
    ROSE_ASSERT(outlined_func->get_args().size() > 2);
    
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
    
    // task = (ptask)__kmpc_omp_task_alloc(NULL, gtid, 1, sizeof(struct task),
    //                      sizeof(struct shar), &OUT__1__3690__fib__14__);
    taskRef = buildVarRefExp("task", g_scope);
    
    SgSizeOfOp *taskSizeOf = buildSizeOfOp(buildOpaqueType("task", g_scope));    // TODO: I think this is wrong
    SgSizeOfOp *sharSizeOf = buildSizeOfOp(buildOpaqueType("shar", g_scope));    // TODO: And I think this is wrong
    SgFunctionRefExp *outlinedRef = buildFunctionRefExp(outlined_func);
    SgAddressOfOp *outlinedAddr = buildAddressOfOp(outlinedRef);
    parameters = buildExprListExp(nullRef, gtidRef, buildIntVal(1), taskSizeOf, sharSizeOf, outlinedAddr);
    
    SgFunctionCallExp *fcTaskAlloc = buildFunctionCallExp("__kmpc_omp_task_alloc", buildPointerType(buildVoidType()), parameters, g_scope);
    SgCastExp *taskCastExp = buildCastExp(fcTaskAlloc, buildOpaqueType("ptask", g_scope));
    SgExprStatement *taskAllocAssign = buildAssignStatement(taskRef, taskCastExp);
    trueBlock->append_statement(taskAllocAssign);
    
    // task->shareds->n = n;
    // task->shareds->x = x;
    SgVarRefExp *xRef = buildOpaqueVarRefExp("x", g_scope);
    nRef = buildOpaqueVarRefExp("n", g_scope);
    sharedsRef = buildOpaqueVarRefExp("shareds", g_scope);
    taskRef = buildOpaqueVarRefExp("task", g_scope);
    
    arrow2 = buildArrowExp(sharedsRef, nRef);
    arrow1 = buildArrowExp(taskRef, arrow2);
    SgPointerDerefExp *nDeref = buildPointerDerefExp(nRef);
    SgExprStatement *nArrowAssign = buildAssignStatement(arrow1, nDeref);
    trueBlock->append_statement(nArrowAssign);
    
    arrow2 = buildArrowExp(sharedsRef, xRef);
    arrow1 = buildArrowExp(taskRef, arrow2);
    SgPointerDerefExp *xDeref = buildPointerDerefExp(xRef);
    SgExprStatement *xArrowAssign = buildAssignStatement(arrow1, xDeref);
    trueBlock->append_statement(xArrowAssign);
    
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
}
