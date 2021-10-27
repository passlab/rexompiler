
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
    
    // Setup the parameters and create the new outlined function
    std::string wrapper_name = "outlined_func";
    SgInitializedName *gtid = buildInitializedName("__global_tid", buildIntType());
    SgInitializedName *task = buildInitializedName("task", buildOpaqueType("ptask", g_scope));
    
    SgFunctionParameterList *params = buildFunctionParameterList(gtid, task);
    SgFunctionDeclaration *funcDec = buildDefiningFunctionDeclaration(wrapper_name, buildVoidType(), params, g_scope);
    
    SgFunctionDefinition *funcDef = funcDec->get_definition();
    ROSE_ASSERT(funcDef != NULL);
    
    appendStatement(funcDec, g_scope);
    
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
    
    SgVarRefExp *lhs = static_cast<SgVarRefExp *>(expr->get_lhs_operand());
    std::string name = lhs->get_symbol()->get_name();
    SgVarRefExp *destRef = buildOpaqueVarRefExp(name, g_scope);
    
    arrow2 = buildArrowExp(sharedsRef, destRef);
    arrow1 = buildArrowExp(taskRef, arrow2);
    SgPointerDerefExp *deref = buildPointerDerefExp(arrow1);
    
    SgExprStatement *assignBody = buildAssignStatement(deref, rhs);
    funcDef->append_statement(assignBody);
    
    outlined_function_list->push_back(isSgFunctionDeclaration(funcDec));
    
    // Create the function call
    ASTtools::VarSymSet_t syms;
    SgExprListExp* parameters =  NULL;
    SgExpression * parameter_data = NULL;
    SgExpression * parameter_cpyfn = NULL;
    SgExpression * parameter_arg_size = NULL;
    SgExpression * parameter_arg_align = NULL;
    SgExpression * parameter_if_clause =  NULL;
    SgExpression * parameter_untied = NULL;
    SgExpression * parameter_argcount = NULL;
    size_t parameter_count = syms.size();
    
    if (parameter_count == 0) // No parameters to be passed at all
    {
        parameter_data = buildIntVal(0);
        parameter_cpyfn=buildIntVal(0); // no copy function is needed
        parameter_arg_size = buildIntVal(0);
        parameter_arg_align = buildIntVal(0);
    }
    else
    {
        SgVarRefExp * data_ref = buildVarRefExp(wrapper_name, g_scope);
        ROSE_ASSERT (data_ref != NULL);
        SgType * data_type = data_ref->get_type();
        parameter_data =  buildAddressOfOp(data_ref);
        parameter_cpyfn=buildIntVal(0); // no special copy function for array of pointers
        // arg size of array of pointers = pointer_count * pointer_size
        // ROSE does not support cross compilation so sizeof(void*) can use as a workaround for now
        //we now use a structure containing pointers or non-pointer typed members to wrap parameters
        parameter_arg_size =  buildSizeOfOp(data_type);
        //  parameter_arg_size = buildIntVal( parameter_count* sizeof(void*));
        //  TODO get right alignment
        parameter_arg_align = buildIntVal(4);
        //parameter_arg_align = buildIntVal(sizeof(void*));
    }
    
    if (hasClause(target, V_SgOmpIfClause)) {
        Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpIfClause);
        ROSE_ASSERT (clauses.size() ==1); // should only have one if ()
        SgOmpIfClause * if_clause = isSgOmpIfClause (clauses[0]);
        ROSE_ASSERT (if_clause->get_expression() != NULL);
        parameter_if_clause = copyExpression(if_clause->get_expression());
    } else {
        parameter_if_clause = buildIntVal(1);
    }

    if (hasClause(target, V_SgOmpUntiedClause))
        parameter_untied = buildIntVal(1);
    else
        parameter_untied = buildIntVal(0);
        
    parameters = buildExprListExp(buildFunctionRefExp(funcDec),
                                parameter_data, parameter_cpyfn, parameter_arg_size,
                                parameter_arg_align, parameter_if_clause, parameter_untied);
    
    SgExprStatement *s1 = buildFunctionCallStmt(wrapper_name, buildVoidType(), parameters, g_scope);
    SageInterface::replaceStatement(target, s1, true);
}
