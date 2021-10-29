
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
    
    // Create the function call
    SgBasicBlock *block = buildBasicBlock();
    SageInterface::replaceStatement(target, block, true);
    
    ///////////////////////////////////
    // Now build the body
    ///////////////////////////////////
    //
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
    
    // Create the function call
    parameters = buildExprListExp();
    SgExprStatement *s1 = buildFunctionCallStmt(outlinedName, buildVoidType(), parameters, g_scope);
    trueBlock->append_statement(s1);
}
