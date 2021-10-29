
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
    
    // Setup the new body
    //SgBasicBlock *newBlock = buildBasicBlock(n, assignBody);
    //funcDef->set_body(newBlock);
    
    outlined_function_list->push_back(isSgFunctionDeclaration(outlined_func));
    
    // Create the function call
    SgExprListExp *parameters = buildExprListExp();
    SgExprStatement *s1 = buildFunctionCallStmt(wrapper_name, buildVoidType(), parameters, g_scope);
    SageInterface::replaceStatement(target, s1, true);
}
