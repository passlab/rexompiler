#include "sage3basic.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

namespace OmpSupport {

Rose_STL_Container<SgNode *>
mergeSgNodeList(Rose_STL_Container<SgNode *> node_list1,
                Rose_STL_Container<SgNode *> node_list2) {

  std::sort(node_list1.begin(), node_list1.end());
  std::sort(node_list2.begin(), node_list2.end());
  Rose_STL_Container<SgNode *> node_list;
  std::merge(node_list1.begin(), node_list1.end(), node_list2.begin(),
             node_list2.end(),
             std::insert_iterator<Rose_STL_Container<SgNode *>>(
                 node_list, node_list.end()));
  return node_list;
}

void analyzeOmpMetadirective(SgNode *node) {
  ROSE_ASSERT(node != NULL);
  SgOmpMetadirectiveStatement *target = isSgOmpMetadirectiveStatement(node);

  ROSE_ASSERT(target != NULL);

  SgFunctionDefinition *func_def = NULL;
  if (SageInterface::is_Fortran_language()) {
    func_def = getEnclosingFunctionDefinition(target);
    ROSE_ASSERT(func_def != NULL);
  }
  SgStatement *body = target->get_body();
  ROSE_ASSERT(body != NULL);
  AttachedPreprocessingInfoType save_buf1, save_buf2, save_buf_inside;
  cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1);
  cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2);

  cutPreprocessingInfo(target, PreprocessingInfo::inside, save_buf_inside);
  std::cout << "Metadirective IR is caught.\n";

  SgIfStmt *root_if_statement = NULL;
  SgStatement *variant_directive;
  SgStatement *variant_body = copyStatement(body);
  ROSE_ASSERT(variant_body != NULL);
  SgIfStmt *if_stmt = NULL;
  SgIfStmt *previous_if_stmt = NULL;
  if (hasClause(target, V_SgOmpWhenClause)) {
    Rose_STL_Container<SgOmpClause *> clauses =
        getClause(target, V_SgOmpWhenClause);
    SgOmpWhenClause *when_clause = isSgOmpWhenClause(clauses[0]);
    SgExpression *condition_expression = when_clause->get_user_condition();
    SgExprStatement *condition_statement =
        buildExprStatement(condition_expression);
    variant_directive = when_clause->get_variant_directive();
    ((SgOmpBodyStatement *)variant_directive)->set_body(variant_body);
    setOneSourcePositionForTransformation(variant_directive);
    if (variant_directive) {
      variant_body->set_parent(variant_directive);
      if_stmt = buildIfStmt(condition_statement, variant_directive, body);
    } else {
      if_stmt = buildIfStmt(condition_statement, variant_body, body);
    }
    root_if_statement = if_stmt;
    for (unsigned int i = 1; i < clauses.size(); i++) {
      previous_if_stmt = if_stmt;
      when_clause = isSgOmpWhenClause(clauses[i]);
      condition_expression = when_clause->get_user_condition();
      condition_statement = buildExprStatement(condition_expression);
      variant_directive = when_clause->get_variant_directive();
      variant_directive->set_parent(target->get_parent());
      variant_body = copyStatement(body);
      ((SgOmpBodyStatement *)variant_directive)->set_body(variant_body);
      setOneSourcePositionForTransformation(variant_directive);
      ROSE_ASSERT(variant_body != NULL);
      if (variant_directive) {
        variant_body->set_parent(variant_directive);
        if_stmt = buildIfStmt(condition_statement, variant_directive, NULL);
      } else {
        if_stmt = buildIfStmt(condition_statement, body, NULL);
      }
      previous_if_stmt->set_false_body(if_stmt);
    }
  }

  SageInterface::replaceStatement(target, root_if_statement, true);
  pastePreprocessingInfo(root_if_statement, PreprocessingInfo::after,
                         save_buf2);
  pastePreprocessingInfo(root_if_statement, PreprocessingInfo::before,
                         save_buf1);
  // std::cout << root_if_statement->unparseToString() << "\n";
} // end analyze omp metadirective

void normalizeOmpLoop(SgStatement *node) {
  ROSE_ASSERT(node != NULL);
  SgOmpForStatement *target = isSgOmpForStatement(node);
  ROSE_ASSERT(target != NULL);

  SgScopeStatement *p_scope = target->get_scope();
  ROSE_ASSERT(p_scope != NULL);
  SgForStatement *loop = isSgForStatement(target->get_body());
  ROSE_ASSERT(loop != NULL);

  Rose_STL_Container<SgOmpClause *> clauses =
      getClause(target, V_SgOmpScheduleClause);
  if (clauses.size() != 0) {
    SgOmpScheduleClause *s_clause = isSgOmpScheduleClause(clauses[0]);
    ROSE_ASSERT(s_clause);
    SgOmpClause::omp_schedule_kind_enum sg_kind = s_clause->get_kind();
    SgExpression *orig_chunk_size = s_clause->get_chunk_size();
    if (!orig_chunk_size) {
      if (sg_kind == SgOmpClause::e_omp_schedule_kind_dynamic ||
          sg_kind == SgOmpClause::e_omp_schedule_kind_guided) {
        printf("A default chunk size is added.\n");
        SgExpression *chunk_size = buildIntVal(1);
        s_clause->set_chunk_size(chunk_size);
      }

      printf("A default schedule modifier is added.\n");
      SgOmpClause::omp_schedule_modifier_enum sg_modifier1 =
          s_clause->get_modifier1();
      if (sg_modifier1 == SgOmpClause::e_omp_schedule_modifier_unspecified) {
        sg_modifier1 = SgOmpClause::e_omp_schedule_modifier_nonmonotonic;
      }
      s_clause->set_modifier1(sg_modifier1);
    }
  } else {
    SgOmpClause::omp_schedule_modifier_enum sg_modifier1 =
        SgOmpClause::e_omp_schedule_modifier_nonmonotonic;
    SgOmpClause::omp_schedule_modifier_enum sg_modifier2 =
        SgOmpClause::e_omp_schedule_modifier_unspecified;
    SgOmpClause::omp_schedule_kind_enum sg_kind =
        SgOmpClause::e_omp_schedule_kind_static;
    SgExpression *chunk_size = NULL;
    SgOmpScheduleClause *sg_clause = new SgOmpScheduleClause(
        sg_modifier1, sg_modifier2, sg_kind, chunk_size);

    ROSE_ASSERT(sg_clause);
    setOneSourcePositionForTransformation(sg_clause);
    target->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(target);
    printf("A default schedule clause is added.\n");
  }
}

//! Patch up private variables for omp for. The reason is that loop indices
//! should be private by default and this function will make this explicit.
//! This should happen before the actual translation is done.
int patchUpPrivateVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);

  VariantVector directive_vv = VariantVector(V_SgOmpForStatement);
  directive_vv.push_back(V_SgOmpDoStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsDistributeStatement);
  directive_vv.push_back(V_SgOmpTargetParallelForStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsDistributeParallelForStatement);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, directive_vv);

  // For each omp for/do statement
  for (Rose_STL_Container<SgNode *>::iterator nodeListIterator =
           node_list.begin();
       nodeListIterator != node_list.end(); nodeListIterator++) {
    SgStatement *omp_loop = isSgStatement(*nodeListIterator);
    ROSE_ASSERT(omp_loop != NULL);
    result += patchUpPrivateVariables(omp_loop);
  } // end for omp for statments
  return result;
} // end patchUpPrivateVariables()

//! Collect threadprivate variables within the current project, return a set
//! to avoid duplicated elements
std::set<SgInitializedName *> collectThreadprivateVariables() {
  // Do the actual collection only once
  static bool calledOnce = false;
  static set<SgInitializedName *> result;

  if (calledOnce)
    return result;
  calledOnce = true;
  std::vector<SgOmpThreadprivateStatement *> tp_stmts =
      getSgNodeListFromMemoryPool<SgOmpThreadprivateStatement>();
  std::vector<SgOmpThreadprivateStatement *>::const_iterator c_iter;
  for (c_iter = tp_stmts.begin(); c_iter != tp_stmts.end(); c_iter++) {
    SgVarRefExpPtrList refs = (*c_iter)->get_variables();
    SgInitializedNamePtrList var_list; // = (*c_iter)->get_variables();
    for (size_t j = 0; j < refs.size(); j++)
      var_list.push_back(refs[j]->get_symbol()->get_declaration());
    std::copy(var_list.begin(), var_list.end(),
              std::inserter(result, result.end()));
  }
  return result;
}

// Check if a variable that is determined to be shared in all enclosing
// constructs, up to and including the innermost enclosing parallel construct,
// is shared start_stmt is the start point to find enclosing OpenMP
// constructs. It is excluded as an enclosing construct for itself.
// TODO: we only check if it is shared to the innermost enclosing parallel
// construct for now
static bool isSharedInEnclosingConstructs(SgInitializedName *init_var,
                                          SgStatement *start_stmt) {
  bool result = false;
  ROSE_ASSERT(init_var != NULL);
  ROSE_ASSERT(start_stmt != NULL);
  SgScopeStatement *var_scope = init_var->get_scope();
  //    SgScopeStatement* directive_scope = start_stmt->get_scope();
  // locally declared variables are private to the start_stmt
  // We should not do this here. It is irrelevant to this function.
  // if (isAncestor(start_stmt, init_var))
  //   return false;

  //   cout<<"Debug omp_lowering.cpp isSharedInEnclosingConstructs()
  //   SgInitializedName name = "<<init_var->get_name().getString()<<endl;
  SgOmpParallelStatement *enclosing_par_stmt =
      getEnclosingNode<SgOmpParallelStatement>(start_stmt, false);
  // Lexically nested within a parallel region
  if (enclosing_par_stmt) {
    // locally declared variables are private to enclosing_par_stmt
    SgScopeStatement *enclosing_construct_scope =
        enclosing_par_stmt->get_scope();
    ROSE_ASSERT(enclosing_construct_scope != NULL);
    if (isAncestor(enclosing_construct_scope, var_scope))
      return false;

    // Explicitly declared as a shared variable
    if (isInClauseVariableList(init_var, enclosing_par_stmt,
                               V_SgOmpSharedClause))
      result = true;
    else { // shared by default
      VariantVector vv(V_SgOmpPrivateClause);
      vv.push_back(V_SgOmpFirstprivateClause);
      vv.push_back(V_SgOmpCopyinClause);
      vv.push_back(V_SgOmpReductionClause);
      if (isInClauseVariableList(init_var, enclosing_par_stmt, vv))
        result = false;
      else
        result = true;
    }
  } else
  // the variable is in an orphaned construct
  // The variable could be
  // 1. a function parameter: it is private to its enclosing parallel region
  // 2. a global variable: either a threadprivate variable or shared by
  // default
  // 3. is a variable declared within an orphaned function: it is private to
  // its enclosing parallel region
  // ?? any other cases?? TODO
  {
    SgFunctionDefinition *func_def = getEnclosingFunctionDefinition(start_stmt);
    ROSE_ASSERT(func_def != NULL);
    if (isSgGlobal(var_scope)) {
      set<SgInitializedName *> tp_vars = collectThreadprivateVariables();
      if (tp_vars.find(init_var) != tp_vars.end())
        result = false; // is threadprivate
      else
        result = true; // otherwise
    } else if (isSgFunctionParameterList(init_var->get_parent())) {
      // function parameters are private to its dynamically (non-lexically)
      // nested parallel regions.
      result = false;
    } else if (isAncestor(func_def, var_scope)) {
      // declared within an orphaned function, should be private
      result = false;
    } else {
      cerr << "Error: OmpSupport::isSharedInEnclosingConstructs() \n "
              "Unhandled "
              "variables within an orphaned construct:"
           << endl;
      cerr << "SgInitializedName name = " << init_var->get_name().getString()
           << endl;
      dumpInfo(init_var);
      init_var->get_file_info()->display("tttt");
      ROSE_ASSERT(false);
    }
  }
  return result;
} // end isSharedInEnclosingConstructs()

//! Patch up firstprivate variables for omp task. The reason is that the
//! specification 3.0 defines rules for implicitly determined data-sharing
//! attributes and this function will make the implicit firstprivate variable
//! of omp task explicit.
/*
variables used in task block:

2.9.1.1 Data-sharing Attribute Rules for Variables Referenced in a Construct
Ref. OMP 3.0 page 79
A variable is firstprivate to the task (default) , if
** not explicitly specified by default(), shared(),private(), firstprivate()
clauses
** not shared in enclosing constructs

It should also satisfy the restriction defined in specification 3.0 page 93
TODO
* cannot be a variable which is part of another variable (as an array or
structure element)
* cannot be private, reduction
* must have an accessible, unambiguous copy constructor for the class type
* must not have a const-qualified type unless it is of class type with a
mutable member
* must not have an incomplete C/C++ type or a reference type
*
I decided to exclude variables which are used by addresses when recognizing
firstprivate variables The reason is that in real code, it is often to have
private variables first then use their address later.   Skipping the
replacement will result in wrong semantics. e.g. from Allan Porterfield void
create_seq( double seed, double a )
      {
             double x, s;
             int    i, k;

      #pragma omp parallel private(x,s,i,k)
         {
              // .....
             // here s is private
             s = find_my_seed( myid, num_procs,
                               (long)4*NUM_KEYS, seed, a );

             for (i=k1; i<k2; i++)
             {
                 x = randlc(&s, &a); // here s is used by its address

             }
         }
      }
If not, wrong code will be generated later on. The reason follows:
   * Considering nested omp tasks:
         #pragma omp task untied
            {
              int j =100;
              // i is firstprivate, item is shared
              {
                for (i = 0; i < LARGE_NUMBER; i++)
                {
      #pragma omp task if(1)
                  process (item[i],&j);
                }
              }
            }
   * the variable j will be firstprivate by default
   * however, it is used by its address within a nested task (&j)
   * replacing it with its local copy will not get the right, original
address.
   *
   * Even worse: the replacement will cause some later translation (outlining)
to
   * access the address of a parent task's local variable.
   * It seems (not 100% certain!!!) that GOMP implements tasks as independent
entities.
   * As a result a parent task's local stack will not be always accessible to
its nested tasks.
   * A segmentation fault will occur when the lexically nested task tries to
obtain the address of
   * its parent task's local variable.
   * An example mistaken translation is shown below
       int main()
      {
        GOMP_parallel_start(OUT__3__1527__,0,0);
        OUT__3__1527__();
        GOMP_parallel_end();
        return 0;
      }

      void OUT__3__1527__()
      {
        if (GOMP_single_start()) {
          int i;
          printf(("Using %d threads.\n"),omp_get_num_threads());
          void *__out_argv2__1527__[1];
          __out_argv2__1527__[0] = ((void *)(&i));
          GOMP_task(OUT__2__1527__,&__out_argv2__1527__,0,4,4,1,1);
          //GOMP_task(OUT__2__1527__,&__out_argv2__1527__,0,4,4,1,0); //untied
or not, no difference
        }
      }

      void OUT__2__1527__(void **__out_argv)
{
  int *i = (int *)(__out_argv[0]);
  //  int _p_i;
  //  _p_i =  *i;
  //  for (_p_i = 0; _p_i < 1000; _p_i++) {
  for (*i = 0; *i < 1000; (*i)++) {
    void *__out_argv1__1527__[1];
    // cannot access auto variable from the stack of another task instance!!
    //__out_argv1__1527__[0] = ((void *)(&_p_i));
    __out_argv1__1527__[0] = ((void *)(&(*i)));// this is the right
translation GOMP_task(OUT__1__1527__,&__out_argv1__1527__,0,4,4,1,0);
  }
}
void OUT__1__1527__(void **__out_argv)
{
  int *i = (int *)(__out_argv[0]);
  int _p_i;
  _p_i =  *i;
  assert(_p_i>=0);
  assert(_p_i<10000);

  process((item[_p_i]));
}
*
  */
int patchUpFirstprivateVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> nodeList =
      NodeQuery::querySubTree(file, V_SgOmpTaskStatement);
  Rose_STL_Container<SgNode *>::iterator iter = nodeList.begin();
  for (; iter != nodeList.end(); iter++) {
    SgOmpTaskStatement *target = isSgOmpTaskStatement(*iter);
    SgScopeStatement *directive_scope = target->get_scope();
    SgStatement *body = target->get_body();
    ROSE_ASSERT(body != NULL);

    // Find all variable references from the task's body
    Rose_STL_Container<SgNode *> refList =
        NodeQuery::querySubTree(body, V_SgVarRefExp);
    Rose_STL_Container<SgNode *>::iterator var_iter = refList.begin();
    for (; var_iter != refList.end(); var_iter++) {
      SgVarRefExp *var_ref = isSgVarRefExp(*var_iter);
      ROSE_ASSERT(var_ref->get_symbol() != NULL);
      SgInitializedName *init_var = var_ref->get_symbol()->get_declaration();
      ROSE_ASSERT(init_var != NULL);
      SgScopeStatement *var_scope = init_var->get_scope();
      ROSE_ASSERT(var_scope != NULL);

      // Variables with automatic storage duration that are declared in
      // a scope inside the construct are private. Skip them
      if (isAncestor(directive_scope, var_scope))
        continue;

      if (SageInterface::isUseByAddressVariableRef(var_ref))
        continue;
      // Skip variables already with explicit data-sharing attributes
      VariantVector vv(V_SgOmpDefaultClause);
      vv.push_back(V_SgOmpPrivateClause);
      vv.push_back(V_SgOmpSharedClause);
      vv.push_back(V_SgOmpFirstprivateClause);
      if (isInClauseVariableList(init_var, target, vv))
        continue;
      // Skip variables which are class/structure members: part of another
      // variable
      if (isSgClassDefinition(init_var->get_scope()))
        continue;
      // Skip variables which are shared in enclosing constructs
      if (isSharedInEnclosingConstructs(init_var, target))
        continue;
      // Now it should be a firstprivate variable
      addClauseVariable(init_var, target, V_SgOmpFirstprivateClause);
      result++;
    } // end for each variable reference
  }   // end for each SgOmpTaskStatement
  return result;
} // end patchUpFirstprivateVariables()

int patchUpImplicitMappingVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);

  VariantVector directive_vv = VariantVector(V_SgOmpTargetStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsStatement);
  directive_vv.push_back(V_SgOmpTargetParallelStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsDistributeParallelForStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsDistributeStatement);
  directive_vv.push_back(V_SgOmpTargetParallelForStatement);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, directive_vv);

  Rose_STL_Container<SgNode *>::iterator iter = node_list.begin();
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpClauseBodyStatement *target = NULL;
    target = isSgOmpClauseBodyStatement(*iter);
    SgScopeStatement *directive_scope = target->get_scope();
    SgStatement *body = target->get_body();
    ROSE_ASSERT(body != NULL);

    // Find all variable references from the task's body
    Rose_STL_Container<SgNode *> ref_list =
        NodeQuery::querySubTree(body, V_SgVarRefExp);
    Rose_STL_Container<SgNode *>::iterator var_iter = ref_list.begin();
    for (var_iter = ref_list.begin(); var_iter != ref_list.end(); var_iter++) {
      SgVarRefExp *var_ref = isSgVarRefExp(*var_iter);
      ROSE_ASSERT(var_ref->get_symbol() != NULL);
      SgInitializedName *init_var = var_ref->get_symbol()->get_declaration();
      ROSE_ASSERT(init_var != NULL);
      SgScopeStatement *var_scope = init_var->get_scope();
      ROSE_ASSERT(var_scope != NULL);

      // Variables with automatic storage duration that are declared in
      // a scope inside the construct are private. Skip them
      if (isAncestor(directive_scope, var_scope))
        continue;

      // Skip variables already with explicit data-sharing attributes
      VariantVector vv(V_SgOmpDefaultClause);
      vv.push_back(V_SgOmpPrivateClause);
      vv.push_back(V_SgOmpSharedClause);
      vv.push_back(V_SgOmpFirstprivateClause);
      vv.push_back(V_SgOmpMapClause);
      if (isInClauseVariableList(init_var, target, vv))
        continue;
      // Skip variables which are class/structure members: part of another
      // variable
      if (isSgClassDefinition(init_var->get_scope()))
        continue;
      // Skip variables which are shared in enclosing constructs
      if (!isSgGlobal(var_scope) &&
          isSharedInEnclosingConstructs(init_var, target))
        continue;
      // Now it should be mapped explicitly.
      SgVariableSymbol *sym = var_ref->get_symbol();
      ROSE_ASSERT(sym != NULL);

      SgOmpMapClause *map_clause = NULL;
      std::map<SgSymbol *,
               std::vector<std::pair<SgExpression *, SgExpression *>>>
          array_dimensions;
      SgExprListExp *explist = NULL;

      if (hasClause(target, V_SgOmpMapClause)) {
        Rose_STL_Container<SgOmpClause *> map_clauses =
            getClause(target, V_SgOmpMapClause);
        Rose_STL_Container<SgOmpClause *>::const_iterator iter;
        for (iter = map_clauses.begin(); iter != map_clauses.end(); iter++) {
          SgOmpMapClause *temp_map_clause = isSgOmpMapClause(*iter);
          if (temp_map_clause->get_operation() == SgOmpClause::e_omp_map_to) {
            map_clause = temp_map_clause;
            array_dimensions = map_clause->get_array_dimensions();
            explist = map_clause->get_variables();
            break;
          }
        }
      }

      if (map_clause == NULL) {
        explist = buildExprListExp();
        SgOmpClause::omp_map_operator_enum sg_type = SgOmpClause::e_omp_map_to;
        map_clause = new SgOmpMapClause(explist, sg_type);
        explist->set_parent(map_clause);
        setOneSourcePositionForTransformation(map_clause);
        map_clause->set_parent(target);
        target->get_clauses().push_back(map_clause);
      }

      bool has_mapped = false;
      Rose_STL_Container<SgExpression *>::iterator iter;
      SgExpressionPtrList expression_list = explist->get_expressions();
      for (iter = expression_list.begin(); iter != expression_list.end();
           iter++) {
        if (isSgVarRefExp(*iter)->get_symbol() == sym) {
          has_mapped = true;
          break;
        }
      }

      if (has_mapped == false) {
        SgType *orig_type = sym->get_type();
        SgArrayType *a_type = isSgArrayType(orig_type);
        if (a_type != NULL) {
          std::vector<SgExpression *> dims = get_C_array_dimensions(a_type);
          SgExpression *array_length = NULL;
          for (std::vector<SgExpression *>::const_iterator iter = dims.begin();
               iter != dims.end(); iter++) {
            SgExpression *length_exp = *iter;
            // TODO: get_C_array_dimensions returns one extra null expression
            // somehow.
            if (!isSgNullExpression(length_exp))
              array_length = length_exp;
          }
          ROSE_ASSERT(array_length != NULL);
          SgVariableSymbol *array_symbol = var_ref->get_symbol();

          SgExpression *lower_exp = buildIntVal(0);
          array_dimensions[array_symbol].push_back(
              std::make_pair(lower_exp, array_length));
          map_clause->set_array_dimensions(array_dimensions);
        }
        explist->append_expression(buildVarRefExp(var_ref->get_symbol()));
      }
      result++;
    } // end for each variable reference
  }
  return result;
} // end patchUpImplicitMappingVariables()

int patchUpImplicitSharedVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);

  VariantVector directive_vv = VariantVector(V_SgOmpParallelStatement);
  directive_vv.push_back(V_SgOmpTeamsStatement);
  directive_vv.push_back(V_SgOmpTeamsDistributeParallelForStatement);
  directive_vv.push_back(V_SgOmpTeamsDistributeStatement);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, directive_vv);

  Rose_STL_Container<SgNode *>::iterator iter = node_list.begin();
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpClauseBodyStatement *target = NULL;
    target = isSgOmpClauseBodyStatement(*iter);
    SgScopeStatement *directive_scope = target->get_scope();
    SgStatement *body = target->get_body();
    ROSE_ASSERT(body != NULL);

    // Find all variable references from the task's body
    Rose_STL_Container<SgNode *> ref_list =
        NodeQuery::querySubTree(body, V_SgVarRefExp);
    Rose_STL_Container<SgNode *>::iterator var_iter = ref_list.begin();
    for (var_iter = ref_list.begin(); var_iter != ref_list.end(); var_iter++) {
      SgVarRefExp *var_ref = isSgVarRefExp(*var_iter);
      ROSE_ASSERT(var_ref->get_symbol() != NULL);
      SgInitializedName *init_var = var_ref->get_symbol()->get_declaration();
      ROSE_ASSERT(init_var != NULL);
      SgScopeStatement *var_scope = init_var->get_scope();
      ROSE_ASSERT(var_scope != NULL);

      // Variables with automatic storage duration that are declared in
      // a scope inside the construct are private. Skip them
      if (isAncestor(directive_scope, var_scope))
        continue;

      // Skip variables already with explicit data-sharing attributes
      VariantVector vv(V_SgOmpDefaultClause);
      vv.push_back(V_SgOmpPrivateClause);
      vv.push_back(V_SgOmpSharedClause);
      vv.push_back(V_SgOmpFirstprivateClause);
      if (isInClauseVariableList(init_var, target, vv))
        continue;
      // Skip variables which are class/structure members: part of another
      // variable
      if (isSgClassDefinition(init_var->get_scope()))
        continue;
      // Skip variables which are shared in enclosing constructs
      if (!isSgGlobal(var_scope) &&
          isSharedInEnclosingConstructs(init_var, target))
        continue;

      // Now it should be in a shared variable
      addClauseVariable(init_var, target, V_SgOmpSharedClause);
      result++;
    } // end for each variable reference
  }
  return result;
} // end patchUpImplicitMappingVariables()

// map variables in omp target firstprivate clause
int normalizeOmpMapVariables(SgFile *file, VariantVector clause_vv,
                             SgOmpClause::omp_map_operator_enum map_type) {
  int result = 0;
  ROSE_ASSERT(file != NULL);

  VariantVector directive_vv = VariantVector(V_SgOmpTargetStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsDistributeParallelForStatement);
  directive_vv.push_back(V_SgOmpTargetTeamsDistributeStatement);
  directive_vv.push_back(V_SgOmpTargetParallelStatement);
  directive_vv.push_back(V_SgOmpTargetParallelForStatement);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, directive_vv);

  Rose_STL_Container<SgNode *>::iterator iter;
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpClauseBodyStatement *target = isSgOmpClauseBodyStatement(*iter);
    SgStatement *body = target->get_body();
    ROSE_ASSERT(body != NULL);

    SgInitializedNamePtrList all_vars =
        collectClauseVariables(target, clause_vv);

    SgOmpMapClause *map_clause = NULL;
    SgExprListExp *explist = NULL;
    bool has_map_to_clause = false;
    // use the existing MAP TO clause if any.
    if (hasClause(target, V_SgOmpMapClause)) {
      Rose_STL_Container<SgOmpClause *> map_clauses =
          getClause(target, V_SgOmpMapClause);
      Rose_STL_Container<SgOmpClause *>::const_iterator iter;
      for (iter = map_clauses.begin(); iter != map_clauses.end(); iter++) {
        SgOmpMapClause *temp_map_clause = isSgOmpMapClause(*iter);
        if (temp_map_clause->get_operation() == SgOmpClause::e_omp_map_to) {
          map_clause = temp_map_clause;
          explist = map_clause->get_variables();
          has_map_to_clause = true;
          break;
        }
      }
    }

    // create a new MAP TO clause if there isn't one.
    if (has_map_to_clause == false) {
      explist = buildExprListExp();
      SgOmpClause::omp_map_operator_enum sg_type = map_type;
      map_clause = new SgOmpMapClause(explist, sg_type);
    };
    bool has_mapped = false;

    for (size_t i = 0; i < all_vars.size(); i++) {
      if (isInClauseVariableList(all_vars[i], target, V_SgOmpMapClause))
        continue;
      SgVarRefExp *var_ref = buildVarRefExp(all_vars[i]);
      explist->append_expression(var_ref);
      var_ref->set_parent(map_clause);
      has_mapped = true;
    }

    if (has_map_to_clause == false && has_mapped == true) {
      setOneSourcePositionForTransformation(map_clause);
      explist->set_parent(map_clause);
      map_clause->set_parent(target);
      target->get_clauses().push_back(map_clause);
    }
  }
  return result;
} // end normalizeOmpMapVariables()

bool isInOmpTargetRegion(SgStatement *node) {
  SgOmpExecStatement *target = isSgOmpExecStatement(node);
  ROSE_ASSERT(target);
  SgOmpExecStatement *parent = NULL;
  do {
    parent = isSgOmpExecStatement(target->get_omp_parent());
    if (parent != NULL) {
      switch (parent->variantT()) {
      case V_SgOmpTargetStatement:
      case V_SgOmpTargetTeamsStatement:
      case V_SgOmpTargetTeamsDistributeStatement:
        return true;
      default:
        target = parent;
      }
    }
  } while (parent != NULL);
  return false;
}

// set the parent and children of a given OpenMP executable directive node
void setOmpRelationship(SgStatement *parent, SgStatement *child) {
  SgOmpExecStatement *omp_parent = isSgOmpExecStatement(parent);
  ROSE_ASSERT(omp_parent != NULL);
  SgStatementPtrList &children = omp_parent->get_omp_children();
  children.push_back(child);

  SgOmpExecStatement *omp_child = isSgOmpExecStatement(child);
  omp_child->set_omp_parent(parent);
}

// search the OpenMP parent of a given OpenMP executable directive node, not
// its SgNode parent.
SgStatement *getOmpParent(SgStatement *node) {
  SgStatement *parent = isSgStatement(node->get_parent());
  while (parent != NULL) {
    if (isSgOmpExecStatement(parent))
      return parent;
    parent = isSgStatement(parent->get_parent());
  }
  return NULL;
}

// traverse the SgNode AST and fill the information of OpenMP executable
// directive parent and children.
void createOmpStatementTree(SgSourceFile *file) {
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgOmpExecStatement);
  Rose_STL_Container<SgNode *>::reverse_iterator node_list_iterator;
  for (node_list_iterator = node_list.rbegin();
       node_list_iterator != node_list.rend(); node_list_iterator++) {
    SgOmpExecStatement *node = isSgOmpExecStatement(*node_list_iterator);
    SgStatement *parent = getOmpParent(node);
    if (parent != NULL) {
      setOmpRelationship(parent, node);
    } else {
      node->set_omp_parent(parent);
    }
  }
}

void setOmpNumTeams(SgNode *node) {
  ROSE_ASSERT(node != NULL);
  SgOmpClauseBodyStatement *target = isSgOmpClauseBodyStatement(node);
  ROSE_ASSERT(target != NULL);

  SgExpression *num_teams_expression = NULL;
  SgOmpNumTeamsClause *num_teams_clause = NULL;
  if (hasClause(target, V_SgOmpNumTeamsClause)) {
    Rose_STL_Container<SgOmpClause *> num_teams_clauses =
        getClause(target, V_SgOmpNumTeamsClause);
    ROSE_ASSERT(num_teams_clauses.size() ==
                1); // should only have one num_teams()
    num_teams_clause = isSgOmpNumTeamsClause(num_teams_clauses[0]);
    ROSE_ASSERT(num_teams_clause->get_expression() != NULL);
  } else {
    num_teams_expression = buildIntVal(256);
    num_teams_clause = new SgOmpNumTeamsClause(num_teams_expression);
    addOmpClause(target, num_teams_clause);
    num_teams_clause->set_parent(target);
    setOneSourcePositionForTransformation(num_teams_clause);
  }
}

void setOmpNumThreads(SgNode *node) {
  ROSE_ASSERT(node != NULL);
  SgOmpClauseBodyStatement *target = isSgOmpClauseBodyStatement(node);
  ROSE_ASSERT(target != NULL);

  SgExpression *num_threads_expression = NULL;
  SgOmpNumThreadsClause *num_threads_clause = NULL;
  if (hasClause(target, V_SgOmpNumThreadsClause)) {
    Rose_STL_Container<SgOmpClause *> num_threads_clauses =
        getClause(target, V_SgOmpNumThreadsClause);
    ROSE_ASSERT(num_threads_clauses.size() ==
                1); // should only have one num_threads()
    num_threads_clause = isSgOmpNumThreadsClause(num_threads_clauses[0]);
    ROSE_ASSERT(num_threads_clause->get_expression() != NULL);
  } else {
    num_threads_expression = buildIntVal(128);
    num_threads_clause = new SgOmpNumThreadsClause(num_threads_expression);
    addOmpClause(target, num_threads_clause);
    num_threads_clause->set_parent(target);
    setOneSourcePositionForTransformation(num_threads_clause);
  }
}

void normalizeOmpTargetOffloadingUnits(SgFile *file) {
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> omp_nodes =
      NodeQuery::querySubTree(file, V_SgOmpExecStatement);
  Rose_STL_Container<SgNode *>::iterator iter;
  SgOmpExecStatement *parent = NULL;
  for (iter = omp_nodes.begin(); iter != omp_nodes.end(); iter++) {
    SgOmpExecStatement *node = isSgOmpExecStatement(*iter);
    ROSE_ASSERT(node != NULL);
    // It doesn't need to check whether the directive is a variant because
    // metadirective has been lowered at this point.
    switch (node->variantT()) {
    case V_SgOmpTargetTeamsStatement:
    case V_SgOmpTargetTeamsDistributeStatement:
      setOmpNumTeams(node);
      break;
    case V_SgOmpTargetParallelForStatement:
    case V_SgOmpTargetParallelStatement:
      setOmpNumThreads(node);
      break;
    case V_SgOmpTargetTeamsDistributeParallelForStatement:
      setOmpNumTeams(node);
      setOmpNumThreads(node);
      break;
    // Check whether parallel/parallel for is in a target region.
    // case V_SgOmpParallelForStatement:
    case V_SgOmpParallelStatement:
      if (isInOmpTargetRegion(node))
        setOmpNumThreads(node);
      break;
    // Check whether teams/teams distribute is in a target region.
    case V_SgOmpTeamsStatement:
    case V_SgOmpTeamsDistributeStatement:
      if (isInOmpTargetRegion(node))
        setOmpNumTeams(node);
      break;
    // Check whether teams distribute parallel for is in a target region.
    case V_SgOmpTeamsDistributeParallelForStatement:
      if (isInOmpTargetRegion(node)) {
        setOmpNumTeams(node);
        setOmpNumThreads(node);
      }
      break;
    default:;
    }
  }
}

void analyze_omp(SgSourceFile *file) {
  // Transform omp metadirective to multiple variants.
  Rose_STL_Container<SgNode *> variant_directives =
      NodeQuery::querySubTree(file, V_SgOmpMetadirectiveStatement);
  Rose_STL_Container<SgNode *>::iterator node_list_iterator;
  for (node_list_iterator = variant_directives.begin();
       node_list_iterator != variant_directives.end(); node_list_iterator++) {
    SgStatement *node = isSgStatement(*node_list_iterator);
    ROSE_ASSERT(node != NULL);
    analyzeOmpMetadirective(node);
  }

  patchUpPrivateVariables(file); // the order of these two functions matter! We
                                 // want to patch up private variable first!
  patchUpFirstprivateVariables(file);

  patchUpImplicitSharedVariables(file);

  patchUpImplicitMappingVariables(file);

  // Convert firstprivate/private/shared clause in target directive to map
  // clause because later only map clause will be lowered for data transferring.
  VariantVector clause_vv = VariantVector(V_SgOmpFirstprivateClause);
  clause_vv.push_back(V_SgOmpPrivateClause);
  clause_vv.push_back(V_SgOmpSharedClause);
  normalizeOmpMapVariables(file, clause_vv, SgOmpClause::e_omp_map_to);

  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgOmpForStatement);
  for (node_list_iterator = node_list.begin();
       node_list_iterator != node_list.end(); node_list_iterator++) {
    SgStatement *node = isSgStatement(*node_list_iterator);
    ROSE_ASSERT(node != NULL);
    normalizeOmpLoop(node);
  }

  // Add the information of OpenMP directive parent and children.
  OmpSupport::createOmpStatementTree(file);
  // Normalize num_teams and num_threads in the target region.
  OmpSupport::normalizeOmpTargetOffloadingUnits(file);
}

} // namespace OmpSupport
