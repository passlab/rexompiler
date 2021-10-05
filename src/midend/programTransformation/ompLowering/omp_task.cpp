
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

//! Translate omp task
/*
The translation of omp task is similar to the one for omp parallel
Please remember to call patchUpFirstprivateVariables() before this function to make implicit firstprivate
variables explicit.

The gomp runtime function for omp task is:
extern void GOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *), long arg_size, long arg_align, bool if_clause, unsigned flags)
    1. void (*fn) (void *): the generated outlined function for the task body
    2. void *data: the parameters for the outlined function
    3. void (*cpyfn) (void *, void *): copy function to replace the default memcpy() from function data to each task's data
    4. long arg_size: specify the size of data
    5. long arg_align: alignment of the data
    6. bool if_clause: the value of if_clause. true --> 1, false -->0; default is set to 1 by GCC
    7. unsigned flags: untied (1) or not (0)

Since we use the ROSE outliner to generate the outlined function. The parameters are wrapped into an array of pointers to them
So the calculation of data(parameter) size/align is simplified . They are all pointer types.
*/
void OmpSupport::transOmpTask(SgNode* node)
{
  ROSE_ASSERT(node != NULL);
  SgOmpTaskStatement* target = isSgOmpTaskStatement(node);
  ROSE_ASSERT (target != NULL);

  // Liao 1/24/2011
  // For Fortran code, we have to insert EXTERNAL OUTLINED_FUNC into
  // the function body containing the parallel region
  // TODO verify this is also necessary for OMP TASK
  SgFunctionDefinition * func_def = NULL;
  if (SageInterface::is_Fortran_language() )
  {
    func_def = getEnclosingFunctionDefinition(target);
    ROSE_ASSERT (func_def != NULL);
  }

  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);
  // Save preprocessing info as early as possible, avoiding mess up from the outliner
  AttachedPreprocessingInfoType save_buf1, save_buf2;
  cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
  cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;

  // generate and insert an outlined function as a task
  std::string wrapper_name;
  ASTtools::VarSymSet_t syms;
  ASTtools::VarSymSet_t pdSyms3; // store all variables which should be passed by reference
  std::set<SgInitializedName*> readOnlyVars;
  SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, wrapper_name, syms, pdSyms3);

  if (SageInterface::is_Fortran_language() )
  { // EXTERNAL outlined_function , otherwise the function name will be interpreted as a integer/real variable
    ROSE_ASSERT (func_def != NULL);
    // There could be an enclosing parallel region
    //SgBasicBlock * func_body = func_def->get_body();
    SgBasicBlock * enclosing_body = getEnclosingRegionOrFuncDefinition (target);
    ROSE_ASSERT (enclosing_body != NULL);
    SgAttributeSpecificationStatement* external_stmt1 = buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_externalStatement);
    SgFunctionRefExp *func_ref1 = buildFunctionRefExp (outlined_func);
    external_stmt1->get_parameter_list()->prepend_expression(func_ref1);
    func_ref1->set_parent(external_stmt1->get_parameter_list());
    // must put it into the declaration statement part, after possible implicit/include statements, if any
    SgStatement* l_stmt = findLastDeclarationStatement (enclosing_body);
    if (l_stmt)
      insertStatementAfter(l_stmt,external_stmt1);
    else
      prependStatement(external_stmt1, enclosing_body);
  }

  SgScopeStatement * p_scope = target->get_scope();
  ROSE_ASSERT(p_scope != NULL);
  // Generate a call to it

  SgExprListExp* parameters =  NULL;
  //SgStatement* func_call = Outliner::generateCall (outlined_func, syms, readOnlyVars, wrapper_name,p_scope);
  //ROSE_ASSERT(func_call != NULL);

  // Replace the parallel region with the function call statement
  // TODO should we introduce another level of scope here?
  // SageInterface::replaceStatement(target,func_call, true);
  // hide this from the unparser TODO this call statement is not really necessary, only the call expression is needed
  //  Sg_File_Info* file_info = type_decl->get_file_info();
  //      file_info->unsetOutputInCodeGeneration ();
  //
  //func_call->get_file_info()->unsetOutputInCodeGeneration ();
  SgExpression * parameter_data = NULL;
  SgExpression * parameter_cpyfn = NULL;
  SgExpression * parameter_arg_size = NULL;
  SgExpression * parameter_arg_align = NULL;
  SgExpression * parameter_if_clause =  NULL;
  SgExpression * parameter_untied = NULL;
  SgExpression * parameter_argcount = NULL;
  size_t parameter_count = syms.size();

  if (SageInterface::is_Fortran_language())
  { // Fortran case
  //  void xomp_task (void (*fn) (void *), void (*cpyfn) (void *, void *), int * arg_size, int * arg_align,
  //                  int * if_clause, int * untied, int * argcount, ...)

      parameter_cpyfn=buildIntVal(0); // no special copy function for array of pointers
      parameter_arg_size = buildIntVal( parameter_count * sizeof(void*) );
      //  TODO get right alignment
      parameter_arg_align = buildIntVal(4);
  }
  else // C/C++ case
  //  void GOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *), long arg_size, long arg_align,
  //                  bool if_clause, unsigned flags)
  {
    if ( parameter_count == 0) // No parameters to be passed at all
    {
      parameter_data = buildIntVal(0);
      parameter_cpyfn=buildIntVal(0); // no copy function is needed
      parameter_arg_size = buildIntVal(0);
      parameter_arg_align = buildIntVal(0);
    }
    else
    {
      SgVarRefExp * data_ref = buildVarRefExp(wrapper_name, p_scope);
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
  }

  if (hasClause(target, V_SgOmpIfClause))
  {
    Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpIfClause);
    ROSE_ASSERT (clauses.size() ==1); // should only have one if ()
    SgOmpIfClause * if_clause = isSgOmpIfClause (clauses[0]);
    ROSE_ASSERT (if_clause->get_expression() != NULL);
    parameter_if_clause = copyExpression(if_clause->get_expression());
  }
  else
    parameter_if_clause = buildIntVal(1);

  if (hasClause(target, V_SgOmpUntiedClause))
    parameter_untied = buildIntVal(1);
  else
    parameter_untied = buildIntVal(0);

  // parameters are different between Fortran and C/C++
  // To support pass-by-value and pass-by-reference in the XOMP runtime
  // We use a triplet for each parameter to be passed to XOMP
  // <pass_by_value-ref, value-size, parameter-address>
  // e.g. if a integer i is intended to be passed by value in the task
  //   we generate three argument for it: 1, sizeof(int), i
  // similarly, for an array item[10], passed by reference in the task
  //   we generate: 0, sizeof(void*), item
  //   As a result, the variable length argument list is 3 times the count of original parameters long
  if (SageInterface::is_Fortran_language())
  {
    parameters = buildExprListExp(buildFunctionRefExp(outlined_func),
    parameter_cpyfn, parameter_arg_size, parameter_arg_align, parameter_if_clause, parameter_untied);

    parameter_argcount =  buildIntVal (syms.size()*3);
    appendExpression (parameters,parameter_argcount);
    ASTtools::VarSymSet_t::iterator iter = syms.begin();
    for (; iter!=syms.end(); iter++)
    {
      const SgVariableSymbol * sb = *iter;
      bool b_pass_value = true;
      // Assumption:
      //   transOmpVariables() should already handled most private, reduction variables
      //    Anything left should be passed by reference by default , except for loop index variables.
      // We check if a variable is a loop index, and pass it by value.
      //
      // TODO more accurate way to decide on pass-by-value or pass-by-reference in patchUpPrivateVariables()
      //    and patchUpFirstprivateVariables()
      if (isLoopIndexVariable (sb->get_declaration(), target))
      {
        b_pass_value = true;
        appendExpression (parameters,buildIntVal(1));
      }
      else
      { // all other should be treated as shared variables ( pass-by-reference )
        b_pass_value = false;
        appendExpression (parameters,buildIntVal(0));
      }

      //2nd of the triplet, the size of the parameter type,
      // if pass-by-value, the actual size
      // if pass-by-reference, the pointer size
      if (b_pass_value)
      { //TODO accurate calculation of type size for Fortran, assume integer for now
         // Provide an interface function for this.
        // Is it feasible to calculate all sizes during compilation time ??
        SgType * s_type = sb->get_type();
        if (isSgTypeInt(s_type))
          appendExpression (parameters,buildIntVal(sizeof(int)));
        else
        {
          printf("Error. transOmpTask(): unhandled Fortran type  (%s) for pass-by-value.\n",s_type->class_name().c_str());
          ROSE_ASSERT (false);
        }
      }
      else
      { // get target platform's pointer size
        appendExpression (parameters,buildIntVal(sizeof(void*)));
      }

      // the third of the triplet
      appendExpression (parameters, buildVarRefExp(const_cast<SgVariableSymbol *>(sb)));
    }
  }
  else
  {
    parameters = buildExprListExp(buildFunctionRefExp(outlined_func),
    parameter_data, parameter_cpyfn, parameter_arg_size, parameter_arg_align, parameter_if_clause, parameter_untied);
  }

#ifdef ENABLE_XOMP
  SgExprStatement * s1 = buildFunctionCallStmt("XOMP_task", buildVoidType(), parameters, p_scope);
#else
  SgExprStatement * s1 = buildFunctionCallStmt("GOMP_task", buildVoidType(), parameters, p_scope);
#endif
  SageInterface::replaceStatement(target,s1, true);

  // Keep preprocessing information
  // I have to use cut-paste instead of direct move since
  // the preprocessing information may be moved to a wrong place during outlining
  // while the destination node is unknown until the outlining is done.
  pastePreprocessingInfo(s1, PreprocessingInfo::before, save_buf1);
  pastePreprocessingInfo(s1, PreprocessingInfo::after, save_buf2);
}
  