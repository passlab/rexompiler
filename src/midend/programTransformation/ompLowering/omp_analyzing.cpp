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

void analyzeUpirLoop(SgStatement *node) {
  SgUpirLoopStatement *target = isSgUpirLoopStatement(node);
  ROSE_ASSERT(target != NULL);
  SgForStatement *for_loop = isSgForStatement(target->get_body());
  ROSE_ASSERT(for_loop != NULL);
  SageInterface::forLoopNormalization(for_loop);

  SgInitializedName *orig_index = NULL;
  SgExpression *orig_lower = NULL;
  SgExpression *orig_upper = NULL;
  SgExpression *orig_stride = NULL;
  bool isIncremental = false;
  bool is_canonical = false;
  is_canonical =
      isCanonicalForLoop(for_loop, &orig_index, &orig_lower, &orig_upper,
                         &orig_stride, NULL, &isIncremental);
  ROSE_ASSERT(is_canonical == true);
  target->set_induction(orig_index);
  target->set_lower_bound(orig_lower);
  target->set_upper_bound(orig_upper);
  target->set_step(orig_stride);
  target->set_incremental(isIncremental);
}

void normalizeUpirLoop(SgStatement *node) {
  ROSE_ASSERT(node != NULL);
  SgOmpForStatement *target = isSgOmpForStatement(node);
  ROSE_ASSERT(target != NULL);

  // analyzeUpirLoop(target);

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
  Rose_STL_Container<SgNode *> nodeList_merged =
      NodeQuery::querySubTree(file, V_SgOmpForStatement);

  nodeList_merged = mergeSgNodeList(
      nodeList_merged, NodeQuery::querySubTree(file, V_SgOmpDoStatement));
  nodeList_merged = mergeSgNodeList(
      nodeList_merged,
      NodeQuery::querySubTree(file, V_SgOmpTargetParallelForStatement));
  nodeList_merged = mergeSgNodeList(
      nodeList_merged,
      NodeQuery::querySubTree(
          file, V_SgOmpTargetTeamsDistributeParallelForStatement));

  Rose_STL_Container<SgNode *>::iterator nodeListIterator =
      nodeList_merged.begin();
  // For each omp for/do statement
  for (; nodeListIterator != nodeList_merged.end(); ++nodeListIterator) {
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

      SgVariableSymbol *variable_symbol =
          isSgVariableSymbol(init_var->search_for_symbol_from_symbol_table());
      std::set<SgOmpClause::upir_data_sharing_enum> searching_variants;
      searching_variants.insert(SgOmpClause::e_upir_data_sharing_private);
      searching_variants.insert(SgOmpClause::e_upir_data_sharing_firstprivate);
      searching_variants.insert(SgOmpClause::e_upir_data_sharing_shared);

      if (isInUpirDataSharingList(target, variable_symbol, searching_variants))
        continue;
      // Skip variables which are class/structure members: part of another
      // variable
      if (isSgClassDefinition(init_var->get_scope()))
        continue;
      // Skip variables which are shared in enclosing constructs
      if (isSharedInEnclosingConstructs(init_var, target))
        continue;
      // Now it should be a firstprivate variable
      SgUpirDataItemField *upir_data_item =
          new SgUpirDataItemField(variable_symbol);
      upir_data_item->set_sharing_property(
          SgOmpClause::e_upir_data_sharing_firstprivate);
      addUpirDataVariable(isSgOmpClauseBodyStatement(target), upir_data_item);

      result++;
    } // end for each variable reference
  }   // end for each SgOmpTaskStatement
  return result;
} // end patchUpFirstprivateVariables()

int patchUpImplicitMappingVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> node_list1 =
      NodeQuery::querySubTree(file, V_SgOmpTargetStatement);

  // TODO: implement a helper function to collect all the nodes into one list
  Rose_STL_Container<SgNode *> node_list2 =
      NodeQuery::querySubTree(file, V_SgOmpTargetParallelForStatement);
  std::sort(node_list1.begin(), node_list1.end());
  std::sort(node_list2.begin(), node_list2.end());
  Rose_STL_Container<SgNode *> node_list;
  std::merge(node_list1.begin(), node_list1.end(), node_list2.begin(),
             node_list2.end(),
             std::insert_iterator<Rose_STL_Container<SgNode *>>(
                 node_list, node_list.end()));

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
      if (isSharedInEnclosingConstructs(init_var, target))
        continue;
      // Now it should be a shared variable
      if (isSgOmpTargetStatement(target) ||
          isSgOmpTargetParallelForStatement(target)) {
        SgVariableSymbol *sym = var_ref->get_symbol();
        ROSE_ASSERT(sym != NULL);
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
          SgOmpMapClause *map_clause = NULL;
          std::map<SgSymbol *,
                   std::vector<std::pair<SgExpression *, SgExpression *>>>
              array_dimensions;
          SgExprListExp *explist = NULL;
          SgVariableSymbol *array_symbol = var_ref->get_symbol();

          if (hasClause(target, V_SgOmpMapClause)) {
            Rose_STL_Container<SgOmpClause *> map_clauses =
                getClause(target, V_SgOmpMapClause);
            Rose_STL_Container<SgOmpClause *>::const_iterator iter;
            for (iter = map_clauses.begin(); iter != map_clauses.end();
                 iter++) {
              SgOmpMapClause *temp_map_clause = isSgOmpMapClause(*iter);
              if (temp_map_clause->get_operation() ==
                  SgOmpClause::e_omp_map_tofrom) {
                map_clause = temp_map_clause;
                array_dimensions = map_clause->get_array_dimensions();
                explist = map_clause->get_variables();
                break;
              }
            }
          }

          if (map_clause == NULL) {
            explist = buildExprListExp();
            SgOmpClause::omp_map_operator_enum sg_type =
                SgOmpClause::e_omp_map_tofrom;
            map_clause = new SgOmpMapClause(explist, sg_type);
            explist->set_parent(map_clause);
            setOneSourcePositionForTransformation(map_clause);
            map_clause->set_parent(target);
          }

          bool has_mapped = false;
          Rose_STL_Container<SgExpression *>::iterator iter;
          SgExpressionPtrList expression_list = explist->get_expressions();
          for (iter = expression_list.begin(); iter != expression_list.end();
               iter++) {
            if (isSgVarRefExp(*iter)->get_symbol() == array_symbol) {
              has_mapped = true;
              break;
            }
          }

          if (has_mapped == false) {
            SgExpression *lower_exp = buildIntVal(0);
            explist->append_expression(buildVarRefExp(var_ref->get_symbol()));
            array_dimensions[array_symbol].push_back(
                std::make_pair(lower_exp, array_length));
            map_clause->set_array_dimensions(array_dimensions);
          }

        } else {
          addClauseVariable(init_var, target, V_SgOmpFirstprivateClause);
        }
      } else {
        addClauseVariable(init_var, target, V_SgOmpSharedClause);
      }
      result++;
    } // end for each variable reference
  }
  return result;
} // end patchUpImplicitMappingVariables()

// map variables in omp target firstprivate clause
int unifyUpirTaskMappingVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgOmpTargetStatement);

  Rose_STL_Container<SgNode *>::iterator iter;
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpClauseBodyStatement *target = NULL;
    target = isSgOmpClauseBodyStatement(*iter);
    SgStatement *body = target->get_body();
    ROSE_ASSERT(body != NULL);

    if (hasClause(target, V_SgOmpFirstprivateClause) == false) {
      continue;
    }
    Rose_STL_Container<SgOmpClause *> clauses =
        getClause(target, V_SgOmpFirstprivateClause);
    SgOmpFirstprivateClause *firstprivate_clause =
        isSgOmpFirstprivateClause(clauses[0]);
    SgExpressionPtrList firstprivate_symbols =
        firstprivate_clause->get_variables()->get_expressions();

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
      SgOmpClause::omp_map_operator_enum sg_type = SgOmpClause::e_omp_map_to;
      map_clause = new SgOmpMapClause(explist, sg_type);
    };
    bool has_mapped = false;

    for (size_t i = 0; i < firstprivate_symbols.size(); i++) {
      SgVarRefExp *var_ref = isSgVarRefExp(firstprivate_symbols[i]);
      SgVariableSymbol *sym = var_ref->get_symbol();
      ROSE_ASSERT(sym != NULL);
      SgType *orig_type = sym->get_type();
      SgArrayType *a_type = isSgArrayType(orig_type);
      if (a_type == NULL && !isPointerType(orig_type)) {
        explist->append_expression(var_ref);
        var_ref->set_parent(map_clause);
        has_mapped = true;
      }
    }
    if (has_map_to_clause == false && has_mapped == true) {
      setOneSourcePositionForTransformation(map_clause);
      explist->set_parent(map_clause);
      map_clause->set_parent(target);
      target->get_clauses().push_back(map_clause);
    }
  }
  return result;
} // end unifyUpirTaskMappingVariables()

// Check if a variable is in the UPIR data field's item list
SgUpirDataItemField *isInUpirDataList(SgOmpClause *target, SgSymbol *variable) {
  SgUpirDataField *data_field = isSgUpirDataField(target);
  SgVariableSymbol *variable_symbol = isSgVariableSymbol(variable);
  ROSE_ASSERT(data_field);

  std::vector<SgSymbol *> variable_list;
  std::list<SgUpirDataItemField *> data_items = data_field->get_data();
  for (std::list<SgUpirDataItemField *>::iterator iter = data_items.begin();
       iter != data_items.end(); iter++) {
    SgUpirDataItemField *data_item = *iter;
    if (data_item->get_symbol() == variable)
      return data_item;
  }

  return NULL;
}

SgUpirDataField *getUpirDataField(SgStatement *target) {
  SgOmpClauseBodyStatement *target_directive =
      isSgOmpClauseBodyStatement(target);
  SgUpirDataField *data_field = NULL;
  Rose_STL_Container<SgOmpClause *> data_fields =
      getClause(target_directive, V_SgUpirDataField);
  if (data_fields.size() == 0) {
    data_field = new SgUpirDataField();
    setOneSourcePositionForTransformation(data_field);
    target_directive->get_clauses().push_back(data_field);
    data_field->set_parent(target_directive);
  } else {
    ROSE_ASSERT(data_fields.size() == 1);
    data_field = isSgUpirDataField(data_fields[0]);
    ROSE_ASSERT(data_field != NULL);
  }

  return data_field;
}

void createUpirDataItems(
    SgOmpVariablesClause *target,
    SgOmpClause::upir_data_sharing_enum data_sharing_type) {
  ROSE_ASSERT(target != NULL);
  SgUpirDataField *data_field =
      getUpirDataField(isSgStatement(target->get_parent()));

  std::list<SgUpirDataItemField *> data_items = data_field->get_data();
  SgExpressionPtrList variables = target->get_variables()->get_expressions();
  for (size_t i = 0; i < variables.size(); i++) {

    SgVarRefExp *variable_expression = isSgVarRefExp(variables[i]);
    SgVariableSymbol *variable_symbol = variable_expression->get_symbol();
    assert(variable_symbol != NULL);

    if (!isInUpirDataList(data_field, variable_symbol))
      ROSE_ASSERT(0 && "The symbol exists in the data field.");

    SgUpirDataItemField *data_item = new SgUpirDataItemField(variable_symbol);

    data_item->set_sharing_property(data_sharing_type);
    data_items.push_back(data_item);
    setOneSourcePositionForTransformation(data_item);
    data_item->set_parent(data_field);
  }

  data_field->set_data(data_items);
}

// create data fields based on data sharing properties
void createUpirDataFields(SgFile *file) {
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> node_list;
  Rose_STL_Container<SgNode *>::iterator iter;

  node_list = NodeQuery::querySubTree(file, V_SgOmpSharedClause);
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpVariablesClause *target = isSgOmpVariablesClause(*iter);
    createUpirDataItems(target, SgOmpClause::e_upir_data_sharing_shared);
  }

  node_list = NodeQuery::querySubTree(file, V_SgOmpPrivateClause);
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpVariablesClause *target = isSgOmpVariablesClause(*iter);
    createUpirDataItems(target, SgOmpClause::e_upir_data_sharing_private);
  }

  node_list = NodeQuery::querySubTree(file, V_SgOmpFirstprivateClause);
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpVariablesClause *target = isSgOmpVariablesClause(*iter);
    createUpirDataItems(target, SgOmpClause::e_upir_data_sharing_firstprivate);
  }

  node_list = NodeQuery::querySubTree(file, V_SgOmpLastprivateClause);
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpVariablesClause *target = isSgOmpVariablesClause(*iter);
    createUpirDataItems(target, SgOmpClause::e_upir_data_sharing_lastprivate);
  }
} // end createUpirDataFields()

// create data fields
int createUpirMappingDataFields(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgOmpMapClause);

  Rose_STL_Container<SgNode *>::iterator iter;
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpMapClause *target = isSgOmpMapClause(*iter);

    SgOmpClause::upir_data_mapping_enum data_mapping_type =
        SgOmpClause::e_upir_data_mapping_unspecified;
    SgOmpClause::omp_map_operator_enum sg_type = target->get_operation();
    switch (sg_type) {
    case SgOmpClause::e_omp_map_to:
      data_mapping_type = SgOmpClause::e_upir_data_mapping_to;
      break;
    case SgOmpClause::e_omp_map_from:
      data_mapping_type = SgOmpClause::e_upir_data_mapping_from;
      break;
    case SgOmpClause::e_omp_map_tofrom:
      data_mapping_type = SgOmpClause::e_upir_data_mapping_tofrom;
      break;
    default:;
    }

    SgExpressionPtrList variables = target->get_variables()->get_expressions();
    std::map<SgSymbol *, std::vector<std::pair<SgExpression *, SgExpression *>>>
        array_dimensions = target->get_array_dimensions();

    SgUpirDataField *upir_data =
        getUpirDataField(isSgStatement(target->get_parent()));
    std::list<SgUpirDataItemField *> data_items = upir_data->get_data();
    for (size_t i = 0; i < variables.size(); i++) {

      SgVarRefExp *variable_expression = isSgVarRefExp(variables[i]);
      SgVariableSymbol *variable_symbol = variable_expression->get_symbol();
      assert(variable_symbol != NULL);

      SgUpirDataItemField *upir_data_item =
          isInUpirDataList(upir_data, variable_symbol);
      if (upir_data_item == NULL) {
        upir_data_item = new SgUpirDataItemField(variable_symbol);
        data_items.push_back(upir_data_item);
        setOneSourcePositionForTransformation(upir_data_item);
        upir_data_item->set_parent(upir_data);
      }

      std::list<std::list<SgExpression *>> upir_section =
          upir_data_item->get_section();
      std::vector<std::pair<SgExpression *, SgExpression *>> sections =
          array_dimensions[variable_symbol];
      for (size_t i = 0; i < sections.size(); i++) {
        SgExpression *lower_bound = sections[i].first;
        SgExpression *length = sections[i].second;
        // ROSE/REX doesn't support stride yet, it is always set to 1 for now.
        SgExpression *stride = buildIntVal(1);
        std::list<SgExpression *> section = {lower_bound, length, stride};
        upir_section.push_back(section);
      }

      upir_data_item->set_section(upir_section);
      upir_data_item->set_mapping_property(data_mapping_type);
      if (upir_data_item->get_sharing_property() ==
          SgOmpClause::e_upir_data_sharing_unspecified)
        upir_data_item->set_sharing_property(
            SgOmpClause::e_upir_data_sharing_shared);
    }

    upir_data->set_data(data_items);
  }

  return result;
} // end createUpirMappingDataFields()

// organize UPIR data information based on different attributes for queries.
void collectUpirDataQueryInformation(SgFile *file) {
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgUpirDataField);
  if (node_list.size() == 0)
    return;
  ROSE_ASSERT(node_list.size() == 1);
  SgUpirDataField *data_field = isSgUpirDataField(node_list[0]);
  std::list<SgUpirDataItemField *> data_items = data_field->get_data();

  std::list<SgUpirDataItemField *> shared_data = data_field->get_shared_data();
  std::list<SgUpirDataItemField *> private_data =
      data_field->get_private_data();
  std::list<SgUpirDataItemField *> firstprivate_data =
      data_field->get_firstprivate_data();
  std::list<SgUpirDataItemField *> lastprivate_data =
      data_field->get_lastprivate_data();
  std::list<SgUpirDataItemField *> reduction_data =
      data_field->get_reduction_data();

  std::list<SgUpirDataItemField *> map_to_data = data_field->get_map_to_data();
  std::list<SgUpirDataItemField *> map_from_data =
      data_field->get_map_from_data();
  std::list<SgUpirDataItemField *> map_tofrom_data =
      data_field->get_map_tofrom_data();
  std::list<SgUpirDataItemField *> map_alloc_data =
      data_field->get_map_alloc_data();

  for (std::list<SgUpirDataItemField *>::iterator iter = data_items.begin();
       iter != data_items.end(); iter++) {
    SgUpirDataItemField *data_item = isSgUpirDataItemField(*iter);
    ROSE_ASSERT(data_item != NULL);

    // check data sharing property
    SgOmpClause::upir_data_sharing_enum data_sharing_type =
        data_item->get_sharing_property();
    switch (data_sharing_type) {
    case SgOmpClause::e_upir_data_sharing_shared:
      shared_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_sharing_private:
      private_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_sharing_firstprivate:
      firstprivate_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_sharing_lastprivate:
      lastprivate_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_sharing_reduction:
      reduction_data.push_back(data_item);
      break;
    default:
      ROSE_ASSERT(0 && "At this point, every data item should have a specified "
                       "data sharing property.");
    }

    // check data mapping property
    SgOmpClause::upir_data_mapping_enum data_mapping_type =
        data_item->get_mapping_property();
    switch (data_mapping_type) {
    case SgOmpClause::e_upir_data_mapping_to:
      map_to_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_mapping_from:
      map_from_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_mapping_tofrom:
      map_tofrom_data.push_back(data_item);
      break;
    case SgOmpClause::e_upir_data_mapping_allocate:
      map_alloc_data.push_back(data_item);
      break;
    default:
      ROSE_ASSERT(0 && "At this point, every data item should have a specified "
                       "data mapping property.");
    }
  }

  data_field->set_shared_data(shared_data);
  data_field->set_private_data(private_data);
  data_field->set_firstprivate_data(firstprivate_data);
  data_field->set_lastprivate_data(lastprivate_data);
  data_field->set_reduction_data(reduction_data);

  data_field->set_map_to_data(map_to_data);
  data_field->set_map_from_data(map_from_data);
  data_field->set_map_tofrom_data(map_tofrom_data);
  data_field->set_map_alloc_data(map_alloc_data);
} // end collectUpirDataQueryInformation()

// set the UPIR parent and children of a given UPIR node
void setUpirRelationship(SgStatement *parent, SgStatement *child) {
  SgUpirBaseStatement *upir_parent = isSgUpirBaseStatement(parent);
  SgStatementPtrList &children = upir_parent->get_upir_children();
  children.push_back(child);

  SgUpirBaseStatement *upir_child = isSgUpirBaseStatement(child);
  upir_child->set_upir_parent(parent);
}

// search the UPIR parent of a given UPIR node, not its SgNode parent.
SgStatement *getUpirParent(SgStatement *node) {
  SgStatement *parent = isSgStatement(node->get_parent());
  while (parent != NULL) {
    if (isSgUpirBaseStatement(parent)) {
      break;
    }
    parent = isSgStatement(parent->get_parent());
  }
  return isSgUpirBaseStatement(parent);
}

// traverse the SgNode AST and fill the information of UPIR parent and
// children.
void createUpirStatementTree(SgSourceFile *file) {
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgUpirBaseStatement);
  Rose_STL_Container<SgNode *>::reverse_iterator node_list_iterator;
  for (node_list_iterator = node_list.rbegin();
       node_list_iterator != node_list.rend(); node_list_iterator++) {
    SgUpirBaseStatement *node = isSgUpirBaseStatement(*node_list_iterator);
    SgStatement *parent = getUpirParent(node);
    if (parent != NULL) {
      setUpirRelationship(parent, node);
    } else {
      node->set_upir_parent(parent);
    }
  }
}

void analyze_omp(SgSourceFile *file) {

  Rose_STL_Container<SgNode *> variant_directives =
      NodeQuery::querySubTree(file, V_SgOmpMetadirectiveStatement);
  Rose_STL_Container<SgNode *>::iterator node_list_iterator;
  for (node_list_iterator = variant_directives.begin();
       node_list_iterator != variant_directives.end(); node_list_iterator++) {
    SgUpirBaseStatement *node = isSgUpirBaseStatement(*node_list_iterator);
    ROSE_ASSERT(node != NULL);
    analyzeOmpMetadirective(node);
  }
  patchUpPrivateVariables(file); // the order of these two functions matter! We
                                 // want to patch up private variable first!
  patchUpFirstprivateVariables(file);

  patchUpImplicitMappingVariables(file);

  // convert firstprivate clause in target directive to map clause because
  // later only map clause will be lowered.
  // unifyUpirTaskMappingVariables(file);

  // Generate UPIR data fields based on data sharing properties
  // createUpirDataFields(file);

  // Generate UPIR data fields based on map clauses
  // createUpirMappingDataFields(file);

  // collectUpirDataQueryInformation(file);

  /*
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgUpirBaseStatement);
  for (node_list_iterator = node_list.begin();
       node_list_iterator != node_list.end(); node_list_iterator++) {
    SgUpirBaseStatement *node = isSgUpirBaseStatement(*node_list_iterator);
    ROSE_ASSERT(node != NULL);
    switch (node->variantT()) {
    case V_SgOmpForStatement: {
      normalizeUpirLoop(node);
      break;
    }
    default: {
      // std::cout << "Nothing happened.\n";
    }
    } // switch
  }
  */

  // after passes of analysis and normalization, add the information of UPIR
  // parent and children.
  // createUpirStatementTree(file);
}
} // namespace OmpSupport
