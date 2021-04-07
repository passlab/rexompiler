#include "sage3basic.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

namespace OmpSupport {

void analyzeOmpFor(SgNode *node) {
  ROSE_ASSERT(node != NULL);
  SgOmpForStatement *target1 = isSgOmpForStatement(node);
  SgOmpDoStatement *target2 = isSgOmpDoStatement(node);

  SgOmpClauseBodyStatement *target =
      (target1 != NULL ? (SgOmpClauseBodyStatement *)target1
                       : (SgOmpClauseBodyStatement *)target2);
  ROSE_ASSERT(target != NULL);

  SgScopeStatement *p_scope = target->get_scope();
  ROSE_ASSERT(p_scope != NULL);

  SgStatement *body = target->get_body();
  ROSE_ASSERT(body != NULL);
  // The OpenMP syntax requires that the omp for pragma is immediately followed
  // by the for loop.
  SgForStatement *for_loop = isSgForStatement(body);
  SgFortranDo *do_loop = isSgFortranDo(body);

  SgStatement *loop =
      (for_loop != NULL ? (SgStatement *)for_loop : (SgStatement *)do_loop);
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
} // end analyze omp for

//! Patch up private variables for omp for. The reason is that loop indices
//! should be private by default and this function will make this explicit. This
//! should happen before the actual translation is done.
int patchUpPrivateVariables(SgFile *file) {
  int result = 0;
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode *> nodeList =
      NodeQuery::querySubTree(file, V_SgOmpForStatement);
  Rose_STL_Container<SgNode *> nodeList2 =
      NodeQuery::querySubTree(file, V_SgOmpDoStatement);

  Rose_STL_Container<SgNode *> nodeList_merged(nodeList.size() +
                                               nodeList2.size());

  std::sort(nodeList.begin(), nodeList.end());
  std::sort(nodeList2.begin(), nodeList2.end());
  std::merge(nodeList.begin(), nodeList.end(), nodeList2.begin(),
             nodeList2.end(), nodeList_merged.begin());

  Rose_STL_Container<SgNode *>::iterator nodeListIterator =
      nodeList_merged.begin();
  // For each omp for/do statement
  for (; nodeListIterator != nodeList_merged.end(); ++nodeListIterator) {
    SgStatement *omp_loop = NULL;
    SgOmpForStatement *for_node = isSgOmpForStatement(*nodeListIterator);
    SgOmpDoStatement *do_node = isSgOmpDoStatement(*nodeListIterator);
    if (for_node)
      omp_loop = for_node;
    else if (do_node)
      omp_loop = do_node;
    else
      ROSE_ASSERT(false);
    result += patchUpPrivateVariables(omp_loop);
  } // end for omp for statments
  return result;
} // end patchUpPrivateVariables()

//! Collect threadprivate variables within the current project, return a set to
//! avoid duplicated elements
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
// constructs, up to and including the innermost enclosing parallel construct, is
// shared
// start_stmt is the start point to find enclosing OpenMP constructs. It is
// excluded as an enclosing construct for itself.
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
  // 2. a global variable: either a threadprivate variable or shared by default
  // 3. is a variable declared within an orphaned function: it is private to its
  // enclosing parallel region
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
#if 1
      cerr << "Error: OmpSupport::isSharedInEnclosingConstructs() \n Unhandled "
              "variables within an orphaned construct:"
           << endl;
      cerr << "SgInitializedName name = " << init_var->get_name().getString()
           << endl;
      dumpInfo(init_var);
      init_var->get_file_info()->display("tttt");
      ROSE_ASSERT(false);
#endif
    }
  }
  return result;
} // end isSharedInEnclosingConstructs()

//! Patch up firstprivate variables for omp task. The reason is that the
//! specification 3.0 defines rules for implicitly determined data-sharing
//! attributes and this function will make the implicit firstprivate variable of
//! omp task explicit.
/*
variables used in task block:

2.9.1.1 Data-sharing Attribute Rules for Variables Referenced in a Construct
Ref. OMP 3.0 page 79
A variable is firstprivate to the task (default) , if
** not explicitly specified by default(), shared(),private(), firstprivate()
clauses
** not shared in enclosing constructs

It should also satisfy the restriction defined in specification 3.0 page 93 TODO
* cannot be a variable which is part of another variable (as an array or
structure element)
* cannot be private, reduction
* must have an accessible, unambiguous copy constructor for the class type
* must not have a const-qualified type unless it is of class type with a mutable
member
* must not have an incomplete C/C++ type or a reference type
*
I decided to exclude variables which are used by addresses when recognizing
firstprivate variables The reason is that in real code, it is often to have
private variables first then use their address later.   Skipping the replacement
will result in wrong semantics. e.g. from Allan Porterfield void    create_seq(
double seed, double a )
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
   * replacing it with its local copy will not get the right, original address.
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
    __out_argv1__1527__[0] = ((void *)(&(*i)));// this is the right translation
    GOMP_task(OUT__1__1527__,&__out_argv1__1527__,0,4,4,1,0);
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
  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgOmpParallelStatement);
  Rose_STL_Container<SgNode *>::iterator iter = node_list.begin();
  for (iter = node_list.begin(); iter != node_list.end(); iter++) {
    SgOmpParallelStatement *target = isSgOmpParallelStatement(*iter);
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
      // Now it should be a shared variable
      addClauseVariable(init_var, target, V_SgOmpSharedClause);
      // std::cout << init_var->get_name() << "\n";
      result++;
    } // end for each variable reference
  }   // end for each SgOmpTaskStatement
  return result;
} // end patchUpImplicitMappingVariables()

void analyze_omp(SgSourceFile *file) {
  int shared_vars = patchUpImplicitMappingVariables(file);
  // std::cout << "New shared vars: " << shared_vars << "\n";

  patchUpPrivateVariables(file); // the order of these two functions matter! We
                                 // want to patch up private variable first!
  patchUpFirstprivateVariables(file);

  Rose_STL_Container<SgNode *> node_list =
      NodeQuery::querySubTree(file, V_SgStatement);
  Rose_STL_Container<SgNode *>::reverse_iterator node_list_iterator;
  for (node_list_iterator = node_list.rbegin();
       node_list_iterator != node_list.rend(); node_list_iterator++) {
    SgStatement *node = isSgStatement(*node_list_iterator);
    ROSE_ASSERT(node != NULL);
    switch (node->variantT()) {
    case V_SgOmpForStatement: {
      analyzeOmpFor(node);
      break;
    }
    default: {
      // std::cout << "Nothing happened.\n";
    }
    } // switch
  }
}
} // namespace OmpSupport
