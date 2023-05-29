// Put here code used to construct SgOmp* nodes
// Liao 10/8/2010
#include "sage3basic.h"
#include "rose_paths.h"
#include "astPostProcessing.h"
#include "sageBuilder.h"
#include "ompAstConstruction.h"

#include <tuple>

// the vector of pairs of OpenACC pragma and accparser IR.
static std::vector<std::pair<SgPragmaDeclaration *, OpenACCDirective *>>
    OpenACCIR_list;
OpenACCDirective *accparser_OpenACCIR;

std::map<SgPragmaDeclaration *, OpenMPDirective *> fortran_paired_pragma_dict;
std::vector<std::tuple<SgLocatedNode *, PreprocessingInfo *, OpenMPDirective *>>
    fortran_omp_pragma_list;

OpenMPDirective *ompparser_OpenMPIR;
static bool use_ompparser = false;
static bool use_accparser = false;

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

// Liao 4/23/2011, special function to copy file info of the original SgPragma
// or Fortran comments
bool copyStartFileInfo(SgNode *src, SgNode *dest) {
  bool result = false;
  ROSE_ASSERT(src && dest);
  // same src and dest, no copy is needed
  if (src == dest)
    return true;

  SgLocatedNode *lsrc = isSgLocatedNode(src);
  ROSE_ASSERT(lsrc);
  SgLocatedNode *ldest = isSgLocatedNode(dest);
  ROSE_ASSERT(ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if (lsrc->get_startOfConstruct()->get_filename() ==
          ldest->get_startOfConstruct()->get_filename() &&
      lsrc->get_startOfConstruct()->get_line() ==
          ldest->get_startOfConstruct()->get_line() &&
      lsrc->get_startOfConstruct()->get_col() ==
          ldest->get_startOfConstruct()->get_col())
    return true;

  Sg_File_Info *copy = new Sg_File_Info(*(lsrc->get_startOfConstruct()));
  ROSE_ASSERT(copy != NULL);

  // delete old start of construct
  Sg_File_Info *old_info = ldest->get_startOfConstruct();
  if (old_info)
    delete (old_info);

  ldest->set_startOfConstruct(copy);
  copy->set_parent(ldest);
  //  cout<<"debug: set ldest@"<<ldest <<" with file info @"<< copy <<endl;

  ROSE_ASSERT(lsrc->get_startOfConstruct()->get_filename() ==
              ldest->get_startOfConstruct()->get_filename());
  ROSE_ASSERT(lsrc->get_startOfConstruct()->get_line() ==
              ldest->get_startOfConstruct()->get_line());
  ROSE_ASSERT(lsrc->get_startOfConstruct()->get_col() ==
              ldest->get_startOfConstruct()->get_col());

  ROSE_ASSERT(lsrc->get_startOfConstruct()->get_filename() ==
              ldest->get_file_info()->get_filename());
  ROSE_ASSERT(lsrc->get_startOfConstruct()->get_line() ==
              ldest->get_file_info()->get_line());
  ROSE_ASSERT(lsrc->get_startOfConstruct()->get_col() ==
              ldest->get_file_info()->get_col());

  ROSE_ASSERT(ldest->get_file_info() == copy);
  // Adjustment for Fortran, the AST node attaching the Fortran comment will not
  // actual give out the accurate line number for the comment
  if (is_Fortran_language()) {
    int commentLine = ompparser_OpenMPIR->getLine();
    ldest->get_file_info()->set_line(commentLine);
  }

  return result;
}
// Liao 3/11/2013, special function to copy end file info of the original
// SgPragma or Fortran comments (src) to OpenMP node (dest) If the OpenMP node
// is a body statement, we have to use the body's end file info as the node's
// end file info.
bool copyEndFileInfo(SgNode *src, SgNode *dest) {
  bool result = false;
  ROSE_ASSERT(src && dest);

  if (isSgOmpBodyStatement(dest))
    src = isSgOmpBodyStatement(dest)->get_body();

  // same src and dest, no copy is needed
  if (src == dest)
    return true;

  SgLocatedNode *lsrc = isSgLocatedNode(src);
  ROSE_ASSERT(lsrc);
  SgLocatedNode *ldest = isSgLocatedNode(dest);
  ROSE_ASSERT(ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if (lsrc->get_endOfConstruct()->get_filename() ==
          ldest->get_endOfConstruct()->get_filename() &&
      lsrc->get_endOfConstruct()->get_line() ==
          ldest->get_endOfConstruct()->get_line() &&
      lsrc->get_endOfConstruct()->get_col() ==
          ldest->get_endOfConstruct()->get_col())
    return true;

  Sg_File_Info *copy = new Sg_File_Info(*(lsrc->get_endOfConstruct()));
  ROSE_ASSERT(copy != NULL);

  // delete old start of construct
  Sg_File_Info *old_info = ldest->get_endOfConstruct();
  if (old_info)
    delete (old_info);

  ldest->set_endOfConstruct(copy);
  copy->set_parent(ldest);

  ROSE_ASSERT(lsrc->get_endOfConstruct()->get_filename() ==
              ldest->get_endOfConstruct()->get_filename());
  ROSE_ASSERT(lsrc->get_endOfConstruct()->get_line() ==
              ldest->get_endOfConstruct()->get_line());
  ROSE_ASSERT(lsrc->get_endOfConstruct()->get_col() ==
              ldest->get_endOfConstruct()->get_col());
  ROSE_ASSERT(ldest->get_endOfConstruct() == copy);

  return result;
}

namespace OmpSupport {
// an internal data structure to avoid redundant AST traversal to find OpenMP
// pragmas
static std::list<SgPragmaDeclaration *> omp_pragma_list;

// the vector of pairs of OpenMP pragma and Ompparser IR.
static std::vector<std::pair<SgPragmaDeclaration *, OpenMPDirective *>>
    OpenMPIR_list;

// Clause node builders
//----------------------------------------------------------
// Sara Royuela ( Nov 2, 2012 ): Check for clause parameters that can be defined
// in macros This adds support for the use of macro definitions in OpenMP
// clauses We need a traversal over SgExpression to support macros in any
// position of an "assignment_expr" F.i.:   #define THREADS_1 16
//         #define THREADS_2 8
//         int main( int arg, char** argv ) {
//         #pragma omp parallel num_threads( THREADS_1 + THREADS_2 )
//           {}
//         }
SgVarRefExpVisitor::SgVarRefExpVisitor() : expressions() {}

std::vector<SgExpression *> SgVarRefExpVisitor::get_expressions() {
  return expressions;
}

void SgVarRefExpVisitor::visit(SgNode *node) {
  SgExpression *expr = isSgVarRefExp(node);
  if (expr != NULL) {
    expressions.push_back(expr);
  }
}

SgExpression *
replace_expression_with_macro_value(std::string define_macro,
                                    SgExpression *old_exp, bool &macro_replaced,
                                    omp_construct_enum clause_type) {
  SgExpression *newExp = old_exp;
  // Parse the macro: we are only interested in macros with the form #define
  // MACRO_NAME MACRO_VALUE, the constant macro
  size_t parenthesis = define_macro.find("(");
  if (parenthesis == string::npos) { // Non function macro, constant macro
    unsigned int macroNameInitPos =
        (unsigned int)(define_macro.find("define")) + 6;
    while (macroNameInitPos < define_macro.size() &&
           define_macro[macroNameInitPos] == ' ')
      macroNameInitPos++;
    unsigned int macroNameEndPos = define_macro.find(" ", macroNameInitPos);
    std::string macroName = define_macro.substr(
        macroNameInitPos, macroNameEndPos - macroNameInitPos);

    if (macroName == isSgVarRefExp(old_exp)
                         ->get_symbol()
                         ->get_name()
                         .getString()) { // Clause is defined in a macro
      size_t comma = define_macro.find(",");
      if (comma == string::npos) // Macros like "#define MACRO_NAME VALUE1,
                                 // VALUE2" are not accepted
      { // We create here an expression with the value of the clause defined in
        // the macro
        unsigned int macroValueInitPos = macroNameEndPos + 1;
        while (macroValueInitPos < define_macro.size() &&
               define_macro[macroValueInitPos] == ' ')
          macroValueInitPos++;
        unsigned int macroValueEndPos = macroValueInitPos;
        while (macroValueEndPos < define_macro.size() &&
               define_macro[macroValueEndPos] != ' ' &&
               define_macro[macroValueEndPos] != '\n')
          macroValueEndPos++;
        std::string macroValue = define_macro.substr(
            macroValueInitPos, macroValueEndPos - macroValueInitPos);

        // Check whether the value is a valid integer
        std::string::const_iterator it = macroValue.begin();
        while (it != macroValue.end() && std::isdigit(*it))
          ++it;
        ROSE_ASSERT(!macroValue.empty() && it == macroValue.end());

        newExp = buildIntVal(atoi(macroValue.c_str()));
        if (!isSgPragmaDeclaration(old_exp->get_parent()))
          replaceExpression(old_exp, newExp);
        macro_replaced = true;
      }
    }
  }
  return newExp;
}

SgExpression *checkOmpExpressionClause(SgExpression *clause_expression,
                                       SgGlobal *global,
                                       omp_construct_enum clause_type) {
  SgExpression *newExp = clause_expression;
  // ordered (n): optional (n)
  if (clause_expression == NULL && clause_type == e_ordered_clause)
    return NULL;
  ROSE_ASSERT(clause_expression != NULL);
  bool returnNewExpression = false;
  if (isSgTypeUnknown(clause_expression->get_type())) {
    SgVarRefExpVisitor v;
    v.traverse(clause_expression, preorder);
    std::vector<SgExpression *> expressions = v.get_expressions();
    if (!expressions.empty()) {
      if (expressions.size() == 1) { // create the new expression and return it
        // otherwise, replace the expression and return the original, which is
        // now modified
        returnNewExpression = true;
      }

      bool macroReplaced;
      SgDeclarationStatementPtrList &declarations = global->get_declarations();
      while (!expressions.empty()) {
        macroReplaced = false;
        SgExpression *oldExp = expressions.back();
        for (SgDeclarationStatementPtrList::iterator declIt =
                 declarations.begin();
             declIt != declarations.end() && !macroReplaced; ++declIt) {
          SgDeclarationStatement *declaration = *declIt;
          AttachedPreprocessingInfoType *preprocInfo =
              declaration->getAttachedPreprocessingInfo();
          if (preprocInfo !=
              NULL) { // There is preprocessed info attached to the current node
            for (AttachedPreprocessingInfoType::iterator infoIt =
                     preprocInfo->begin();
                 infoIt != preprocInfo->end() && !macroReplaced; infoIt++) {
              if ((*infoIt)->getTypeOfDirective() ==
                  PreprocessingInfo::CpreprocessorDefineDeclaration) {
                newExp = replace_expression_with_macro_value(
                    (*infoIt)->getString(), oldExp, macroReplaced, clause_type);
              }
            }
          }
        }

        // When a macro is defined in a header without any statement, the
        // preprocessed information is attached to the SgFile
        if (!macroReplaced) {
          SgProject *project = SageInterface::getProject();
          int nFiles = project->numberOfFiles();
          for (int fileIt = 0; fileIt < nFiles && !macroReplaced; fileIt++) {
            SgFile &file = project->get_file(fileIt);
            ROSEAttributesListContainerPtr filePreprocInfo =
                file.get_preprocessorDirectivesAndCommentsList();
            if (filePreprocInfo != NULL) {
              std::map<std::string, ROSEAttributesList *> preprocInfoMap =
                  filePreprocInfo->getList();
              for (std::map<std::string, ROSEAttributesList *>::iterator mapIt =
                       preprocInfoMap.begin();
                   mapIt != preprocInfoMap.end() && !macroReplaced; mapIt++) {
                std::vector<PreprocessingInfo *> preprocInfoList =
                    mapIt->second->getList();
                for (std::vector<PreprocessingInfo *>::iterator infoIt =
                         preprocInfoList.begin();
                     infoIt != preprocInfoList.end() && !macroReplaced;
                     infoIt++) {
                  if ((*infoIt)->getTypeOfDirective() ==
                      PreprocessingInfo::CpreprocessorDefineDeclaration) {
                    newExp = replace_expression_with_macro_value(
                        (*infoIt)->getString(), oldExp, macroReplaced,
                        clause_type);
                  }
                }
              }
            }
          }
        }

        expressions.pop_back();
      }
    } else {
      printf("error in checkOmpExpressionClause(): no expression found in an "
             "expression clause\n");
      ROSE_ASSERT(false);
    }
  }

  return (returnNewExpression ? newExp : clause_expression);
}

//! A helper function to convert OpenMPIfClause modifier to SgClause if modifier
static SgOmpClause::omp_if_modifier_enum
toSgOmpClauseIfModifier(OpenMPIfClauseModifier modifier) {
  SgOmpClause::omp_if_modifier_enum result;
  switch (modifier) {
  case OMPC_IF_MODIFIER_parallel: {
    result = SgOmpClause::e_omp_if_parallel;
    break;
  }
  case OMPC_IF_MODIFIER_simd: {
    result = SgOmpClause::e_omp_if_simd;
    break;
  }
  case OMPC_IF_MODIFIER_cancel: {
    result = SgOmpClause::e_omp_if_cancel;
    break;
  }
  case OMPC_IF_MODIFIER_taskloop: {
    result = SgOmpClause::e_omp_if_taskloop;
    break;
  }
  case OMPC_IF_MODIFIER_target_enter_data: {
    result = SgOmpClause::e_omp_if_target_enter_data;
    break;
  }
  case OMPC_IF_MODIFIER_target_exit_data: {
    result = SgOmpClause::e_omp_if_target_exit_data;
    break;
  }
  case OMPC_IF_MODIFIER_task: {
    result = SgOmpClause::e_omp_if_task;
    break;
  }
  case OMPC_IF_MODIFIER_target_data: {
    result = SgOmpClause::e_omp_if_target_data;
    break;
  }
  case OMPC_IF_MODIFIER_target: {
    result = SgOmpClause::e_omp_if_target;
    break;
  }
  case OMPC_IF_MODIFIER_target_update: {
    result = SgOmpClause::e_omp_if_target_update;
    break;
  }
  case OMPC_IF_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_if_modifier_unknown;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for if modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_lastprivate_modifier_enum
toSgOmpClauseLastprivateModifier(OpenMPLastprivateClauseModifier modifier) {
  SgOmpClause::omp_lastprivate_modifier_enum result =
      SgOmpClause::e_omp_lastprivate_modifier_unspecified;
  switch (modifier) {
  case OMPC_LASTPRIVATE_MODIFIER_conditional: {
    result = SgOmpClause::e_omp_lastprivate_conditional;
    break;
  }
  case OMPC_LASTPRIVATE_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_lastprivate_modifier_unspecified;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for lastprivate modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_device_modifier_enum
toSgOmpClauseDeviceModifier(OpenMPDeviceClauseModifier modifier) {
  SgOmpClause::omp_device_modifier_enum result =
      SgOmpClause::e_omp_device_modifier_unspecified;
  switch (modifier) {
  case OMPC_DEVICE_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_device_modifier_unspecified;
    break;
  }
  case OMPC_DEVICE_MODIFIER_ancestor: {
    result = SgOmpClause::e_omp_device_modifier_ancestor;
    break;
  }
  case OMPC_DEVICE_MODIFIER_device_num: {
    result = SgOmpClause::e_omp_device_modifier_device_num;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for device modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_schedule_modifier_enum
toSgOmpClauseScheduleModifier(OpenMPScheduleClauseModifier modifier) {
  SgOmpClause::omp_schedule_modifier_enum result =
      SgOmpClause::e_omp_schedule_modifier_unspecified;
  switch (modifier) {
  case OMPC_SCHEDULE_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_schedule_modifier_unspecified;
    break;
  }
  case OMPC_SCHEDULE_MODIFIER_monotonic: {
    result = SgOmpClause::e_omp_schedule_modifier_monotonic;
    break;
  }
  case OMPC_SCHEDULE_MODIFIER_nonmonotonic: {
    result = SgOmpClause::e_omp_schedule_modifier_nonmonotonic;
    break;
  }
  case OMPC_SCHEDULE_MODIFIER_simd: {
    result = SgOmpClause::e_omp_schedule_modifier_simd;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for schedule modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_schedule_kind_enum
toSgOmpClauseScheduleKind(OpenMPScheduleClauseKind kind) {
  SgOmpClause::omp_schedule_kind_enum result =
      SgOmpClause::e_omp_schedule_kind_unspecified;
  switch (kind) {
  case OMPC_SCHEDULE_KIND_unspecified: {
    result = SgOmpClause::e_omp_schedule_kind_unspecified;
    break;
  }
  case OMPC_SCHEDULE_KIND_static: {
    result = SgOmpClause::e_omp_schedule_kind_static;
    break;
  }
  case OMPC_SCHEDULE_KIND_dynamic: {
    result = SgOmpClause::e_omp_schedule_kind_dynamic;
    break;
  }
  case OMPC_SCHEDULE_KIND_guided: {
    result = SgOmpClause::e_omp_schedule_kind_guided;
    break;
  }
  case OMPC_SCHEDULE_KIND_auto: {
    result = SgOmpClause::e_omp_schedule_kind_auto;
    break;
  }
  case OMPC_SCHEDULE_KIND_runtime: {
    result = SgOmpClause::e_omp_schedule_kind_runtime;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for schedule kind "
           "conversion:%d\n",
           kind);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_defaultmap_behavior_enum
toSgOmpClauseDefaultmapBehavior(OpenMPDefaultmapClauseBehavior behavior) {
  SgOmpClause::omp_defaultmap_behavior_enum result =
      SgOmpClause::e_omp_defaultmap_behavior_unspecified;
  switch (behavior) {
  case OMPC_DEFAULTMAP_BEHAVIOR_alloc: {
    result = SgOmpClause::e_omp_defaultmap_behavior_alloc;
    break;
  }
  case OMPC_DEFAULTMAP_BEHAVIOR_to: {
    result = SgOmpClause::e_omp_defaultmap_behavior_to;
    break;
  }
  case OMPC_DEFAULTMAP_BEHAVIOR_from: {
    result = SgOmpClause::e_omp_defaultmap_behavior_from;
    break;
  }
  case OMPC_DEFAULTMAP_BEHAVIOR_tofrom: {
    result = SgOmpClause::e_omp_defaultmap_behavior_tofrom;
    break;
  }
  case OMPC_DEFAULTMAP_BEHAVIOR_firstprivate: {
    result = SgOmpClause::e_omp_defaultmap_behavior_firstprivate;
    break;
  }
  case OMPC_DEFAULTMAP_BEHAVIOR_none: {
    result = SgOmpClause::e_omp_defaultmap_behavior_none;
    break;
  }
  case OMPC_DEFAULTMAP_BEHAVIOR_default: {
    result = SgOmpClause::e_omp_defaultmap_behavior_default;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for defaultmap behavior "
           "conversion:%d\n",
           behavior);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_map_operator_enum
toSgOmpClauseMapOperator(OpenMPMapClauseType at_op) {
  SgOmpClause::omp_map_operator_enum result = SgOmpClause::e_omp_map_unknown;
  switch (at_op) {
  case OMPC_MAP_TYPE_tofrom:
  case OMPC_MAP_TYPE_unspecified: {
    result = SgOmpClause::e_omp_map_tofrom;
    break;
  }
  case OMPC_MAP_TYPE_to: {
    result = SgOmpClause::e_omp_map_to;
    break;
  }
  case OMPC_MAP_TYPE_from: {
    result = SgOmpClause::e_omp_map_from;
    break;
  }
  case OMPC_MAP_TYPE_alloc: {
    result = SgOmpClause::e_omp_map_alloc;
    break;
  }
  default: {
    // printf("error: unacceptable omp construct enum for map operator
    // conversion:%s\n", OmpSupport::toString(at_op).c_str());
    ROSE_ASSERT(false);
    break;
  }
  }
  ROSE_ASSERT(result != SgOmpClause::e_omp_map_unknown);
  return result;
}

static SgOmpClause::omp_defaultmap_category_enum
toSgOmpClauseDefaultmapCategory(OpenMPDefaultmapClauseCategory category) {
  SgOmpClause::omp_defaultmap_category_enum result =
      SgOmpClause::e_omp_defaultmap_category_unspecified;
  switch (category) {
  case OMPC_DEFAULTMAP_CATEGORY_unspecified: {
    result = SgOmpClause::e_omp_defaultmap_category_unspecified;
    break;
  }
  case OMPC_DEFAULTMAP_CATEGORY_scalar: {
    result = SgOmpClause::e_omp_defaultmap_category_scalar;
    break;
  }
  case OMPC_DEFAULTMAP_CATEGORY_aggregate: {
    result = SgOmpClause::e_omp_defaultmap_category_aggregate;
    break;
  }
  case OMPC_DEFAULTMAP_CATEGORY_pointer: {
    result = SgOmpClause::e_omp_defaultmap_category_pointer;
    break;
  }
  case OMPC_DEFAULTMAP_CATEGORY_allocatable: {
    result = SgOmpClause::e_omp_defaultmap_category_allocatable;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for defaultmap category "
           "conversion:%d\n",
           category);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_dist_schedule_kind_enum
toSgOmpClauseDistScheduleKind(OpenMPDistScheduleClauseKind kind) {
  SgOmpClause::omp_dist_schedule_kind_enum result =
      SgOmpClause::e_omp_dist_schedule_kind_unspecified;
  switch (kind) {
  case OMPC_DIST_SCHEDULE_KIND_static: {
    result = SgOmpClause::e_omp_dist_schedule_kind_static;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for dist_schedule kind "
           "conversion:%d\n",
           kind);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

static SgOmpClause::omp_linear_modifier_enum
toSgOmpClauseLinearModifier(OpenMPLinearClauseModifier modifier) {
  SgOmpClause::omp_linear_modifier_enum result =
      SgOmpClause::e_omp_linear_modifier_unspecified;
  switch (modifier) {
  case OMPC_LINEAR_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_linear_modifier_unspecified;
    break;
  }
  case OMPC_LINEAR_MODIFIER_ref: {
    result = SgOmpClause::e_omp_linear_modifier_ref;
    break;
  }
  case OMPC_LINEAR_MODIFIER_val: {
    result = SgOmpClause::e_omp_linear_modifier_val;
    break;
  }
  case OMPC_LINEAR_MODIFIER_uval: {
    result = SgOmpClause::e_omp_linear_modifier_uval;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for linear modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

//! A helper function to convert OpenMPIR reduction modifier to SgClause
//! reduction modifier
static SgOmpClause::omp_reduction_modifier_enum
toSgOmpClauseReductionModifier(OpenMPReductionClauseModifier modifier) {
  SgOmpClause::omp_reduction_modifier_enum result;
  switch (modifier) {
  case OMPC_REDUCTION_MODIFIER_inscan: {
    result = SgOmpClause::e_omp_reduction_inscan;
    break;
  }
  case OMPC_REDUCTION_MODIFIER_task: {
    result = SgOmpClause::e_omp_reduction_task;
    break;
  }
  case OMPC_REDUCTION_MODIFIER_default: {
    result = SgOmpClause::e_omp_reduction_default;
    break;
  }
  case OMPC_REDUCTION_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_reduction_modifier_unknown;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for reduction modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
  }
  }
  return result;
}

//! A helper function to convert OpenMPIR reduction identifier to SgClause
//! reduction identifier
static SgOmpClause::omp_reduction_identifier_enum
toSgOmpClauseReductionIdentifier(OpenMPReductionClauseIdentifier identifier) {
  SgOmpClause::omp_reduction_identifier_enum result =
      SgOmpClause::e_omp_reduction_unknown;
  switch (identifier) {
  case OMPC_REDUCTION_IDENTIFIER_plus: //+
  {
    result = SgOmpClause::e_omp_reduction_plus;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_mul: //*
  {
    result = SgOmpClause::e_omp_reduction_mul;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_minus: // -
  {
    result = SgOmpClause::e_omp_reduction_minus;
    break;
  }
    // C/C++ only
  case OMPC_REDUCTION_IDENTIFIER_bitand: // &
  {
    result = SgOmpClause::e_omp_reduction_bitand;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_bitor: // |
  {
    result = SgOmpClause::e_omp_reduction_bitor;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_bitxor: // ^
  {
    result = SgOmpClause::e_omp_reduction_bitxor;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_logand: // &&
  {
    result = SgOmpClause::e_omp_reduction_logand;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_logor: // ||
  {
    result = SgOmpClause::e_omp_reduction_logor;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_max: {
    result = SgOmpClause::e_omp_reduction_max;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_min: {
    result = SgOmpClause::e_omp_reduction_min;
    break;
  }
  case OMPC_REDUCTION_IDENTIFIER_user: {
    result = SgOmpClause::e_omp_reduction_user_defined_identifier;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for reduction operator "
           "conversion:%d\n",
           identifier);
    ROSE_ASSERT(false);
    break;
  }
  }
  ROSE_ASSERT(result != SgOmpClause::e_omp_reduction_unknown);
  return result;
}

//! A helper function to convert OpenMPIR reduction identifier to SgClause
//! reduction identifier
static SgOmpClause::omp_in_reduction_identifier_enum
toSgOmpClauseInReductionIdentifier(
    OpenMPInReductionClauseIdentifier identifier) {
  SgOmpClause::omp_in_reduction_identifier_enum result =
      SgOmpClause::e_omp_in_reduction_identifier_unspecified;
  switch (identifier) {
  case OMPC_IN_REDUCTION_IDENTIFIER_plus: //+
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_plus;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_mul: //*
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_mul;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_minus: // -
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_minus;
    break;
  }
    // C/C++ only
  case OMPC_IN_REDUCTION_IDENTIFIER_bitand: // &
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_bitand;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_bitor: // |
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_bitor;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_bitxor: // ^
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_bitxor;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_logand: // &&
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_logand;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_logor: // ||
  {
    result = SgOmpClause::e_omp_in_reduction_identifier_logor;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_max: {
    result = SgOmpClause::e_omp_in_reduction_identifier_max;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_min: {
    result = SgOmpClause::e_omp_in_reduction_identifier_min;
    break;
  }
  case OMPC_IN_REDUCTION_IDENTIFIER_user: {
    result = SgOmpClause::e_omp_in_reduction_user_defined_identifier;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for in_reduction operator "
           "conversion:%d\n",
           identifier);
    ROSE_ASSERT(false);
    break;
  }
  }
  ROSE_ASSERT(result != SgOmpClause::e_omp_in_reduction_identifier_unspecified);
  return result;
}

//! A helper function to convert OpenMPIR reduction identifier to SgClause
//! reduction identifier
static SgOmpClause::omp_task_reduction_identifier_enum
toSgOmpClauseTaskReductionIdentifier(
    OpenMPTaskReductionClauseIdentifier identifier) {
  SgOmpClause::omp_task_reduction_identifier_enum result =
      SgOmpClause::e_omp_task_reduction_identifier_unspecified;
  switch (identifier) {
  case OMPC_TASK_REDUCTION_IDENTIFIER_plus: //+
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_plus;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_mul: //*
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_mul;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_minus: // -
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_minus;
    break;
  }
    // C/C++ only
  case OMPC_TASK_REDUCTION_IDENTIFIER_bitand: // &
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_bitand;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_bitor: // |
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_bitor;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_bitxor: // ^
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_bitxor;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_logand: // &&
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_logand;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_logor: // ||
  {
    result = SgOmpClause::e_omp_task_reduction_identifier_logor;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_max: {
    result = SgOmpClause::e_omp_task_reduction_identifier_max;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_min: {
    result = SgOmpClause::e_omp_task_reduction_identifier_min;
    break;
  }
  case OMPC_TASK_REDUCTION_IDENTIFIER_user: {
    result = SgOmpClause::e_omp_task_reduction_user_defined_identifier;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for task_reduction operator "
           "conversion:%d\n",
           identifier);
    ROSE_ASSERT(false);
    break;
  }
  }
  ROSE_ASSERT(result !=
              SgOmpClause::e_omp_task_reduction_identifier_unspecified);
  return result;
}

//! A helper function to convert OpenMPIR ALLOCATE allocator to SgClause
//! ALLOCATE modifier
static SgOmpClause::omp_allocate_modifier_enum
toSgOmpClauseAllocateAllocator(OpenMPAllocateClauseAllocator allocator) {
  SgOmpClause::omp_allocate_modifier_enum result;
  switch (allocator) {
  case OMPC_ALLOCATE_ALLOCATOR_default: {
    result = SgOmpClause::e_omp_allocate_default_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_large_cap: {
    result = SgOmpClause::e_omp_allocate_large_cap_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_cons_mem: {
    result = SgOmpClause::e_omp_allocate_const_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_high_bw: {
    result = SgOmpClause::e_omp_allocate_high_bw_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_low_lat: {
    result = SgOmpClause::e_omp_allocate_low_lat_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_cgroup: {
    result = SgOmpClause::e_omp_allocate_cgroup_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_pteam: {
    result = SgOmpClause::e_omp_allocate_pteam_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_thread: {
    result = SgOmpClause::e_omp_allocate_thread_mem_alloc;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_user: {
    result = SgOmpClause::e_omp_allocate_user_defined_modifier;
    break;
  }
  case OMPC_ALLOCATE_ALLOCATOR_unspecified: {
    result = SgOmpClause::e_omp_allocate_modifier_unknown;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for allocate modifier "
           "conversion:%d\n",
           allocator);
    ROSE_ASSERT(false);
    break;
  }
  }

  return result;
}

//! A helper function to convert OpenMPIR ALLOCATOR allocator to SgClause
//! ALLOCATOR modifier
static SgOmpClause::omp_allocator_modifier_enum
toSgOmpClauseAllocatorAllocator(OpenMPAllocatorClauseAllocator allocator) {
  SgOmpClause::omp_allocator_modifier_enum result;
  switch (allocator) {
  case OMPC_ALLOCATOR_ALLOCATOR_default: {
    result = SgOmpClause::e_omp_allocator_default_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_large_cap: {
    result = SgOmpClause::e_omp_allocator_large_cap_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_cons_mem: {
    result = SgOmpClause::e_omp_allocator_const_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_high_bw: {
    result = SgOmpClause::e_omp_allocator_high_bw_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_low_lat: {
    result = SgOmpClause::e_omp_allocator_low_lat_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_cgroup: {
    result = SgOmpClause::e_omp_allocator_cgroup_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_pteam: {
    result = SgOmpClause::e_omp_allocator_pteam_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_thread: {
    result = SgOmpClause::e_omp_allocator_thread_mem_alloc;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_user: {
    result = SgOmpClause::e_omp_allocator_user_defined_modifier;
    break;
  }
  case OMPC_ALLOCATOR_ALLOCATOR_unknown: {
    result = SgOmpClause::e_omp_allocator_modifier_unknown;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for allocator modifier "
           "conversion:%d\n",
           allocator);
    ROSE_ASSERT(false);
    break;
  }
  }

  return result;
}

//! A helper function to convert OpenMPIR TO kind to SgClause TO kind
static SgOmpClause::omp_to_kind_enum
toSgOmpClauseToKind(OpenMPToClauseKind kind) {
  SgOmpClause::omp_to_kind_enum result;
  switch (kind) {
  case OMPC_TO_mapper: {
    result = SgOmpClause::e_omp_to_kind_mapper;
    break;
  }

  case OMPC_TO_unspecified: {
    result = SgOmpClause::e_omp_to_kind_unknown;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for TO kind conversion:%d\n",
           kind);
    ROSE_ASSERT(false);
    break;
  }
  }

  return result;
}

//! A helper function to convert OpenMPIR FROM kind to SgClause FROM kind
static SgOmpClause::omp_from_kind_enum
toSgOmpClauseFromKind(OpenMPFromClauseKind kind) {
  SgOmpClause::omp_from_kind_enum result;
  switch (kind) {
  case OMPC_FROM_mapper: {
    result = SgOmpClause::e_omp_from_kind_mapper;
    break;
  }

  case OMPC_FROM_unspecified: {
    result = SgOmpClause::e_omp_from_kind_unknown;
    break;
  }
  default: {
    printf(
        "error: unacceptable omp construct enum for FROM kind conversion:%d\n",
        kind);
    ROSE_ASSERT(false);
    break;
  }
  }

  return result;
}

//! A helper function to convert OpenMPIR uses_allocator allocator to SgClause
//! uses_allocator allocator
static SgOmpClause::omp_uses_allocators_allocator_enum
toSgOmpClauseUsesAllocatorsAllocator(
    OpenMPUsesAllocatorsClauseAllocator allocator) {
  SgOmpClause::omp_uses_allocators_allocator_enum result;
  switch (allocator) {
  case OMPC_USESALLOCATORS_ALLOCATOR_default: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_default_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_large_cap: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_large_cap_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_cons_mem: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_const_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_high_bw: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_high_bw_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_low_lat: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_low_lat_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_cgroup: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_cgroup_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_pteam: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_pteam_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_thread: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_thread_mem_alloc;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_user: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_user_defined;
    break;
  }
  case OMPC_USESALLOCATORS_ALLOCATOR_unknown: {
    result = SgOmpClause::e_omp_uses_allocators_allocator_unknown;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for allocator modifier "
           "conversion:%d\n",
           allocator);
    ROSE_ASSERT(false);
    break;
  }
  }

  return result;
}

static SgOmpClause::omp_depobj_modifier_enum
toSgOmpClauseDepobjModifierType(OpenMPDepobjUpdateClauseDependeceType type) {
  SgOmpClause::omp_depobj_modifier_enum result =
      SgOmpClause::e_omp_depobj_modifier_unknown;
  switch (type) {
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_in: {
    result = SgOmpClause::e_omp_depobj_modifier_in;
    break;
  }
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_out: {
    result = SgOmpClause::e_omp_depobj_modifier_out;
    break;
  }
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_inout: {
    result = SgOmpClause::e_omp_depobj_modifier_inout;
    break;
  }
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_mutexinoutset: {
    result = SgOmpClause::e_omp_depobj_modifier_mutexinoutset;
    break;
  }
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_depobj: {
    result = SgOmpClause::e_omp_depobj_modifier_depobj;
    break;
  }
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_sink: {
    result = SgOmpClause::e_omp_depobj_modifier_sink;
    break;
  }
  case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_source: {
    result = SgOmpClause::e_omp_depobj_modifier_source;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for dependence type "
           "conversion:%d\n",
           type);
    ROSE_ASSERT(false);
    break;
  }
  }
  return result;
}

static SgOmpClause::omp_dependence_type_enum
toSgOmpClauseDependenceType(OpenMPDependClauseType type) {
  SgOmpClause::omp_dependence_type_enum result =
      SgOmpClause::e_omp_depend_unspecified;
  switch (type) {
  case OMPC_DEPENDENCE_TYPE_in: {
    result = SgOmpClause::e_omp_depend_in;
    break;
  }
  case OMPC_DEPENDENCE_TYPE_out: {
    result = SgOmpClause::e_omp_depend_out;
    break;
  }
  case OMPC_DEPENDENCE_TYPE_inout: {
    result = SgOmpClause::e_omp_depend_inout;
    break;
  }
  case OMPC_DEPENDENCE_TYPE_mutexinoutset: {
    result = SgOmpClause::e_omp_depend_mutexinoutset;
    break;
  }
  case OMPC_DEPENDENCE_TYPE_depobj: {
    result = SgOmpClause::e_omp_depend_depobj;
    break;
  }
  case OMPC_DEPENDENCE_TYPE_source: {
    result = SgOmpClause::e_omp_depend_source;
    break;
  }
  case OMPC_DEPENDENCE_TYPE_sink: {
    result = SgOmpClause::e_omp_depend_sink;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for dependence type "
           "conversion:%d\n",
           type);
    ROSE_ASSERT(false);
    break;
  }
  }
  return result;
}

static SgOmpClause::omp_depend_modifier_enum
toSgOmpClauseDependModifier(OpenMPDependClauseModifier modifier) {
  SgOmpClause::omp_depend_modifier_enum result =
      SgOmpClause::e_omp_depend_modifier_unspecified;
  switch (modifier) {
  case OMPC_DEPEND_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_depend_modifier_unspecified;
    break;
  }
  case OMPC_DEPEND_MODIFIER_iterator: {
    result = SgOmpClause::e_omp_depend_modifier_iterator;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for depend modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
    break;
  }
  }
  return result;
}

static SgOmpClause::omp_affinity_modifier_enum
toSgOmpClauseAffinityModifier(OpenMPAffinityClauseModifier modifier) {
  SgOmpClause::omp_affinity_modifier_enum result =
      SgOmpClause::e_omp_affinity_modifier_unspecified;
  switch (modifier) {
  case OMPC_AFFINITY_MODIFIER_unspecified: {
    result = SgOmpClause::e_omp_affinity_modifier_unspecified;
    break;
  }
  case OMPC_AFFINITY_MODIFIER_iterator: {
    result = SgOmpClause::e_omp_affinity_modifier_iterator;
    break;
  }
  default: {
    printf("error: unacceptable omp construct enum for affinity modifier "
           "conversion:%d\n",
           modifier);
    ROSE_ASSERT(false);
    break;
  }
  }
  return result;
}

//! Convert omp_pragma_list to SgOmpxxx nodes
void OpenMPIRToSageAST(SgSourceFile *sageFilePtr) {
  list<SgPragmaDeclaration *>::reverse_iterator
      iter; // bottom up handling for nested cases
  ROSE_ASSERT(sageFilePtr != NULL);
  int OpenMPIR_index = OpenMPIR_list.size() - 1;
  int OpenACCIR_index = OpenACCIR_list.size() - 1;
  for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend();
       iter++) {
    // Liao, 11/18/2009
    // It is possible that several source files showing up in a single
    // compilation line We have to check if the pragma declaration's file
    // information matches the current file being processed Otherwise we will
    // process the same pragma declaration multiple times!!
    SgPragmaDeclaration *decl = *iter;
    // Liao, 2/8/2010
    // Some pragmas are set to "transformation generated" when we fix scopes for
    // some pragma under single statement block e.g if ()
    //      #pragma
    //        do_sth()
    //  will be changed to
    //     if ()
    //     {
    //       #pragma
    //        do_sth()
    //     }
    // So we process a pragma if it is either within the same file or marked as
    // transformation
    if (decl->get_file_info()->get_filename() !=
            sageFilePtr->get_file_info()->get_filename() &&
        !(decl->get_file_info()->isTransformation()))
      continue;

    if (getEnclosingSourceFile(decl) != sageFilePtr)
      continue;

    if (OpenMPIR_list.size() != 0) {
      convertDirective(OpenMPIR_list[OpenMPIR_index]);
      OpenMPIR_index--;
    } else {
      convertOpenACCDirective(OpenACCIR_list[OpenACCIR_index]);
      OpenACCIR_index--;
    };

  } // end for (omp_pragma_list)
}

//! A helper function to ensure a sequence statements either has only one
//! statement
//  or all are put under a single basic block.
//  begin_decl is the begin directive which is immediately in front of the list
//  of statements Return the single statement or the basic block. This function
//  is used to wrap all statement between begin and end Fortran directives into
//  a block, if necessary(more than one statement)
static SgStatement *
ensureSingleStmtOrBasicBlock(SgPragmaDeclaration *begin_decl,
                             const std::vector<SgStatement *> &stmt_vec) {
  ROSE_ASSERT(begin_decl != NULL);
  SgStatement *result = NULL;
  ROSE_ASSERT(stmt_vec.size() > 0);
  if (stmt_vec.size() == 1) {
    result = stmt_vec[0];
    ROSE_ASSERT(getNextStatement(begin_decl) == result);
  } else {
    result = buildBasicBlock();
    // Have to remove them from their original scope first.
    // Otherwise they will show up twice in the unparsed code: original place
    // and under the new block I tried to merge this into appendStatement() but
    // it broke other transformations I don't want debug
    for (std::vector<SgStatement *>::const_iterator iter = stmt_vec.begin();
         iter != stmt_vec.end(); iter++)
      removeStatement(*iter);
    appendStatementList(stmt_vec, isSgScopeStatement(result));
    insertStatementAfter(begin_decl, result, false);
  }
  return result;
}

//! This function will Find a (optional) end pragma for an input pragma (decl)
//  and merge clauses from the end pragma to the beginning pragma
//  statements in between will be put into a basic block if there are more than
//  one statements
void merge_Matching_Fortran_Pragma_pairs(SgPragmaDeclaration *decl) {
  SgPragmaDeclaration *end_decl = NULL;
  SgStatement *next_stmt = getNextStatement(decl);
  OpenMPDirectiveKind begin_directive_kind =
      fortran_paired_pragma_dict[decl]->getKind();

  std::vector<SgStatement *>
      affected_stmts; // statements which are inside the begin .. end pair

  // Find possible end directives attached to a pragma declaration
  while (next_stmt != NULL) {
    end_decl = isSgPragmaDeclaration(next_stmt);
    if ((end_decl) &&
        (((OpenMPEndDirective *)fortran_paired_pragma_dict[end_decl])
             ->getPairedDirective()) == fortran_paired_pragma_dict[decl])
      break;
    else
      end_decl = NULL; // MUST reset to NULL if not a match
    affected_stmts.push_back(next_stmt);
    next_stmt = getNextStatement(next_stmt);
  } // end while

  // mandatory end directives for most begin directives, except for two cases:
  // !$omp end do
  // !$omp end parallel do
  if (end_decl == NULL) {
    if ((begin_directive_kind != OMPD_parallel) &&
        (begin_directive_kind != OMPD_do) &&
        (begin_directive_kind != OMPD_parallel_do) &&
        (begin_directive_kind != OMPD_parallel_loop)) {
      cerr << "merge_Matching_Fortran_Pragma_pairs(): cannot find required end "
              "directive for: "
           << endl;
      cerr << decl->get_pragma()->get_pragma() << endl;
      ROSE_ASSERT(false);
    } else
      return; // There is nothing further to do if the optional end directives
              // do not exist
  }           // end if sanity check

  // at this point, we have found a matching end directive/pragma
  ROSE_ASSERT(end_decl);
  ensureSingleStmtOrBasicBlock(decl, affected_stmts);

  // SgBasicBlock is not unparsed in Fortran
  //
  // To ensure the unparsed Fortran code is correct for debugging
  // -rose:openmp:ast_only
  //  after converting Fortran comments to Pragmas.
  // x.  We should not tweak the original text for the pragmas.
  // x.  We should not remove the end pragma declaration since SgBasicBlock is
  // not unparsed. In the end , the pragmas don't matter too much, the OpenMPIR
  // attached to them are used to guide translations.
  removeStatement(end_decl);
  // we should save those useless end pragmas to a list
  // and remove them as one of the first steps in OpenMP lowering for Fortran
  // omp_end_pragma_list.push_back(end_decl);
} // end merge_Matching_Fortran_Pragma_pairs()

//! This function will
//   x. Find matching OpenMP directive pairs
//      an inside out order is used to handle nested regions
//   x. Put statements in between into a basic block
//   x. Merge clauses from the ending directive to the beginning directives
//  The result is an Fortran OpenMP AST with C/C++ pragmas
//  so we can simply reuse convert_OpenMP_pragma_to_AST() to generate
//  OpenMP AST nodes for Fortran programs
void convert_Fortran_Pragma_Pairs(SgSourceFile *sageFilePtr) {
  ROSE_ASSERT(sageFilePtr != NULL);
  list<SgPragmaDeclaration *>::reverse_iterator
      iter; // bottom up handling for nested cases
  for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend();
       iter++) {
    // It is possible that several source files showing up in a single
    // compilation line We have to check if the pragma declaration's file
    // information matches the current file being processed Otherwise we will
    // process the same pragma declaration multiple times!!
    SgPragmaDeclaration *decl = *iter;
    // Some pragmas are set to "transformation generated" when we fix scopes for
    // some pragma under single statement block e.g if ()
    //      #pragma
    //        do_sth()
    //  will be changed to
    //     if ()
    //     {
    //       #pragma
    //        do_sth()
    //     }
    // So we process a pragma if it is either within the same file or marked as
    // transformation
    if (decl->get_file_info()->get_filename() !=
            sageFilePtr->get_file_info()->get_filename() &&
        !(decl->get_file_info()->isTransformation()))
      continue;
    if (isFortranPairedDirective(fortran_paired_pragma_dict[decl])) {
      merge_Matching_Fortran_Pragma_pairs(decl);
    }
  } // end for omp_pragma_list

} // end convert_Fortran_Pragma_Pairs()

//! Convert OpenMP Fortran comments to pragmas
//  main purpose is to
//     x. Generate pragmas from OpenMPIR and insert them into the right places
//        since the floating comments are very difficult to work with
//        we move them to the fake pragmas to ease later translations.
//        The backend has been extended to unparse the pragma in order to debug
//        this step.
//     x. Enclose affected Fortran statement into a basic block
//     x. Merge clauses from END directives to the begin directive
// This will temporarily introduce C/C++-like AST with pragmas.
// This should be fine since we have SgBasicBlock in Fortran AST also.
//
// The benefit is that pragma-to-AST conversion written for C/C++ can
// be reused for Fortran after this pass.
// Liao 10/18/2010
void convert_Fortran_OMP_Comments_to_Pragmas(SgSourceFile *sageFilePtr) {
  ROSE_ASSERT(sageFilePtr != NULL);
  // step 1: Each OpenMPIR will have a dedicated SgPragmaDeclaration for it

  // we record the last pragma inserted after a statement, if any
  std::map<SgStatement *, SgPragmaDeclaration *> stmt_last_pragma_dict;

  std::vector<std::tuple<SgLocatedNode *, PreprocessingInfo *,
                         OpenMPDirective *>>::iterator iter;
  for (iter = fortran_omp_pragma_list.begin();
       iter != fortran_omp_pragma_list.end(); iter++) {
    SgLocatedNode *loc_node = std::get<0>(*iter);
    SgStatement *stmt = isSgStatement(loc_node);
    OpenMPDirective *ompparser_directive_ir = std::get<2>(*iter);
    // TODO verify this assertion is true for Fortran OpenMP comments
    ROSE_ASSERT(stmt != NULL);
    // cout<<"debug at ompAstConstruction.cpp:"<<stmt <<" " <<
    // stmt->getAttachedPreprocessingInfo ()->size() <<endl;
    ROSE_ASSERT(stmt->getAttachedPreprocessingInfo()->size() != 0);
    // So we process the directive if it's anchor node is either within the same
    // file or marked as transformation
    if (stmt->get_file_info()->get_filename() !=
            sageFilePtr->get_file_info()->get_filename() &&
        !(stmt->get_file_info()->isTransformation()))
      continue;
    SgScopeStatement *scope = stmt->get_scope();
    ROSE_ASSERT(scope != NULL);
    // the pragma will have string to ease debugging
    std::string pragma_string =
        ompparser_directive_ir->generatePragmaString("omp ", "", "");
    SgPragmaDeclaration *p_decl = buildPragmaDeclaration(pragma_string, scope);
    // preserve the original source file info ,TODO complex cases , use real
    // preprocessing info's line information !!
    copyStartFileInfo(loc_node, p_decl);

    if (ompparser_directive_ir->getKind() != OMPD_end) {
      OpenMPIR_list.push_back(std::make_pair(p_decl, ompparser_directive_ir));
      omp_pragma_list.push_back(p_decl);
    }
    fortran_paired_pragma_dict[p_decl] = ompparser_directive_ir;

    PreprocessingInfo *info = std::get<1>(*iter);
    ROSE_ASSERT(info != NULL);
    // We still keep the peprocessingInfo. its line number will be used later to
    // set file info object
    AttachedPreprocessingInfoType *comments =
        stmt->getAttachedPreprocessingInfo();
    ROSE_ASSERT(comments != NULL);
    ROSE_ASSERT(comments->size() != 0);
    AttachedPreprocessingInfoType::iterator m_pos =
        find(comments->begin(), comments->end(), info);
    if (m_pos == comments->end()) {
      cerr << "Cannot find a Fortran comment from a node: " << endl;
      cerr << "The comment is " << info->getString() << endl;
      cerr << "The AST Node is " << stmt->class_name() << endl;
      stmt->get_file_info()->display("debug here");
      AttachedPreprocessingInfoType::iterator i;
      for (i = comments->begin(); i != comments->end(); i++) {
        cerr << (*i)->getString() << endl;
      }
      // cerr<<"The AST Node is at
      // line:"<<stmt->get_file_info().get_line()<<endl;
      ROSE_ASSERT(m_pos != comments->end());
    }
    comments->erase(m_pos);

    // two cases for where to insert the pragma, depending on where the
    // preprocessing info is attached to stmt
    //  1. PreprocessingInfo::before
    //     insert the pragma right before the original Fortran statement
    //  2. PreprocessingInfo::inside
    //      insert it as the last statement within stmt
    PreprocessingInfo::RelativePositionType position =
        info->getRelativePosition();
    if (position == PreprocessingInfo::before) {
      // Don't automatically move comments here!
      if (isSgBasicBlock(stmt) &&
          isSgFortranDo(
              stmt->get_parent())) { // special handling for the body of
                                     // SgFortranDo.  The comments will be
                                     // attached before the body But we cannot
                                     // insert the pragma before the body. So we
                                     // prepend it into the body instead
        prependStatement(p_decl, isSgBasicBlock(stmt));
      } else
        insertStatementBefore(stmt, p_decl, false);
    } else if (position == PreprocessingInfo::inside) {
      SgScopeStatement *scope = isSgScopeStatement(stmt);
      ROSE_ASSERT(scope != NULL);
      appendStatement(p_decl, scope);
    } else if (position == PreprocessingInfo::after) {
      SgStatement *last = stmt;
      if (stmt_last_pragma_dict.count(stmt))
        last = stmt_last_pragma_dict[stmt];
      // Liao, 3/31/2021
      // It is possible there are several comments attached after a same
      // statement. In this case, we should not just insert each generated
      // pragma right after the statement. We should insert each pragma after
      // the previously inserted pragma to preserve the original order.
      // Otherwise , we will end up with reversed order of pragmas, causing
      // later pragma pair matching problem.

      insertStatementAfter(last, p_decl, false);
      stmt_last_pragma_dict[stmt] = p_decl;
    } else {
      cerr << "ompAstConstruction.cpp , illegal "
              "PreprocessingInfo::RelativePositionType:"
           << position << endl;
      ROSE_ASSERT(false);
    }
  } // end for omp_comment_list

  convert_Fortran_Pragma_Pairs(sageFilePtr);
} // end convert_Fortran_OMP_Comments_to_Pragmas ()

// Liao, 5/31/2009 an entry point for OpenMP related processing
// including parsing, AST construction, and later on translation
void processOpenMP(SgSourceFile *sageFilePtr) {
  // DQ (4/4/2010): This function processes both C/C++ and Fortran code.
  // As a result of the Fortran processing some OMP pragmas will cause
  // transformation (e.g. declaration of private variables will add variables
  // to the local scope).  So this function has side-effects for all languages.

  if (SgProject::get_verbose() > 1) {
    printf("Processing OpenMP directives ... \n");
  }

  ROSE_ASSERT(sageFilePtr != NULL);
  if (sageFilePtr->get_openmp() == false) {
    if (SgProject::get_verbose() > 1) {
      printf("Stop processing OpenMP directives since no OpenMP found. \n");
    }
    return;
  }

  bool isFortran = sageFilePtr->get_Fortran_only() ||
                   sageFilePtr->get_F77_only() || sageFilePtr->get_F90_only() ||
                   sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only();

  // ==================================================================================================================//
  // ====== Stage 1: parse OpenMP directives using ompparser and store the
  // ompparser's OpenMPIR nodes in a map   ======
  // ==================================================================================================================//
  // find all SgPragmaDeclaration nodes within a file, parse OpenMP directives
  // using ompparser, and store the ompparser OpenMPIR in a map OpenMPIR_list.
  // ompparser only parse OpenMP directive/clauses not the expressions that are
  // used by the directives/clauses For Fortran, search comments for OpenMP
  // directives
  if (isFortran) { // use ompparser to process Fortran.
    parseOpenMPFortran(sageFilePtr);
  } else { // For C/C++, search pragma declarations for OpenMP directives
    std::vector<SgNode *> all_pragmas =
        NodeQuery::querySubTree(sageFilePtr, V_SgPragmaDeclaration);
    std::vector<SgNode *>::iterator iter;
    for (iter = all_pragmas.begin(); iter != all_pragmas.end(); iter++) {
      SgPragmaDeclaration *pragmaDeclaration = isSgPragmaDeclaration(*iter);
      ROSE_ASSERT(pragmaDeclaration != NULL);
      SageInterface::replaceMacroCallsWithExpandedStrings(pragmaDeclaration);
      string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
      istringstream istr(pragmaString);
      std::string key;
      istr >> key;
      if (key == "omp") {
        omp_pragma_list.push_back(pragmaDeclaration);

        // parse expression
        // Get the object that ompparser IR.
        ompparser_OpenMPIR = parseOpenMP(pragmaString.c_str(), NULL);
        assert(ompparser_OpenMPIR != NULL);
        use_ompparser = checkOpenMPIR(ompparser_OpenMPIR);
        assert(use_ompparser == true);
        OpenMPIR_list.push_back(
            std::make_pair(pragmaDeclaration, ompparser_OpenMPIR));
      } else if (key == "acc") {
        // store them into a buffer, reused by build_OpenMP_AST()
        omp_pragma_list.push_back(pragmaDeclaration);
        // Call parser
        // Get the OpenMP IR converted from the OpenACC IR.
        pragmaString = "#pragma " + pragmaString;
        accparser_OpenACCIR = parseOpenACC(pragmaString);
        assert(accparser_OpenACCIR != NULL);
        use_accparser = checkOpenACCIR(accparser_OpenACCIR);
        assert(use_accparser == true);
        OpenACCIR_list.push_back(
            std::make_pair(pragmaDeclaration, accparser_OpenACCIR));
      }
    } // end for
  }

  // stop here if only OpenMP parsing is requested
  if (sageFilePtr->get_openmp_parse_only()) {
    if (SgProject::get_verbose() > 1) {
      printf("Skipping calls to lower OpenMP "
             "sageFilePtr->get_openmp_parse_only() = %s \n",
             sageFilePtr->get_openmp_parse_only() ? "true" : "false");
    }
    return;
  }

  // Build OpenMP AST nodes based on parsing results
  if (isFortran) {
    convert_Fortran_OMP_Comments_to_Pragmas(
        sageFilePtr); // TODO: need to fix not sure why we still need this here
                      // since Fortran is already parsed before.
  }
  if (SgProject::get_verbose() > 1) {
    printf("Calling convert_OpenMP_pragma_to_AST() \n");
  }
  // We can turn this off to debug the convert_Fortran_OMP_Comments_to_Pragmas()
  OpenMPIRToSageAST(sageFilePtr);

  // stop here if only OpenMP AST construction is requested
  if (sageFilePtr->get_openmp_ast_only()) {
    if (SgProject::get_verbose() > 1) {
      printf("Skipping calls to analyze/lower OpenMP "
             "sageFilePtr->get_openmp_ast_only() = %s \n",
             sageFilePtr->get_openmp_ast_only() ? "true" : "false");
    }
    return;
  }

  // Analyze OpenMP AST
  analyze_omp(sageFilePtr);

  // stop here if only OpenMP AST analyzing is requested
  if (sageFilePtr->get_openmp_analyzing()) {
    if (SgProject::get_verbose() > 1) {
      printf("Skipping calls to lower OpenMP "
             "sageFilePtr->get_openmp_analyzing() = %s \n",
             sageFilePtr->get_openmp_analyzing() ? "true" : "false");
    }
    return;
  }

  lower_omp(sageFilePtr);
}

} // namespace OmpSupport

SgStatement *
convertDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                     current_OpenMPIR_to_SageIII) {
  //    printf("ompparser directive is ready.\n");
  OpenMPDirectiveKind directive_kind =
      current_OpenMPIR_to_SageIII.second->getKind();
  SgStatement *result = NULL;

  switch (directive_kind) {
  case OMPD_metadirective:
  case OMPD_teams:
  case OMPD_atomic:
  case OMPD_do:
  case OMPD_taskgroup:
  case OMPD_master:
  case OMPD_distribute:
  case OMPD_loop:
  case OMPD_scan:
  case OMPD_taskloop:
  case OMPD_target_enter_data:
  case OMPD_target_exit_data:
  case OMPD_target_parallel_for:
  case OMPD_target_parallel:
  case OMPD_distribute_simd:
  case OMPD_distribute_parallel_for:
  case OMPD_distribute_parallel_for_simd:
  case OMPD_taskloop_simd:
  case OMPD_target_parallel_for_simd:
  case OMPD_target_parallel_loop:
  case OMPD_target_simd:
  case OMPD_target_teams:
  case OMPD_target_teams_distribute:
  case OMPD_target_teams_distribute_simd:
  case OMPD_target_teams_loop:
  case OMPD_target_teams_distribute_parallel_for:
  case OMPD_target_teams_distribute_parallel_for_simd:
  case OMPD_master_taskloop_simd:
  case OMPD_parallel_master_taskloop:
  case OMPD_parallel_master_taskloop_simd:
  case OMPD_teams_distribute:
  case OMPD_teams_distribute_simd:
  case OMPD_teams_distribute_parallel_for:
  case OMPD_teams_distribute_parallel_for_simd:
  case OMPD_teams_loop:
  case OMPD_parallel_master:
  case OMPD_master_taskloop:
  case OMPD_parallel_loop:
  case OMPD_task:
  case OMPD_target_data:
  case OMPD_single:
  case OMPD_for:
  case OMPD_for_simd:
  case OMPD_target:
  case OMPD_critical:
  case OMPD_depobj:
  case OMPD_sections:
  case OMPD_section:
  case OMPD_simd:
  case OMPD_parallel:
  case OMPD_workshare:
  case OMPD_tile:
  case OMPD_unroll: {
    result = convertBodyDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_ordered: {
    if (current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder()
            ->size() != 0) {
      std::vector<OpenMPClause *> *ordered_clauses =
          current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
      OpenMPClause *clause = *ordered_clauses->begin();
      if (clause->getKind() == OMPC_depend) {
        result = convertNonBodyDirective(current_OpenMPIR_to_SageIII);
        break;
      } else {
        result = convertBodyDirective(current_OpenMPIR_to_SageIII);
        break;
      }
    } else {
      result = convertBodyDirective(current_OpenMPIR_to_SageIII);
      break;
    }
  }
  case OMPD_parallel_do:
  case OMPD_parallel_for:
  case OMPD_parallel_for_simd:
  case OMPD_parallel_sections:
  case OMPD_parallel_workshare: {
    result = convertCombinedBodyDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_declare_mapper:
  case OMPD_cancellation_point:
  case OMPD_target_update:
  case OMPD_cancel: {
    result = convertNonBodyDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_requires: {
    result = convertOmpRequiresDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_taskwait: {
    result = convertOmpTaskwaitDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_barrier: {
    result = new SgOmpBarrierStatement();
    break;
  }
  case OMPD_declare_simd: {
    result = convertOmpDeclareSimdDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_declare_target: {
    result = convertOmpDeclareTargetDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_end_declare_target: {
    result = convertOmpEndDeclareTargetDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_flush: {
    result = convertOmpFlushDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_allocate: {
    result = convertOmpAllocateDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  case OMPD_taskyield: {
    result = new SgOmpTaskyieldStatement();
    break;
  }
  case OMPD_threadprivate: {
    result = convertOmpThreadprivateStatement(current_OpenMPIR_to_SageIII);
    break;
  }
  default: {
    printf("Unknown directive is found.\n");
  }
  }
  setOneSourcePositionForTransformation(result);
  SgPragmaDeclaration *pdecl = current_OpenMPIR_to_SageIII.first;
  copyStartFileInfo(pdecl, result);
  copyEndFileInfo(pdecl, result);

  //! For C/C++ replace OpenMP pragma declaration with an SgOmpxxStatement
  SgScopeStatement *scope = pdecl->get_scope();
  ROSE_ASSERT(scope != NULL);
  moveUpPreprocessingInfo(result,
                          pdecl); // keep #ifdef etc attached to the pragma
  replaceStatement(pdecl, result);

  return result;
}

SgStatement *
convertVariantDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII) {
  printf("ompparser variant directive is ready.\n");
  OpenMPDirectiveKind directive_kind =
      current_OpenMPIR_to_SageIII.second->getKind();
  SgStatement *result = NULL;

  switch (directive_kind) {
  case OMPD_parallel: {
    result = convertVariantBodyDirective(current_OpenMPIR_to_SageIII);
    break;
  }
  default: {
    printf("Unknown directive is found.\n");
  }
  }
  setOneSourcePositionForTransformation(result);

  return result;
}

SgOmpBodyStatement *
convertCombinedBodyDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                 current_OpenMPIR_to_SageIII) {

  OpenMPDirectiveKind directive_kind =
      current_OpenMPIR_to_SageIII.second->getKind();
  // directives like parallel and for have a following code block beside the
  // pragma itself.
  SgOmpBodyStatement *result = NULL;

  switch (directive_kind) {
  case OMPD_parallel_do:
  case OMPD_parallel_for:
  case OMPD_parallel_for_simd:
  case OMPD_parallel_sections:
  case OMPD_parallel_workshare: {
    result = convertOmpParallelStatementFromCombinedDirectives(
        current_OpenMPIR_to_SageIII);
    break;
  }
  default: {
    printf("Unknown directive is found.\n");
  }
  }
  return result;
}

SgOmpClause *
convertSimpleClause(SgStatement *directive,
                    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                        current_OpenMPIR_to_SageIII,
                    OpenMPClause *current_omp_clause) {
  printf("ompparser simple clause is ready.\n");
  SgOmpClause *sg_clause = NULL;
  OpenMPClauseKind clause_kind = current_omp_clause->getKind();
  switch (clause_kind) {
  case OMPC_nowait: {
    sg_clause = new SgOmpNowaitClause();
    break;
  }
  case OMPC_nogroup: {
    sg_clause = new SgOmpNogroupClause();
    break;
  }
  case OMPC_untied: {
    sg_clause = new SgOmpUntiedClause();
    break;
  }
  case OMPC_mergeable: {
    sg_clause = new SgOmpMergeableClause();
    break;
  }
  case OMPC_read: {
    sg_clause = new SgOmpReadClause();
    break;
  }
  case OMPC_reverse_offload: {
    sg_clause = new SgOmpReverseOffloadClause();
    break;
  }
  case OMPC_unified_address: {
    sg_clause = new SgOmpUnifiedAddressClause();
    break;
  }
  case OMPC_unified_shared_memory: {
    sg_clause = new SgOmpUnifiedSharedMemoryClause();
    break;
  }
  case OMPC_dynamic_allocators: {
    sg_clause = new SgOmpDynamicAllocatorsClause();
    break;
  }
  case OMPC_write: {
    sg_clause = new SgOmpWriteClause();
    break;
  }
  case OMPC_threads: {
    sg_clause = new SgOmpThreadsClause();
    break;
  }
  case OMPC_simd: {
    sg_clause = new SgOmpSimdClause();
    break;
  }
  case OMPC_update: {
    sg_clause = new SgOmpUpdateClause();
    break;
  }
  case OMPC_capture: {
    sg_clause = new SgOmpCaptureClause();
    break;
  }
  case OMPC_seq_cst: {
    sg_clause = new SgOmpSeqCstClause();
    break;
  }
  case OMPC_acq_rel: {
    sg_clause = new SgOmpAcqRelClause();
    break;
  }
  case OMPC_release: {
    sg_clause = new SgOmpReleaseClause();
    break;
  }
  case OMPC_acquire: {
    sg_clause = new SgOmpAcquireClause();
    break;
  }
  case OMPC_relaxed: {
    sg_clause = new SgOmpRelaxedClause();
    break;
  }
  case OMPC_destroy: {
    sg_clause = new SgOmpDestroyClause();
    break;
  }
  case OMPC_inbranch: {
    sg_clause = new SgOmpInbranchClause();
    break;
  }
  case OMPC_notinbranch: {
    sg_clause = new SgOmpNotinbranchClause();
    break;
  }
  case OMPC_parallel: {
    sg_clause = new SgOmpParallelClause();
    break;
  }
  case OMPC_sections: {
    sg_clause = new SgOmpSectionsClause();
    break;
  }
  case OMPC_for: {
    sg_clause = new SgOmpForClause();
    break;
  }
  case OMPC_taskgroup: {
    sg_clause = new SgOmpTaskgroupClause();
    break;
  }
  case OMPC_full: {
    sg_clause = new SgOmpFullClause();
    break;
  }
  default: {
    cerr << "error: unknown clause " << endl;
    ROSE_ASSERT(false);
  }
  };
  setOneSourcePositionForTransformation(sg_clause);
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_declare_simd) {
    ((SgOmpDeclareSimdStatement *)directive)
        ->get_clauses()
        .push_back(sg_clause);
  } else if (current_OpenMPIR_to_SageIII.second->getKind() ==
                 OMPD_target_update ||
             current_OpenMPIR_to_SageIII.second->getKind() == OMPD_cancel ||
             current_OpenMPIR_to_SageIII.second->getKind() ==
                 OMPD_cancellation_point) {
    ((SgOmpTargetUpdateStatement *)directive)
        ->get_clauses()
        .push_back(sg_clause);
  } else if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_requires) {
    ((SgOmpRequiresStatement *)directive)->get_clauses().push_back(sg_clause);
  } else if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_flush) {
    ((SgOmpFlushStatement *)directive)->get_clauses().push_back(sg_clause);
  } else {
    addOmpClause(directive, sg_clause);
  }
  sg_clause->set_parent(directive);
  return sg_clause;
}

SgStatement *
convertNonBodyDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII) {

  OpenMPDirectiveKind directive_kind =
      current_OpenMPIR_to_SageIII.second->getKind();
  SgStatement *result = NULL;
  OpenMPClauseKind clause_kind;

  switch (directive_kind) {
  case OMPD_cancellation_point: {
    result = new SgOmpCancellationPointStatement();
    break;
  }
  case OMPD_declare_mapper: {
    result = new SgOmpDeclareMapperStatement();
    break;
  }
  case OMPD_cancel: {
    result = new SgOmpCancelStatement();
    break;
  }
  case OMPD_target_update: {
    result = new SgOmpTargetUpdateStatement();
    break;
  }
  case OMPD_ordered: {
    result = new SgOmpOrderedDependStatement();
    break;
  }
  default: {
    printf("Unknown directive is found.\n");
  }
  }
  // extract all the clauses based on the vector of clauses in the original
  // order
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_device:
    case OMPC_if: {
      convertExpressionClause(isSgStatement(result),
                              current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_parallel:
    case OMPC_sections:
    case OMPC_for:
    case OMPC_nowait:
    case OMPC_reverse_offload:
    case OMPC_unified_address:
    case OMPC_unified_shared_memory:
    case OMPC_dynamic_allocators:
    case OMPC_taskgroup: {
      convertSimpleClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    case OMPC_depend: {
      convertDependClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    case OMPC_to: {
      convertToClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                      *clause_iter);
      break;
    }
    case OMPC_from: {
      convertFromClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                        *clause_iter);
      break;
    }
    default: {
      cerr << "error: unknown clause " << endl;
      ROSE_ASSERT(false);
    }
    };
  };
  return result;
}

SgStatement *
convertBodyDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                         current_OpenMPIR_to_SageIII) {

  OpenMPDirectiveKind directive_kind =
      current_OpenMPIR_to_SageIII.second->getKind();
  // directives like parallel and for have a following code block beside the
  // pragma itself.
  SgStatement *body = getOpenMPBlockBody(current_OpenMPIR_to_SageIII);
  removeStatement(body, false);
  SgStatement *result = NULL;
  OpenMPClauseKind clause_kind;

  switch (directive_kind) {
  case OMPD_do: {
    result = new SgOmpDoStatement(NULL, body);
    break;
  }
  case OMPD_for: {
    result = new SgOmpForStatement(NULL, body);
    break;
  }
  case OMPD_ordered: {
    result = new SgOmpOrderedStatement(NULL, body);
    break;
  }
  case OMPD_parallel: {
    result = new SgOmpParallelStatement(NULL, body);
    break;
  }
  case OMPD_teams: {
    result = new SgOmpTeamsStatement(NULL, body);
    break;
  }
  case OMPD_atomic: {
    result = new SgOmpAtomicStatement(NULL, body);
    break;
  }
  case OMPD_taskgroup: {
    result = new SgOmpTaskgroupStatement(NULL, body);
    break;
  }
  case OMPD_master: {
    result = new SgOmpMasterStatement(NULL, body);
    break;
  }
  case OMPD_distribute: {
    result = new SgOmpDistributeStatement(NULL, body);
    break;
  }
  case OMPD_loop: {
    result = new SgOmpLoopStatement(NULL, body);
    break;
  }
  case OMPD_scan: {
    result = new SgOmpScanStatement(NULL, body);
    break;
  }
  case OMPD_taskloop: {
    result = new SgOmpTaskloopStatement(NULL, body);
    break;
  }
  case OMPD_target_enter_data: {
    result = new SgOmpTargetEnterDataStatement(NULL, body);
    break;
  }
  case OMPD_target_exit_data: {
    result = new SgOmpTargetExitDataStatement(NULL, body);
    break;
  }
  case OMPD_task: {
    result = new SgOmpTaskStatement(NULL, body);
    break;
  }
  case OMPD_target_data: {
    result = new SgOmpTargetDataStatement(NULL, body);
    break;
  }
  case OMPD_simd: {
    result = new SgOmpSimdStatement(NULL, body);
    break;
  }
  case OMPD_single: {
    result = new SgOmpSingleStatement(NULL, body);
    break;
  }
  case OMPD_for_simd: {
    result = new SgOmpForSimdStatement(NULL, body);
    break;
  }
  case OMPD_target: {
    result = new SgOmpTargetStatement(NULL, body);
    break;
  }
  case OMPD_critical: {
    std::string name =
        ((OpenMPCriticalDirective *)(current_OpenMPIR_to_SageIII.second))
            ->getCriticalName();
    result = new SgOmpCriticalStatement(NULL, body, SgName(name));
    break;
  }
  case OMPD_depobj: {
    std::string name =
        ((OpenMPDepobjDirective *)(current_OpenMPIR_to_SageIII.second))
            ->getDepobj();
    result = new SgOmpDepobjStatement(NULL, body, SgName(name));
    break;
  }
  case OMPD_sections: {
    result = new SgOmpSectionsStatement(NULL, body);
    break;
  }
  case OMPD_section: {
    result = new SgOmpSectionStatement(NULL, body);
    break;
  }
  case OMPD_metadirective: {
    result = new SgOmpMetadirectiveStatement(NULL, body);
    break;
  }
  case OMPD_target_parallel_for: {
    result = new SgOmpTargetParallelForStatement(NULL, body);
    break;
  }
  case OMPD_target_parallel: {
    result = new SgOmpTargetParallelStatement(NULL, body);
    break;
  }
  case OMPD_distribute_simd: {
    result = new SgOmpDistributeSimdStatement(NULL, body);
    break;
  }
  case OMPD_distribute_parallel_for: {
    result = new SgOmpDistributeParallelForStatement(NULL, body);
    break;
  }
  case OMPD_distribute_parallel_for_simd: {
    result = new SgOmpDistributeParallelForSimdStatement(NULL, body);
    break;
  }
  case OMPD_taskloop_simd: {
    result = new SgOmpTaskloopSimdStatement(NULL, body);
    break;
  }
  case OMPD_target_parallel_for_simd: {
    result = new SgOmpTargetParallelForSimdStatement(NULL, body);
    break;
  }
  case OMPD_target_parallel_loop: {
    result = new SgOmpTargetParallelLoopStatement(NULL, body);
    break;
  }
  case OMPD_target_simd: {
    result = new SgOmpTargetSimdStatement(NULL, body);
    break;
  }
  case OMPD_target_teams: {
    result = new SgOmpTargetTeamsStatement(NULL, body);
    break;
  }
  case OMPD_target_teams_distribute: {
    result = new SgOmpTargetTeamsDistributeStatement(NULL, body);
    break;
  }
  case OMPD_target_teams_distribute_simd: {
    result = new SgOmpTargetTeamsDistributeSimdStatement(NULL, body);
    break;
  }
  case OMPD_target_teams_loop: {
    result = new SgOmpTargetTeamsLoopStatement(NULL, body);
    break;
  }
  case OMPD_target_teams_distribute_parallel_for: {
    result = new SgOmpTargetTeamsDistributeParallelForStatement(NULL, body);
    break;
  }
  case OMPD_target_teams_distribute_parallel_for_simd: {
    result = new SgOmpTargetTeamsDistributeParallelForSimdStatement(NULL, body);
    break;
  }
  case OMPD_master_taskloop_simd: {
    result = new SgOmpMasterTaskloopSimdStatement(NULL, body);
    break;
  }
  case OMPD_parallel_master_taskloop: {
    result = new SgOmpParallelMasterTaskloopStatement(NULL, body);
    break;
  }
  case OMPD_parallel_master_taskloop_simd: {
    result = new SgOmpParallelMasterTaskloopSimdStatement(NULL, body);
    break;
  }
  case OMPD_teams_distribute: {
    result = new SgOmpTeamsDistributeStatement(NULL, body);
    break;
  }
  case OMPD_teams_distribute_simd: {
    result = new SgOmpTeamsDistributeSimdStatement(NULL, body);
    break;
  }
  case OMPD_teams_distribute_parallel_for: {
    result = new SgOmpTeamsDistributeParallelForStatement(NULL, body);
    break;
  }
  case OMPD_teams_distribute_parallel_for_simd: {
    result = new SgOmpTeamsDistributeParallelForSimdStatement(NULL, body);
    break;
  }
  case OMPD_teams_loop: {
    result = new SgOmpTeamsLoopStatement(NULL, body);
    break;
  }
  case OMPD_parallel_master: {
    result = new SgOmpParallelMasterStatement(NULL, body);
    break;
  }
  case OMPD_master_taskloop: {
    result = new SgOmpMasterTaskloopStatement(NULL, body);
    break;
  }
  case OMPD_parallel_loop: {
    result = new SgOmpParallelLoopStatement(NULL, body);
    break;
  }
  case OMPD_end: {
    return result;
  }
  case OMPD_workshare: {
    result = new SgOmpWorkshareStatement(NULL, body);
    break;
  }
  case OMPD_unroll: {
    result = new SgOmpUnrollStatement(NULL, body);
    break;
  }
  case OMPD_tile: {
    result = new SgOmpTileStatement(NULL, body);
    break;
  }
  default: {
    printf("Unknown directive is found.\n");
  }
  }
  body->set_parent(result);
  // extract all the clauses based on the vector of clauses in the original
  // order
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_if:
    case OMPC_num_teams:
    case OMPC_final:
    case OMPC_priority:
    case OMPC_hint:
    case OMPC_safelen:
    case OMPC_simdlen:
    case OMPC_ordered:
    case OMPC_collapse:
    case OMPC_thread_limit:
    case OMPC_device:
    case OMPC_grainsize:
    case OMPC_detach:
    case OMPC_num_tasks:
    case OMPC_num_threads:
    case OMPC_partial: {
      convertExpressionClause(result, current_OpenMPIR_to_SageIII,
                              *clause_iter);
      break;
    }
    case OMPC_sizes: {
      convertSizesClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_default: {
      convertDefaultClause(isSgOmpClauseBodyStatement(result),
                           current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_proc_bind: {
      convertProcBindClause(isSgOmpClauseBodyStatement(result),
                            current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_order: {
      convertOrderClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_bind: {
      convertBindClause(isSgOmpClauseBodyStatement(result),
                        current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_when: {
      convertWhenClause(isSgOmpClauseBodyStatement(result),
                        current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_inbranch:
    case OMPC_notinbranch: {
      convertSimpleClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    case OMPC_uses_allocators: {
      convertUsesAllocatorsClause(isSgOmpClauseBodyStatement(result),
                                  current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_read:
    case OMPC_write:
    case OMPC_threads:
    case OMPC_simd:
    case OMPC_update:
    case OMPC_capture:
    case OMPC_seq_cst:
    case OMPC_acq_rel:
    case OMPC_release:
    case OMPC_acquire:
    case OMPC_relaxed:
    case OMPC_mergeable:
    case OMPC_untied:
    case OMPC_nogroup:
    case OMPC_destroy:
    case OMPC_nowait:
    case OMPC_full: {
      convertSimpleClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_schedule: {
      convertScheduleClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_dist_schedule: {
      convertDistScheduleClause(isSgOmpClauseBodyStatement(result),
                                current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_defaultmap: {
      convertDefaultmapClause(isSgOmpClauseBodyStatement(result),
                              current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_map: {
      convertMapClause(isSgOmpClauseBodyStatement(result),
                       current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_depend: {
      convertDependClause(isSgOmpClauseBodyStatement(result),
                          current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_affinity: {
      convertAffinityClause(isSgOmpClauseBodyStatement(result),
                            current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_depobj_update: {
      convertDepobjUpdateClause(isSgOmpClauseBodyStatement(result),
                                current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    default: {
      convertClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
    }
    };
  };

  return result;
}

// Convert an OpenMPIR Declare Simd Directive to a ROSE node
SgStatement *convertOmpDeclareSimdDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII) {
  SgOmpDeclareSimdStatement *result = new SgOmpDeclareSimdStatement();
  result->set_firstNondefiningDeclaration(result);

  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_simdlen: {
      convertExpressionClause(isSgStatement(result),
                              current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_inbranch:
    case OMPC_notinbranch: {
      convertSimpleClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    case OMPC_aligned:
    case OMPC_linear:
    case OMPC_uniform: {
      convertClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                    *clause_iter);
      break;
    }
    default: {
      convertClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                    *clause_iter);
    }
    };
  };
  return result;
}

// Convert an OpenMPIR Declare Target Directive to a ROSE node
SgStatement *convertOmpDeclareTargetDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII) {
  SgOmpDeclareTargetStatement *result = new SgOmpDeclareTargetStatement();
  result->set_firstNondefiningDeclaration(result);

  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_to:
      convertToClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                      *clause_iter);
      break;
    default:
      convertClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                    *clause_iter);
    };
  };
  return result;
}

// Convert an OpenMPIR End Declare Target Directive to a ROSE node
SgStatement *convertOmpEndDeclareTargetDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII) {
  SgOmpEndDeclareTargetStatement *result = new SgOmpEndDeclareTargetStatement();
  result->set_firstNondefiningDeclaration(result);

  return result;
}

SgStatement *
convertOmpRequiresDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII) {
  SgOmpRequiresStatement *result = new SgOmpRequiresStatement();
  result->set_firstNondefiningDeclaration(result);
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_reverse_offload:
    case OMPC_unified_address:
    case OMPC_unified_shared_memory:
    case OMPC_dynamic_allocators: {
      convertSimpleClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    case OMPC_atomic_default_mem_order: {
      convertAtomicDefaultMemOrderClause(
          isSgStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_ext_implementation_defined_requirement: {
      convertExtImplementationDefinedRequirementClause(
          isSgStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    default: {
      convertClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                    *clause_iter);
    }
    };
  };
  return result;
}

SgStatement *
convertOmpTaskwaitDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII) {
  SgOmpTaskwaitStatement *result = new SgOmpTaskwaitStatement();
  result->set_firstNondefiningDeclaration(result);
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_depend: {
      convertDependClause(isSgStatement(result), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    default: {
    }
    };
  };
  return result;
}

// Convert an OpenMPIR Flush Directive to a ROSE node
SgStatement *
convertOmpFlushDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                             current_OpenMPIR_to_SageIII) {
  SgOmpFlushStatement *statement = new SgOmpFlushStatement();
  OpenMPFlushDirective *current_ir =
      static_cast<OpenMPFlushDirective *>(current_OpenMPIR_to_SageIII.second);
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_seq_cst:
    case OMPC_acq_rel:
    case OMPC_release:
    case OMPC_acquire: {
      convertSimpleClause(isSgStatement(statement), current_OpenMPIR_to_SageIII,
                          *clause_iter);
      break;
    }
    default: {
      convertClause(isSgStatement(statement), current_OpenMPIR_to_SageIII,
                    *clause_iter);
    }
    };
  };
  std::vector<std::string> *current_expressions = current_ir->getFlushList();
  if (current_expressions->size() != 0) {
    std::vector<std::string>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      std::string expr_string = std::string() + "varlist " + *iter + "\n";
      omp_exprparser_parser_init(current_OpenMPIR_to_SageIII.first,
                                 expr_string.c_str());
      omp_exprparser_parse();
    }
  }

  std::vector<std::pair<std::string, SgNode *>>::iterator iter;
  for (iter = omp_variable_list.begin(); iter != omp_variable_list.end();
       iter++) {
    if (SgInitializedName *iname = isSgInitializedName((*iter).second)) {
      SgVarRefExp *var_ref = buildVarRefExp(iname);
      statement->get_variables().push_back(var_ref);
      var_ref->set_parent(statement);
    } else if (SgVarRefExp *vref = isSgVarRefExp((*iter).second)) {
      statement->get_variables().push_back(vref);
      vref->set_parent(statement);
    } else {
      cerr << "error: unhandled type of variable within a list:"
           << ((*iter).second)->class_name();
    }
  }
  current_expressions->clear();
  omp_variable_list.clear();
  return statement;
}

// Convert an OpenMPIR Allocate Directive to a ROSE node
SgStatement *
convertOmpAllocateDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII) {
  SgOmpAllocateStatement *statement = new SgOmpAllocateStatement();
  OpenMPAllocateDirective *current_ir = static_cast<OpenMPAllocateDirective *>(
      current_OpenMPIR_to_SageIII.second);
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_allocator: {
      convertAllocatorClause(isSgOmpClauseStatement(statement),
                             current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    default: {
      convertClause(isSgStatement(statement), current_OpenMPIR_to_SageIII,
                    *clause_iter);
    }
    };
  };
  std::vector<const char *> *current_expressions =
      current_ir->getAllocateList();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      std::string expr_string = std::string() + "varlist " + *iter + "\n";
      omp_exprparser_parser_init(current_OpenMPIR_to_SageIII.first,
                                 expr_string.c_str());
      omp_exprparser_parse();
    }
  }

  std::vector<std::pair<std::string, SgNode *>>::iterator iter;
  for (iter = omp_variable_list.begin(); iter != omp_variable_list.end();
       iter++) {
    if (SgInitializedName *iname = isSgInitializedName((*iter).second)) {
      SgVarRefExp *var_ref = buildVarRefExp(iname);
      statement->get_variables().push_back(var_ref);
      var_ref->set_parent(statement);
    } else if (SgVarRefExp *vref = isSgVarRefExp((*iter).second)) {
      statement->get_variables().push_back(vref);
      vref->set_parent(statement);
    } else {
      cerr << "error: unhandled type of variable within a list:"
           << ((*iter).second)->class_name();
    }
  }
  current_expressions->clear();
  omp_variable_list.clear();
  return statement;
}

// Convert an OpenMPIR Threadprivate Directive to a ROSE node
// Because we have to do some non-standard things, I'm putting this in a
// separate function
SgStatement *convertOmpThreadprivateStatement(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII) {
  SgOmpThreadprivateStatement *statement = new SgOmpThreadprivateStatement();
  OpenMPThreadprivateDirective *current_ir =
      static_cast<OpenMPThreadprivateDirective *>(
          current_OpenMPIR_to_SageIII.second);

  std::vector<const char *> *current_expressions =
      current_ir->getThreadprivateList();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      std::string expr_string = std::string() + "varlist " + *iter + "\n";
      omp_exprparser_parser_init(current_OpenMPIR_to_SageIII.first,
                                 expr_string.c_str());
      omp_exprparser_parse();
    }
  }

  std::vector<std::pair<std::string, SgNode *>>::iterator iter;
  for (iter = omp_variable_list.begin(); iter != omp_variable_list.end();
       iter++) {
    if (SgInitializedName *iname = isSgInitializedName((*iter).second)) {
      SgVarRefExp *var_ref = buildVarRefExp(iname);
      statement->get_variables().push_back(var_ref);
      var_ref->set_parent(statement);
    } else if (SgVarRefExp *vref = isSgVarRefExp((*iter).second)) {
      statement->get_variables().push_back(vref);
      vref->set_parent(statement);
    } else {
      cerr << "error: unhandled type of variable within a list:"
           << ((*iter).second)->class_name();
    }
  }

  statement->set_definingDeclaration(statement);
  return statement;
}

SgOmpDepobjUpdateClause *
convertDepobjUpdateClause(SgOmpClauseBodyStatement *clause_body,
                          std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                              current_OpenMPIR_to_SageIII,
                          OpenMPClause *current_omp_clause) {
  printf("ompparser depobj update clause is ready.\n");

  OpenMPDepobjUpdateClauseDependeceType modifier =
      ((OpenMPDepobjUpdateClause *)current_omp_clause)->getType();
  SgOmpClause::omp_depobj_modifier_enum sg_type =
      toSgOmpClauseDepobjModifierType(modifier);
  SgOmpDepobjUpdateClause *result = new SgOmpDepobjUpdateClause(sg_type);
  ROSE_ASSERT(result);

  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);

  printf("ompparser depobj update clause added!\n");
  return result;
}

SgOmpAtomicDefaultMemOrderClause *convertAtomicDefaultMemOrderClause(
    SgStatement *directive,
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII,
    OpenMPClause *current_omp_clause) {
  printf("ompparser atomic_default_mem_order clause is ready.\n");
  OpenMPAtomicDefaultMemOrderClauseKind atomic_default_mem_order_kind =
      ((OpenMPAtomicDefaultMemOrderClause *)current_omp_clause)->getKind();
  SgOmpClause::omp_atomic_default_mem_order_kind_enum sg_dv =
      SgOmpClause::e_omp_atomic_default_mem_order_kind_unspecified;
  switch (atomic_default_mem_order_kind) {
  case OMPC_ATOMIC_DEFAULT_MEM_ORDER_seq_cst: {
    sg_dv = SgOmpClause::e_omp_atomic_default_mem_order_kind_seq_cst;
    break;
  }
  case OMPC_ATOMIC_DEFAULT_MEM_ORDER_acq_rel: {
    sg_dv = SgOmpClause::e_omp_atomic_default_mem_order_kind_acq_rel;
    break;
  }
  case OMPC_ATOMIC_DEFAULT_MEM_ORDER_relaxed: {
    sg_dv = SgOmpClause::e_omp_atomic_default_mem_order_kind_relaxed;
    break;
  }
  default: {
    cerr << "error: buildOmpAtomicDefaultMemOrderClause () Unacceptable "
            "default option from OpenMPIR:"
         << atomic_default_mem_order_kind;
  }
  }; // end switch
  SgOmpAtomicDefaultMemOrderClause *result =
      new SgOmpAtomicDefaultMemOrderClause(sg_dv);
  setOneSourcePositionForTransformation(result);
  ((SgOmpRequiresStatement *)directive)->get_clauses().push_back(result);
  result->set_parent(directive);
  printf("ompparser atomic_default_mem_order clause is added.\n");
  return result;
}

SgOmpExtImplementationDefinedRequirementClause *
convertExtImplementationDefinedRequirementClause(
    SgStatement *directive,
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII,
    OpenMPClause *current_omp_clause) {
  printf("ompparser atomic_default_mem_order clause is ready.\n");
  SgExpression *ext_implementation_defined_requirement = NULL;
  ext_implementation_defined_requirement = parseOmpExpression(
      current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
      ((OpenMPExtImplementationDefinedRequirementClause *)current_omp_clause)
          ->getImplementationDefinedRequirement());
  SgOmpExtImplementationDefinedRequirementClause *result =
      new SgOmpExtImplementationDefinedRequirementClause(
          ext_implementation_defined_requirement);
  setOneSourcePositionForTransformation(result);
  ((SgOmpRequiresStatement *)directive)->get_clauses().push_back(result);
  result->set_parent(directive);
  printf("ompparser atomic_default_mem_order clause is added.\n");
  return result;
}

SgOmpScheduleClause *
convertScheduleClause(SgStatement *directive,
                      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClause *current_omp_clause) {
  printf("ompparser schedule clause is ready.\n");

  OpenMPScheduleClauseModifier modifier1 =
      ((OpenMPScheduleClause *)current_omp_clause)->getModifier1();
  SgOmpClause::omp_schedule_modifier_enum sg_modifier1 =
      toSgOmpClauseScheduleModifier(modifier1);
  OpenMPScheduleClauseModifier modifier2 =
      ((OpenMPScheduleClause *)current_omp_clause)->getModifier2();
  SgOmpClause::omp_schedule_modifier_enum sg_modifier2 =
      toSgOmpClauseScheduleModifier(modifier2);
  OpenMPScheduleClauseKind kind =
      ((OpenMPScheduleClause *)current_omp_clause)->getKind();
  SgOmpClause::omp_schedule_kind_enum sg_kind = toSgOmpClauseScheduleKind(kind);

  SgExpression *chunk_size = NULL;
  if ((((OpenMPScheduleClause *)current_omp_clause)->getChunkSize()) != "") {
    chunk_size = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        ((OpenMPScheduleClause *)current_omp_clause)->getChunkSize());
  }

  SgOmpScheduleClause *result =
      new SgOmpScheduleClause(sg_modifier1, sg_modifier2, sg_kind, chunk_size);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  addOmpClause(directive, result);
  result->set_parent(directive);
  printf("ompparser schedule clause is added.\n");
  return result;
}

SgOmpDistScheduleClause *
convertDistScheduleClause(SgOmpClauseBodyStatement *clause_body,
                          std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                              current_OpenMPIR_to_SageIII,
                          OpenMPClause *current_omp_clause) {
  printf("ompparser dist_schedule clause is ready.\n");

  OpenMPDistScheduleClauseKind kind =
      ((OpenMPDistScheduleClause *)current_omp_clause)->getKind();
  SgOmpClause::omp_dist_schedule_kind_enum sg_kind =
      toSgOmpClauseDistScheduleKind(kind);

  SgExpression *chunk_size = NULL;
  if ((((OpenMPDistScheduleClause *)current_omp_clause)->getChunkSize()) !=
      "") {
    chunk_size = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        ((OpenMPDistScheduleClause *)current_omp_clause)->getChunkSize());
  }

  SgOmpDistScheduleClause *result =
      new SgOmpDistScheduleClause(sg_kind, chunk_size);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);
  printf("ompparser dist_schedule clause is added.\n");
  return result;
}

SgOmpDefaultmapClause *
convertDefaultmapClause(SgOmpClauseBodyStatement *clause_body,
                        std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII,
                        OpenMPClause *current_omp_clause) {
  printf("ompparser defaultmap clause is ready.\n");

  OpenMPDefaultmapClauseBehavior behavior =
      ((OpenMPDefaultmapClause *)current_omp_clause)->getBehavior();
  SgOmpClause::omp_defaultmap_behavior_enum sg_behavior =
      toSgOmpClauseDefaultmapBehavior(behavior);

  OpenMPDefaultmapClauseCategory category =
      ((OpenMPDefaultmapClause *)current_omp_clause)->getCategory();
  SgOmpClause::omp_defaultmap_category_enum sg_category =
      toSgOmpClauseDefaultmapCategory(category);

  SgOmpDefaultmapClause *result =
      new SgOmpDefaultmapClause(sg_behavior, sg_category);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);
  printf("ompparser defaultmap clause is added.\n");
  return result;
}

SgOmpUsesAllocatorsClause *
convertUsesAllocatorsClause(SgOmpClauseBodyStatement *clause_body,
                            std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII,
                            OpenMPClause *current_omp_clause) {

  // budui, allocator yinggai he array duiyingqilai , yinggai you henduo
  // allocators
  printf("ompparser uses_allocators clause is ready.\n");
  SgOmpUsesAllocatorsClause *result = NULL;
  SgOmpUsesAllocatorsDefination *uses_allocators_defination = NULL;
  SgOmpClause::omp_uses_allocators_allocator_enum sg_allocator;
  SgExpression *user_defined_allocator = NULL;
  SgExpression *clause_expression = NULL;
  std::vector<usesAllocatorParameter *> *uses_allocators =
      ((OpenMPUsesAllocatorsClause *)current_omp_clause)
          ->getUsesAllocatorsAllocatorSequence();
  std::vector<usesAllocatorParameter *>::iterator iter;
  std::list<SgOmpUsesAllocatorsDefination *> uses_allocators_definations;
  for (iter = uses_allocators->begin(); iter != uses_allocators->end();
       iter++) {
    OpenMPUsesAllocatorsClauseAllocator allocator =
        ((usesAllocatorParameter *)(*iter))->getUsesAllocatorsAllocator();
    sg_allocator = toSgOmpClauseUsesAllocatorsAllocator(allocator);
    if (sg_allocator ==
        SgOmpClause::e_omp_uses_allocators_allocator_user_defined) {
      clause_expression = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          ((usesAllocatorParameter *)(*iter))->getAllocatorUser());
    }

    SgExpression *allocator_traits_array = NULL;
    std::string allocator_array =
        ((usesAllocatorParameter *)(*iter))->getAllocatorTraitsArray();
    allocator_traits_array =
        parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                           current_omp_clause->getKind(), allocator_array);

    uses_allocators_defination = new SgOmpUsesAllocatorsDefination();
    uses_allocators_defination->set_allocator_traits_array(
        allocator_traits_array);
    uses_allocators_defination->set_allocator(sg_allocator);

    uses_allocators_defination->set_user_defined_allocator(clause_expression);
    uses_allocators_definations.push_back(uses_allocators_defination);
  }

  result = new SgOmpUsesAllocatorsClause();

  ROSE_ASSERT(result != NULL);
  result->set_uses_allocators_defination(uses_allocators_definations);
  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);
  printf("ompparser uses_allocators clause is added.\n");
  return result;
}

SgOmpMapClause *
convertMapClause(SgOmpClauseBodyStatement *clause_body,
                 std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                     current_OpenMPIR_to_SageIII,
                 OpenMPClause *current_omp_clause) {
  printf("ompparser map clause is ready.\n");
  SgOmpMapClause *result = NULL;
  OpenMPMapClauseType type = ((OpenMPMapClause *)current_omp_clause)->getType();
  SgOmpClause::omp_map_operator_enum sg_type = toSgOmpClauseMapOperator(type);

  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      parseOmpArraySection(current_OpenMPIR_to_SageIII.first,
                           current_omp_clause->getKind(), *iter);
    }
  }
  SgExprListExp *explist = buildExprListExp();

  result = new SgOmpMapClause(explist, sg_type);
  ROSE_ASSERT(result != NULL);
  buildVariableList(result);
  explist->set_parent(result);
  result->set_array_dimensions(array_dimensions);

  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);
  array_dimensions.clear();
  omp_variable_list.clear();
  printf("ompparser map clause is added.\n");
  return result;
}

SgStatement *
convertVariantBodyDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII) {

  OpenMPDirectiveKind directive_kind =
      current_OpenMPIR_to_SageIII.second->getKind();
  // directives like parallel and for have a following code block beside the
  // pragma itself.
  SgStatement *result = NULL;
  OpenMPClauseKind clause_kind;

  switch (directive_kind) {
  case OMPD_do: {
    result = new SgOmpDoStatement(NULL, NULL);
    break;
  }
  case OMPD_ordered: {
    result = new SgOmpOrderedStatement(NULL, NULL);
    break;
  }
  case OMPD_parallel: {
    result = new SgOmpParallelStatement(NULL, NULL);
    break;
  }
  case OMPD_simd: {
    result = new SgOmpSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_teams: {
    result = new SgOmpTeamsStatement(NULL, NULL);
    break;
  }
  case OMPD_atomic: {
    result = new SgOmpAtomicStatement(NULL, NULL);
    break;
  }
  case OMPD_taskgroup: {
    result = new SgOmpTaskgroupStatement(NULL, NULL);
    break;
  }
  case OMPD_master: {
    result = new SgOmpMasterStatement(NULL, NULL);
    break;
  }
  case OMPD_distribute: {
    result = new SgOmpDistributeStatement(NULL, NULL);
    break;
  }
  case OMPD_loop: {
    result = new SgOmpLoopStatement(NULL, NULL);
    break;
  }
  case OMPD_scan: {
    result = new SgOmpScanStatement(NULL, NULL);
    break;
  }
  case OMPD_taskloop: {
    result = new SgOmpTaskloopStatement(NULL, NULL);
    break;
  }
  case OMPD_target_enter_data: {
    result = new SgOmpTargetEnterDataStatement(NULL, NULL);
    break;
  }
  case OMPD_target_exit_data: {
    result = new SgOmpTargetExitDataStatement(NULL, NULL);
    break;
  }
  case OMPD_task: {
    result = new SgOmpTaskStatement(NULL, NULL);
    break;
  }
  case OMPD_target_data: {
    result = new SgOmpTargetDataStatement(NULL, NULL);
    break;
  }
  case OMPD_single: {
    result = new SgOmpSingleStatement(NULL, NULL);
    break;
  }
  case OMPD_for: {
    result = new SgOmpForStatement(NULL, NULL);
    break;
  }
  case OMPD_target: {
    result = new SgOmpTargetStatement(NULL, NULL);
    break;
  }
  case OMPD_critical: {
    std::string name =
        ((OpenMPCriticalDirective *)(current_OpenMPIR_to_SageIII.second))
            ->getCriticalName();
    result = new SgOmpCriticalStatement(NULL, NULL, SgName(name));
    break;
  }
  case OMPD_depobj: {
    std::string name =
        ((OpenMPDepobjDirective *)(current_OpenMPIR_to_SageIII.second))
            ->getDepobj();
    result = new SgOmpDepobjStatement(NULL, NULL, SgName(name));
    break;
  }
  case OMPD_metadirective: {
    result = new SgOmpMetadirectiveStatement(NULL, NULL);
    break;
  }
  case OMPD_target_parallel_for: {
    result = new SgOmpTargetParallelForStatement(NULL, NULL);
    break;
  }
  case OMPD_target_parallel: {
    result = new SgOmpTargetParallelStatement(NULL, NULL);
    break;
  }
  case OMPD_distribute_simd: {
    result = new SgOmpDistributeSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_distribute_parallel_for: {
    result = new SgOmpDistributeParallelForStatement(NULL, NULL);
    break;
  }
  case OMPD_distribute_parallel_for_simd: {
    result = new SgOmpDistributeParallelForSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_taskloop_simd: {
    result = new SgOmpTaskloopSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_target_parallel_for_simd: {
    result = new SgOmpTargetParallelForSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_target_parallel_loop: {
    result = new SgOmpTargetParallelLoopStatement(NULL, NULL);
    break;
  }
  case OMPD_target_simd: {
    result = new SgOmpTargetSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_target_teams: {
    result = new SgOmpTargetTeamsStatement(NULL, NULL);
    break;
  }
  case OMPD_target_teams_distribute: {
    result = new SgOmpTargetTeamsDistributeStatement(NULL, NULL);
    break;
  }
  case OMPD_target_teams_distribute_simd: {
    result = new SgOmpTargetTeamsDistributeSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_target_teams_loop: {
    result = new SgOmpTargetTeamsLoopStatement(NULL, NULL);
    break;
  }
  case OMPD_target_teams_distribute_parallel_for: {
    result = new SgOmpTargetTeamsDistributeParallelForStatement(NULL, NULL);
    break;
  }
  case OMPD_target_teams_distribute_parallel_for_simd: {
    result = new SgOmpTargetTeamsDistributeParallelForSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_master_taskloop_simd: {
    result = new SgOmpMasterTaskloopSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_parallel_master_taskloop: {
    result = new SgOmpParallelMasterTaskloopStatement(NULL, NULL);
    break;
  }
  case OMPD_parallel_master_taskloop_simd: {
    result = new SgOmpParallelMasterTaskloopSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_teams_distribute: {
    result = new SgOmpTeamsDistributeStatement(NULL, NULL);
    break;
  }
  case OMPD_teams_distribute_simd: {
    result = new SgOmpTeamsDistributeSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_teams_distribute_parallel_for: {
    result = new SgOmpTeamsDistributeParallelForStatement(NULL, NULL);
    break;
  }
  case OMPD_teams_distribute_parallel_for_simd: {
    result = new SgOmpTeamsDistributeParallelForSimdStatement(NULL, NULL);
    break;
  }
  case OMPD_teams_loop: {
    result = new SgOmpTeamsLoopStatement(NULL, NULL);
    break;
  }
  case OMPD_parallel_master: {
    result = new SgOmpParallelMasterStatement(NULL, NULL);
    break;
  }
  case OMPD_master_taskloop: {
    result = new SgOmpMasterTaskloopStatement(NULL, NULL);
    break;
  }
  case OMPD_parallel_loop: {
    result = new SgOmpParallelLoopStatement(NULL, NULL);
    break;
  }
  case OMPD_end: {
    return result;
  }
  case OMPD_workshare: {
    result = new SgOmpWorkshareStatement(NULL, NULL);
    break;
  }
  default: {
    printf("Unknown directive is found.\n");
  }
  }
  // body->set_parent(result);
  //  extract all the clauses based on the vector of clauses in the original
  //  order
  std::vector<OpenMPClause *> *all_clauses =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  std::vector<OpenMPClause *>::iterator clause_iter;
  for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end();
       clause_iter++) {
    clause_kind = (*clause_iter)->getKind();
    switch (clause_kind) {
    case OMPC_if:
    case OMPC_num_teams:
    case OMPC_grainsize:
    case OMPC_detach:
    case OMPC_num_tasks:
    case OMPC_safelen:
    case OMPC_hint:
    case OMPC_simdlen:
    case OMPC_ordered:
    case OMPC_collapse:
    case OMPC_final:
    case OMPC_priority:
    case OMPC_thread_limit:
    case OMPC_num_threads: {
      convertExpressionClause(result, current_OpenMPIR_to_SageIII,
                              *clause_iter);
      break;
    }
    case OMPC_default: {
      convertDefaultClause(isSgOmpClauseBodyStatement(result),
                           current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_proc_bind: {
      convertProcBindClause(isSgOmpClauseBodyStatement(result),
                            current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_order: {
      convertOrderClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_bind: {
      convertBindClause(isSgOmpClauseBodyStatement(result),
                        current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    case OMPC_when: {
      convertWhenClause(isSgOmpClauseBodyStatement(result),
                        current_OpenMPIR_to_SageIII, *clause_iter);
      break;
    }
    default: {
      convertClause(result, current_OpenMPIR_to_SageIII, *clause_iter);
    }
    };
  };

  return result;
}

SgStatement *
getOpenMPBlockBody(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                       current_OpenMPIR_to_SageIII) {

  SgStatement *result = NULL;
  result = getNextStatement(current_OpenMPIR_to_SageIII.first);
  return result;
}

//! Build SgOmpDefaultClause from OpenMPIR
SgOmpDefaultClause *
convertDefaultClause(SgOmpClauseBodyStatement *clause_body,
                     std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                         current_OpenMPIR_to_SageIII,
                     OpenMPClause *current_omp_clause) {
  OpenMPDefaultClauseKind default_kind =
      ((OpenMPDefaultClause *)current_omp_clause)->getDefaultClauseKind();
  SgOmpClause::omp_default_option_enum sg_dv;
  SgStatement *variant_directive = NULL;
  switch (default_kind) {
  case OMPC_DEFAULT_none: {
    sg_dv = SgOmpClause::e_omp_default_none;
    break;
  }
  case OMPC_DEFAULT_shared: {
    sg_dv = SgOmpClause::e_omp_default_shared;
    break;
  }
  case OMPC_DEFAULT_private: {
    sg_dv = SgOmpClause::e_omp_default_private;
    break;
  }
  case OMPC_DEFAULT_firstprivate: {
    sg_dv = SgOmpClause::e_omp_default_firstprivate;
    break;
  }
  case OMPC_DEFAULT_variant: {
    sg_dv = SgOmpClause::e_omp_default_variant;
    OpenMPDirective *variant_OpenMPIR =
        ((OpenMPDefaultClause *)current_omp_clause)->getVariantDirective();
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        paired_variant_OpenMPIR =
            make_pair(current_OpenMPIR_to_SageIII.first, variant_OpenMPIR);
    variant_directive = convertVariantDirective(paired_variant_OpenMPIR);
    break;
  }
  default: {
    cerr << "error: buildOmpDefaultClase() Unacceptable default option from "
            "OpenMPIR:"
         << default_kind;
    ROSE_ASSERT(false);
  }
  }; // end switch
  SgOmpDefaultClause *result = new SgOmpDefaultClause(sg_dv, variant_directive);
  setOneSourcePositionForTransformation(result);

  if (variant_directive != NULL) {
    variant_directive->set_parent(result);
  };

  // reconsider the location of following code to attach clause
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);

  return result;
}

//! Build SgOmpAllocatorClause from OpenMPIR
SgOmpAllocatorClause *
convertAllocatorClause(SgOmpClauseStatement *clause_body,
                       std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                           current_OpenMPIR_to_SageIII,
                       OpenMPClause *current_omp_clause) {
  OpenMPAllocatorClauseAllocator allocator =
      ((OpenMPAllocatorClause *)current_omp_clause)->getAllocator();
  SgOmpClause::omp_allocator_modifier_enum sg_modifier =
      toSgOmpClauseAllocatorAllocator(allocator);
  SgExpression *user_defined_parameter = NULL;
  SgGlobal *global =
      SageInterface::getGlobalScope(current_OpenMPIR_to_SageIII.first);
  if (sg_modifier == SgOmpClause::e_omp_allocator_user_defined_modifier) {
    SgExpression *clause_expression = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        ((OpenMPAllocatorClause *)current_omp_clause)
            ->getUserDefinedAllocator());
    user_defined_parameter =
        checkOmpExpressionClause(clause_expression, global, e_allocate);
  }
  SgOmpAllocatorClause *result =
      new SgOmpAllocatorClause(sg_modifier, user_defined_parameter);
  setOneSourcePositionForTransformation(result);
  // reconsider the location of following code to attach clause
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);

  return result;
}

//! Build SgOmpProcBindClause from OpenMPIR
SgOmpProcBindClause *
convertProcBindClause(SgOmpClauseBodyStatement *clause_body,
                      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClause *current_omp_clause) {
  OpenMPProcBindClauseKind proc_bind_kind =
      ((OpenMPProcBindClause *)current_omp_clause)->getProcBindClauseKind();
  SgOmpClause::omp_proc_bind_policy_enum sg_dv;
  switch (proc_bind_kind) {
  case OMPC_PROC_BIND_close: {
    sg_dv = SgOmpClause::e_omp_proc_bind_policy_close;
    break;
  }
  case OMPC_PROC_BIND_master: {
    sg_dv = SgOmpClause::e_omp_proc_bind_policy_master;
    break;
  }
  case OMPC_PROC_BIND_spread: {
    sg_dv = SgOmpClause::e_omp_proc_bind_policy_spread;
    break;
  }
  default: {
    cerr << "error: buildOmpProcBindClause () Unacceptable default option from "
            "OpenMPIR:"
         << proc_bind_kind;
    ROSE_ASSERT(false);
  }
  }; // end switch
  SgOmpProcBindClause *result = new SgOmpProcBindClause(sg_dv);
  setOneSourcePositionForTransformation(result);

  // reconsider the location of following code to attach clause
  clause_body->get_clauses().push_back(result);
  result->set_parent(clause_body);

  return result;
}

SgOmpOrderClause *
convertOrderClause(SgStatement *directive,
                   std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                       current_OpenMPIR_to_SageIII,
                   OpenMPClause *current_omp_clause) {
  OpenMPOrderClauseKind order_kind =
      ((OpenMPOrderClause *)current_omp_clause)->getOrderClauseKind();
  SgOmpClause::omp_order_kind_enum sg_dv =
      SgOmpClause::e_omp_order_kind_unspecified;
  switch (order_kind) {
  case OMPC_ORDER_concurrent: {
    sg_dv = SgOmpClause::e_omp_order_kind_concurrent;
    break;
  }
  default: {
    cerr << "error: buildOmpOrderClause () Unacceptable default option from "
            "OpenMPIR:"
         << order_kind;
  }
  }; // end switch
  SgOmpOrderClause *result = new SgOmpOrderClause(sg_dv);
  setOneSourcePositionForTransformation(result);

  // reconsider the location of following code to attach clause
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_declare_simd) {
    ((SgOmpDeclareSimdStatement *)directive)->get_clauses().push_back(result);
  } else {
    addOmpClause(directive, result);
  }
  result->set_parent(directive);

  return result;
}

SgOmpBindClause *
convertBindClause(SgOmpClauseBodyStatement *clause_body,
                  std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                      current_OpenMPIR_to_SageIII,
                  OpenMPClause *current_omp_clause) {
  OpenMPBindClauseBinding bind_binding =
      ((OpenMPBindClause *)current_omp_clause)->getBindClauseBinding();
  SgOmpClause::omp_bind_binding_enum sg_dv =
      SgOmpClause::e_omp_bind_binding_unspecified;
  switch (bind_binding) {
  case OMPC_BIND_teams: {
    sg_dv = SgOmpClause::e_omp_bind_binding_teams;
    break;
  }
  case OMPC_BIND_parallel: {
    sg_dv = SgOmpClause::e_omp_bind_binding_parallel;
    break;
  }
  case OMPC_BIND_thread: {
    sg_dv = SgOmpClause::e_omp_bind_binding_thread;
    break;
  }
  default: {
    cerr << "error: buildOmpBindClause () Unacceptable default option from "
            "OpenMPIR:"
         << bind_binding;
  }
  }; // end switch
  SgOmpBindClause *result = new SgOmpBindClause(sg_dv);
  setOneSourcePositionForTransformation(result);

  // reconsider the location of following code to attach clause
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);

  return result;
}

SgOmpWhenClause *
convertWhenClause(SgOmpClauseBodyStatement *clause_body,
                  std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                      current_OpenMPIR_to_SageIII,
                  OpenMPClause *current_omp_clause) {
  printf("when clause is coming.\n");
  SgStatement *variant_directive = NULL;
  OpenMPDirective *variant_OpenMPIR =
      ((OpenMPWhenClause *)current_omp_clause)->getVariantDirective();
  if (variant_OpenMPIR) {
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        paired_variant_OpenMPIR =
            make_pair(current_OpenMPIR_to_SageIII.first, variant_OpenMPIR);
    variant_directive = convertVariantDirective(paired_variant_OpenMPIR);
  };

  SgExpression *user_condition = NULL;
  std::string user_condition_string =
      ((OpenMPWhenClause *)current_omp_clause)->getUserCondition()->second;
  if (user_condition_string.size()) {
    user_condition = parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                                        current_omp_clause->getKind(),
                                        user_condition_string.c_str());
  };
  SgExpression *user_condition_score = NULL;
  std::string user_condition_score_string =
      ((OpenMPWhenClause *)current_omp_clause)->getUserCondition()->first;
  if (user_condition_score_string.size()) {
    user_condition_score = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        user_condition_score_string.c_str());
  };

  SgExpression *device_arch = NULL;
  std::string device_arch_string =
      ((OpenMPWhenClause *)current_omp_clause)->getArchExpression()->second;
  if (device_arch_string.size()) {
    device_arch = parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                                     current_omp_clause->getKind(),
                                     device_arch_string.c_str());
  };

  SgExpression *device_isa = NULL;
  std::string device_isa_string =
      ((OpenMPWhenClause *)current_omp_clause)->getIsaExpression()->second;
  if (device_isa_string.size()) {
    device_isa = parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                                    current_omp_clause->getKind(),
                                    device_isa_string.c_str());
  };

  SgOmpClause::omp_when_context_kind_enum sg_device_kind =
      SgOmpClause::e_omp_when_context_kind_unknown;
  OpenMPClauseContextKind device_kind =
      ((OpenMPWhenClause *)current_omp_clause)->getContextKind()->second;
  switch (device_kind) {
  case OMPC_CONTEXT_KIND_host: {
    sg_device_kind = SgOmpClause::e_omp_when_context_kind_host;
    break;
  }
  case OMPC_CONTEXT_KIND_nohost: {
    sg_device_kind = SgOmpClause::e_omp_when_context_kind_nohost;
    break;
  }
  case OMPC_CONTEXT_KIND_any: {
    sg_device_kind = SgOmpClause::e_omp_when_context_kind_any;
    break;
  }
  case OMPC_CONTEXT_KIND_cpu: {
    sg_device_kind = SgOmpClause::e_omp_when_context_kind_cpu;
    break;
  }
  case OMPC_CONTEXT_KIND_gpu: {
    sg_device_kind = SgOmpClause::e_omp_when_context_kind_gpu;
    break;
  }
  case OMPC_CONTEXT_KIND_fpga: {
    sg_device_kind = SgOmpClause::e_omp_when_context_kind_fpga;
    break;
  }
  default: {
    ;
  }
  };
  SgOmpClause::omp_when_context_vendor_enum sg_implementation_vendor =
      SgOmpClause::e_omp_when_context_vendor_unspecified;
  OpenMPClauseContextVendor implementation_vendor =
      ((OpenMPWhenClause *)current_omp_clause)->getImplementationKind()->second;
  switch (implementation_vendor) {
  case OMPC_CONTEXT_VENDOR_amd: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_amd;
    break;
  }
  case OMPC_CONTEXT_VENDOR_arm: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_arm;
    break;
  }
  case OMPC_CONTEXT_VENDOR_bsc: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_bsc;
    break;
  }
  case OMPC_CONTEXT_VENDOR_cray: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_cray;
    break;
  }
  case OMPC_CONTEXT_VENDOR_fujitsu: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_fujitsu;
    break;
  }
  case OMPC_CONTEXT_VENDOR_gnu: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_gnu;
    break;
  }
  case OMPC_CONTEXT_VENDOR_ibm: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_ibm;
    break;
  }
  case OMPC_CONTEXT_VENDOR_intel: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_intel;
    break;
  }
  case OMPC_CONTEXT_VENDOR_llvm: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_llvm;
    break;
  }
  case OMPC_CONTEXT_VENDOR_pgi: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_pgi;
    break;
  }
  case OMPC_CONTEXT_VENDOR_ti: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_ti;
    break;
  }
  case OMPC_CONTEXT_VENDOR_unknown: {
    sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_unknown;
    break;
  }
  default: {
    ;
  }
  };

  SgExpression *implementation_user_defined = NULL;
  std::string implementation_user_defined_string =
      ((OpenMPWhenClause *)current_omp_clause)
          ->getImplementationExpression()
          ->second;
  if (implementation_user_defined_string.size()) {
    implementation_user_defined = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        implementation_user_defined_string.c_str());
  };

  SgExpression *implementation_extension = NULL;
  std::string implementation_extension_string =
      ((OpenMPWhenClause *)current_omp_clause)
          ->getExtensionExpression()
          ->second;
  if (implementation_extension_string.size()) {
    implementation_extension = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        implementation_extension_string.c_str());
  };

  SgOmpWhenClause *result = new SgOmpWhenClause(
      user_condition, user_condition_score, device_arch, device_isa,
      sg_device_kind, sg_implementation_vendor, implementation_user_defined,
      implementation_extension, variant_directive);
  std::vector<std::pair<std::string, OpenMPDirective *>> *construct_directive =
      ((OpenMPWhenClause *)current_omp_clause)->getConstructDirective();
  if (construct_directive->size()) {
    std::list<SgStatement *> sg_construct_directives;
    SgStatement *sg_construct_directive = NULL;
    for (unsigned int i = 0; i < construct_directive->size(); i++) {
      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
          paired_construct_OpenMPIR =
              make_pair(current_OpenMPIR_to_SageIII.first,
                        construct_directive->at(i).second);
      sg_construct_directive =
          convertVariantDirective(paired_construct_OpenMPIR);
      sg_construct_directives.push_back(sg_construct_directive);
    };
    result->set_construct_directives(sg_construct_directives);
  };

  setOneSourcePositionForTransformation(result);
  if (variant_directive != NULL) {
    variant_directive->set_parent(result);
  };

  // reconsider the location of following code to attach clause
  SgOmpClause *sg_clause = result;
  clause_body->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);

  return result;
}

SgOmpSizesClause *
convertSizesClause(SgStatement *directive,
                   std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                       current_OpenMPIR_to_SageIII,
                   OpenMPClause *current_omp_clause) {
  omp_variable_list.clear();
  OpenMPClauseKind clause_kind = current_omp_clause->getKind();
  SgGlobal *global =
      SageInterface::getGlobalScope(current_OpenMPIR_to_SageIII.first);
  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  SgExprListExp *explist = buildExprListExp();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      SgExpression *exp =
          parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                             current_omp_clause->getKind(), *iter);
      explist->append_expression(exp);
    }
  }

  // SgExprListExp* explist = buildExprListExp();
  SgOmpSizesClause *result = new SgOmpSizesClause(explist);
  printf("Sizes Clause added!\n");

  setOneSourcePositionForTransformation(result);
  // buildVariableList(result);
  explist->set_parent(result);
  // reconsider the location of following code to attach clause
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_declare_simd) {
    ((SgOmpDeclareSimdStatement *)directive)->get_clauses().push_back(result);
  } else {
    addOmpClause(directive, result);
  }
  result->set_parent(directive);
  omp_variable_list.clear();
  return result;
}

SgOmpVariablesClause *
convertClause(SgStatement *directive,
              std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                  current_OpenMPIR_to_SageIII,
              OpenMPClause *current_omp_clause) {
  omp_variable_list.clear();
  SgOmpVariablesClause *result = NULL;
  OpenMPClauseKind clause_kind = current_omp_clause->getKind();
  SgGlobal *global =
      SageInterface::getGlobalScope(current_OpenMPIR_to_SageIII.first);
  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      parseOmpVariable(current_OpenMPIR_to_SageIII,
                       current_omp_clause->getKind(), *iter);
    }
  }

  SgExprListExp *explist = buildExprListExp();
  switch (clause_kind) {
  case OMPC_allocate: {
    OpenMPAllocateClauseAllocator allocate_allocator =
        ((OpenMPAllocateClause *)current_omp_clause)->getAllocator();
    SgOmpClause::omp_allocate_modifier_enum sg_modifier =
        toSgOmpClauseAllocateAllocator(allocate_allocator);
    SgExpression *user_defined_parameter = NULL;
    if (sg_modifier == SgOmpClause::e_omp_allocate_user_defined_modifier) {
      SgExpression *clause_expression = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          ((OpenMPAllocateClause *)current_omp_clause)
              ->getUserDefinedAllocator());
      user_defined_parameter =
          checkOmpExpressionClause(clause_expression, global, e_allocate);
    }
    result =
        new SgOmpAllocateClause(explist, sg_modifier, user_defined_parameter);
    printf("Allocate Clause added!\n");
    break;
  }
  case OMPC_copyin: {
    result = new SgOmpCopyinClause(explist);
    printf("Copyin Clause added!\n");
    break;
  }
  case OMPC_firstprivate: {
    result = new SgOmpFirstprivateClause(explist);
    printf("Firstprivate Clause added!\n");
    break;
  }
  case OMPC_nontemporal: {
    result = new SgOmpNontemporalClause(explist);
    printf("Nontemporal Clause added!\n");
    break;
  }
  case OMPC_inclusive: {
    result = new SgOmpInclusiveClause(explist);
    printf("Inclusive Clause added!\n");
    break;
  }
  case OMPC_exclusive: {
    result = new SgOmpExclusiveClause(explist);
    printf("Exclusive Clause added!\n");
    break;
  }
  case OMPC_is_device_ptr: {
    result = new SgOmpIsDevicePtrClause(explist);
    printf("is_device_ptr Clause added!\n");
    break;
  }
  case OMPC_use_device_ptr: {
    result = new SgOmpUseDevicePtrClause(explist);
    printf("use_device_ptr Clause added!\n");
    break;
  }
  case OMPC_use_device_addr: {
    result = new SgOmpUseDeviceAddrClause(explist);
    printf("use_device_addr Clause added!\n");
    break;
  }
  case OMPC_private: {
    result = new SgOmpPrivateClause(explist);
    printf("Private Clause added!\n");
    break;
  }
  case OMPC_copyprivate: {
    result = new SgOmpCopyprivateClause(explist);
    printf("Copyprivate Clause added!\n");
    break;
  }
  case OMPC_reduction: {
    OpenMPReductionClauseModifier modifier =
        ((OpenMPReductionClause *)current_omp_clause)->getModifier();
    SgOmpClause::omp_reduction_modifier_enum sg_modifier =
        toSgOmpClauseReductionModifier(modifier);
    OpenMPReductionClauseIdentifier identifier =
        ((OpenMPReductionClause *)current_omp_clause)->getIdentifier();
    SgOmpClause::omp_reduction_identifier_enum sg_identifier =
        toSgOmpClauseReductionIdentifier(identifier);
    SgExpression *user_defined_identifier = NULL;
    if (sg_identifier == SgOmpClause::e_omp_reduction_user_defined_identifier) {
      SgExpression *clause_expression = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          ((OpenMPReductionClause *)current_omp_clause)
              ->getUserDefinedIdentifier());
      user_defined_identifier =
          checkOmpExpressionClause(clause_expression, global, e_reduction);
    }
    result = new SgOmpReductionClause(explist, sg_modifier, sg_identifier,
                                      user_defined_identifier);
    printf("Reduction Clause added!\n");
    break;
  }
  case OMPC_in_reduction: {
    OpenMPInReductionClauseIdentifier identifier =
        ((OpenMPInReductionClause *)current_omp_clause)->getIdentifier();
    SgOmpClause::omp_in_reduction_identifier_enum sg_identifier =
        toSgOmpClauseInReductionIdentifier(identifier);
    SgExpression *user_defined_identifier = NULL;
    if (sg_identifier ==
        SgOmpClause::e_omp_in_reduction_user_defined_identifier) {
      SgExpression *clause_expression = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          ((OpenMPInReductionClause *)current_omp_clause)
              ->getUserDefinedIdentifier());
      user_defined_identifier =
          checkOmpExpressionClause(clause_expression, global, e_reduction);
    }
    result = new SgOmpInReductionClause(explist, sg_identifier,
                                        user_defined_identifier);
    printf("In_reduction Clause added!\n");
    break;
  }
  case OMPC_task_reduction: {
    OpenMPTaskReductionClauseIdentifier identifier =
        ((OpenMPTaskReductionClause *)current_omp_clause)->getIdentifier();
    SgOmpClause::omp_task_reduction_identifier_enum sg_identifier =
        toSgOmpClauseTaskReductionIdentifier(identifier);
    SgExpression *user_defined_identifier = NULL;
    if (sg_identifier ==
        SgOmpClause::e_omp_task_reduction_user_defined_identifier) {
      SgExpression *clause_expression = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          ((OpenMPTaskReductionClause *)current_omp_clause)
              ->getUserDefinedIdentifier());
      user_defined_identifier =
          checkOmpExpressionClause(clause_expression, global, e_reduction);
    }
    result = new SgOmpTaskReductionClause(explist, sg_identifier,
                                          user_defined_identifier);
    printf("Task_reduction Clause added!\n");
    break;
  }
  case OMPC_linear: {
    OpenMPLinearClauseModifier modifier =
        ((OpenMPLinearClause *)current_omp_clause)->getModifier();
    SgOmpClause::omp_linear_modifier_enum sg_modifier =
        toSgOmpClauseLinearModifier(modifier);
    SgExpression *stepExp = NULL;
    if ((((OpenMPLinearClause *)current_omp_clause)->getUserDefinedStep()) !=
        "") {
      stepExp = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          ((OpenMPLinearClause *)current_omp_clause)->getUserDefinedStep());
    }
    result = new SgOmpLinearClause(explist, stepExp, sg_modifier);
    printf("Linear Clause added!\n");
    break;
  }
  case OMPC_aligned: {
    SgExpression *alignExp = NULL;
    if ((((OpenMPAlignedClause *)current_omp_clause)
             ->getUserDefinedAlignment()) != "") {
      alignExp = parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                                    current_omp_clause->getKind(),
                                    ((OpenMPAlignedClause *)current_omp_clause)
                                        ->getUserDefinedAlignment());
    }
    result = new SgOmpAlignedClause(explist, alignExp);
    printf("Aligned Clause added!\n");
    break;
  }
  case OMPC_lastprivate: {
    OpenMPLastprivateClauseModifier modifier =
        ((OpenMPLastprivateClause *)current_omp_clause)->getModifier();
    SgOmpClause::omp_lastprivate_modifier_enum sg_modifier =
        toSgOmpClauseLastprivateModifier(modifier);
    result = new SgOmpLastprivateClause(explist, sg_modifier);
    printf("Lastprivate Clause added!\n");
    break;
  }
  case OMPC_shared: {
    result = new SgOmpSharedClause(explist);
    printf("Shared Clause added!\n");
    break;
  }
  case OMPC_uniform: {
    result = new SgOmpUniformClause(explist);
    printf("Uniform Clause added!\n");
    break;
  }
  default: {
    printf("Unknown Clause!\n");
  }
  }
  setOneSourcePositionForTransformation(result);
  buildVariableList(result);
  explist->set_parent(result);
  // reconsider the location of following code to attach clause
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_declare_simd) {
    ((SgOmpDeclareSimdStatement *)directive)->get_clauses().push_back(result);
  } else {
    addOmpClause(directive, result);
  }
  result->set_parent(directive);
  omp_variable_list.clear();
  return result;
}

SgOmpToClause *
convertToClause(SgStatement *clause_body,
                std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                    current_OpenMPIR_to_SageIII,
                OpenMPClause *current_omp_clause) {
  printf("ompparser to clause is ready.\n");
  SgOmpToClause *result = NULL;
  OpenMPToClauseKind kind = ((OpenMPToClause *)current_omp_clause)->getKind();
  SgOmpClause::omp_to_kind_enum sg_type = toSgOmpClauseToKind(kind);
  SgExpression *mapper_identifier = NULL;

  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      parseOmpArraySection(current_OpenMPIR_to_SageIII.first,
                           current_omp_clause->getKind(), *iter);
    }
  }
  SgExprListExp *explist = buildExprListExp();

  result = new SgOmpToClause(explist, sg_type);
  if ((((OpenMPToClause *)current_omp_clause)->getMapperIdentifier()) != "") {
    mapper_identifier = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        ((OpenMPToClause *)current_omp_clause)->getMapperIdentifier());
  }
  result->set_mapper_identifier(mapper_identifier);
  ROSE_ASSERT(result != NULL);
  buildVariableList(result);
  explist->set_parent(result);
  result->set_array_dimensions(array_dimensions);

  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_target_update) {
    ((SgOmpTargetUpdateStatement *)clause_body)
        ->get_clauses()
        .push_back(sg_clause);
  }
  sg_clause->set_parent(clause_body);
  array_dimensions.clear();
  omp_variable_list.clear();
  printf("ompparser to clause is added.\n");
  return result;
}

SgOmpFromClause *
convertFromClause(SgStatement *clause_body,
                  std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                      current_OpenMPIR_to_SageIII,
                  OpenMPClause *current_omp_clause) {
  printf("ompparser from clause is ready.\n");
  SgOmpFromClause *result = NULL;
  OpenMPFromClauseKind kind =
      ((OpenMPFromClause *)current_omp_clause)->getKind();
  SgOmpClause::omp_from_kind_enum sg_type = toSgOmpClauseFromKind(kind);
  SgExpression *mapper_identifier = NULL;

  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      parseOmpArraySection(current_OpenMPIR_to_SageIII.first,
                           current_omp_clause->getKind(), *iter);
    }
  }
  SgExprListExp *explist = buildExprListExp();
  result = new SgOmpFromClause(explist, sg_type);
  if ((((OpenMPToClause *)current_omp_clause)->getMapperIdentifier()) != "") {
    mapper_identifier = parseOmpExpression(
        current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
        ((OpenMPToClause *)current_omp_clause)->getMapperIdentifier());
  }
  result->set_mapper_identifier(mapper_identifier);
  ROSE_ASSERT(result != NULL);
  buildVariableList(result);
  explist->set_parent(result);
  result->set_array_dimensions(array_dimensions);

  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_target_update) {
    ((SgOmpTargetUpdateStatement *)clause_body)
        ->get_clauses()
        .push_back(sg_clause);
  }
  sg_clause->set_parent(clause_body);
  array_dimensions.clear();
  omp_variable_list.clear();
  printf("ompparser from clause is added.\n");
  return result;
}

SgOmpDependClause *
convertDependClause(SgStatement *clause_body,
                    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                        current_OpenMPIR_to_SageIII,
                    OpenMPClause *current_omp_clause) {
  printf("ompparser depend clause is ready.\n");
  SgOmpDependClause *result = NULL;

  SgExpression *iterator_type = NULL;
  SgExpression *identifier = NULL;
  SgExpression *begin = NULL;
  SgExpression *end = NULL;
  SgExpression *step = NULL;

  OpenMPDependClauseModifier modifier =
      ((OpenMPDependClause *)current_omp_clause)->getModifier();
  std::vector<vector<const char *> *> *omp_depend_iterators_definition_class =
      NULL;
  std::list<std::list<SgExpression *>> depend_iterators_definition_class;
  if (modifier == OMPC_DEPEND_MODIFIER_iterator) {
    omp_depend_iterators_definition_class =
        ((OpenMPDependClause *)current_omp_clause)
            ->getDependIteratorsDefinitionClass();
    for (unsigned int i = 0; i < omp_depend_iterators_definition_class->size();
         i++) {
      std::list<SgExpression *> iterator_expressions;
      if ((string)(omp_depend_iterators_definition_class->at(i)->at(0)) != "") {
        iterator_type = parseOmpExpression(
            current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
            std::string(omp_depend_iterators_definition_class->at(i)->at(0)));
        iterator_expressions.push_back(iterator_type);
      } else {
        iterator_type = NULL;
        iterator_expressions.push_back(iterator_type);
      }
      identifier = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          std::string(omp_depend_iterators_definition_class->at(i)->at(1)));
      iterator_expressions.push_back(identifier);
      begin = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          std::string(omp_depend_iterators_definition_class->at(i)->at(2)));
      iterator_expressions.push_back(begin);
      end = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          std::string(omp_depend_iterators_definition_class->at(i)->at(3)));
      iterator_expressions.push_back(end);

      if ((string)(omp_depend_iterators_definition_class->at(i)->at(4)) != "") {
        step = parseOmpExpression(
            current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
            std::string(omp_depend_iterators_definition_class->at(i)->at(4)));
        iterator_expressions.push_back(step);
      } else {
        step = NULL;
        iterator_expressions.push_back(step);
      }
      depend_iterators_definition_class.push_back(iterator_expressions);
    }
  }
  SgOmpClause::omp_depend_modifier_enum sg_modifier =
      toSgOmpClauseDependModifier(modifier);
  OpenMPDependClauseType type =
      ((OpenMPDependClause *)current_omp_clause)->getType();
  SgOmpClause::omp_dependence_type_enum sg_type =
      toSgOmpClauseDependenceType(type);
  SgExprListExp *explist = NULL;
  SgExpression *vec = NULL;
  std::list<SgExpression *> vec_list;
  if (type != OMPC_DEPENDENCE_TYPE_sink) {
    std::vector<const char *> *current_expressions =
        current_omp_clause->getExpressions();
    if (current_expressions->size() != 0) {
      std::vector<const char *>::iterator iter;
      for (iter = current_expressions->begin();
           iter != current_expressions->end(); iter++) {
        parseOmpArraySection(current_OpenMPIR_to_SageIII.first,
                             current_omp_clause->getKind(), *iter);
      }
    }
    explist = buildExprListExp();
  } else if (type == OMPC_DEPENDENCE_TYPE_sink) {
    explist = buildExprListExp();
    std::vector<const char *> *current_expressions =
        current_omp_clause->getExpressions();
    if (current_expressions->size() != 0) {
      std::vector<const char *>::iterator iter;
      for (iter = current_expressions->begin();
           iter != current_expressions->end(); iter++) {
        vec = parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                                 current_omp_clause->getKind(), *iter);
        vec_list.push_back(vec);
      }
    }
  }
  result = new SgOmpDependClause(explist, sg_modifier, sg_type);
  ROSE_ASSERT(result != NULL);
  buildVariableList(result);
  if (type != OMPC_DEPENDENCE_TYPE_sink)
    explist->set_parent(result);
  result->set_vec(vec_list);
  result->set_array_dimensions(array_dimensions);
  result->set_iterator(depend_iterators_definition_class);
  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_target_update) {
    ((SgOmpTargetUpdateStatement *)clause_body)
        ->get_clauses()
        .push_back(sg_clause);
  } else if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_taskwait) {
    ((SgOmpTaskwaitStatement *)clause_body)->get_clauses().push_back(sg_clause);
  } else if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_ordered) {
    ((SgOmpOrderedDependStatement *)clause_body)
        ->get_clauses()
        .push_back(sg_clause);
  } else {
    ((SgOmpClauseBodyStatement *)clause_body)
        ->get_clauses()
        .push_back(sg_clause);
  }
  sg_clause->set_parent(clause_body);
  array_dimensions.clear();
  omp_variable_list.clear();
  printf("ompparser depend clause is added.\n");
  return result;
}

SgOmpAffinityClause *
convertAffinityClause(SgStatement *clause_body,
                      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClause *current_omp_clause) {
  printf("ompparser affinity clause is ready.\n");
  SgOmpAffinityClause *result = NULL;

  SgExpression *iterator_type = NULL;
  SgExpression *identifier = NULL;
  SgExpression *begin = NULL;
  SgExpression *end = NULL;
  SgExpression *step = NULL;

  OpenMPAffinityClauseModifier modifier =
      ((OpenMPAffinityClause *)current_omp_clause)->getModifier();
  std::vector<vector<const char *> *> *omp_affinity_iterators_definition_class =
      NULL;
  std::list<std::list<SgExpression *>> affinity_iterators_definition_class;
  if (modifier == OMPC_AFFINITY_MODIFIER_iterator) {
    omp_affinity_iterators_definition_class =
        ((OpenMPAffinityClause *)current_omp_clause)
            ->getIteratorsDefinitionClass();
    for (unsigned int i = 0;
         i < omp_affinity_iterators_definition_class->size(); i++) {
      std::list<SgExpression *> iterator_expressions;
      if ((string)(omp_affinity_iterators_definition_class->at(i)->at(0)) !=
          "") {
        iterator_type = parseOmpExpression(
            current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
            std::string(omp_affinity_iterators_definition_class->at(i)->at(0)));
        iterator_expressions.push_back(iterator_type);
      } else {
        iterator_type = NULL;
        iterator_expressions.push_back(iterator_type);
      }
      identifier = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          std::string(omp_affinity_iterators_definition_class->at(i)->at(1)));
      iterator_expressions.push_back(identifier);
      begin = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          std::string(omp_affinity_iterators_definition_class->at(i)->at(2)));
      iterator_expressions.push_back(begin);
      end = parseOmpExpression(
          current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
          std::string(omp_affinity_iterators_definition_class->at(i)->at(3)));
      iterator_expressions.push_back(end);

      if ((string)(omp_affinity_iterators_definition_class->at(i)->at(4)) !=
          "") {
        step = parseOmpExpression(
            current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),
            std::string(omp_affinity_iterators_definition_class->at(i)->at(4)));
        iterator_expressions.push_back(step);
      } else {
        step = NULL;
        iterator_expressions.push_back(step);
      }
      affinity_iterators_definition_class.push_back(iterator_expressions);
    }
  }
  SgOmpClause::omp_affinity_modifier_enum sg_modifier =
      toSgOmpClauseAffinityModifier(modifier);

  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      parseOmpArraySection(current_OpenMPIR_to_SageIII.first,
                           current_omp_clause->getKind(), *iter);
    }
  }
  SgExprListExp *explist = buildExprListExp();

  result = new SgOmpAffinityClause(explist, sg_modifier);
  ROSE_ASSERT(result != NULL);
  buildVariableList(result);
  explist->set_parent(result);
  result->set_array_dimensions(array_dimensions);
  result->set_iterator(affinity_iterators_definition_class);

  setOneSourcePositionForTransformation(result);
  SgOmpClause *sg_clause = result;
  ((SgOmpClauseBodyStatement *)clause_body)->get_clauses().push_back(sg_clause);
  sg_clause->set_parent(clause_body);
  array_dimensions.clear();
  omp_variable_list.clear();
  printf("ompparser affinity clause is added.\n");
  return result;
}

SgOmpExpressionClause *
convertExpressionClause(SgStatement *directive,
                        std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII,
                        OpenMPClause *current_omp_clause) {
  printf("ompparser expression clause is ready.\n");
  SgOmpExpressionClause *result = NULL;
  SgExpression *clause_expression = NULL;
  SgGlobal *global =
      SageInterface::getGlobalScope(current_OpenMPIR_to_SageIII.first);
  OpenMPClauseKind clause_kind = current_omp_clause->getKind();
  std::vector<const char *> *current_expressions =
      current_omp_clause->getExpressions();
  if (current_expressions->size() != 0) {
    std::vector<const char *>::iterator iter;
    for (iter = current_expressions->begin();
         iter != current_expressions->end(); iter++) {
      clause_expression =
          parseOmpExpression(current_OpenMPIR_to_SageIII.first,
                             current_omp_clause->getKind(), *iter);
    }
  }

  switch (clause_kind) {
  case OMPC_if: {
    OpenMPIfClauseModifier if_modifier =
        ((OpenMPIfClause *)current_omp_clause)->getModifier();
    SgOmpClause::omp_if_modifier_enum sg_modifier =
        toSgOmpClauseIfModifier(if_modifier);
    clause_expression->set_parent(current_OpenMPIR_to_SageIII.first);
    SgExpression *if_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpIfClause(if_expression, sg_modifier);
    printf("If Clause added!\n");
    break;
  }
  case OMPC_num_threads: {
    SgExpression *num_threads_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpNumThreadsClause(num_threads_expression);
    printf("Num_threads Clause added!\n");
    break;
  }
  case OMPC_num_teams: {
    SgExpression *num_teams_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpNumTeamsClause(num_teams_expression);
    printf("Num_teams Clause added!\n");
    break;
  }
  case OMPC_grainsize: {
    SgExpression *grainsize_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpGrainsizeClause(grainsize_expression);
    printf("Grainsize Clause added!\n");
    break;
  }
  case OMPC_detach: {
    SgExpression *detach_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpDetachClause(detach_expression);
    printf("Detach Clause added!\n");
    break;
  }
  case OMPC_num_tasks: {
    SgExpression *num_tasks_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpNumTasksClause(num_tasks_expression);
    printf("Num_tasks Clause added!\n");
    break;
  }
  case OMPC_final: {
    SgExpression *final_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpFinalClause(final_expression);
    printf("Final Clause added!\n");
    break;
  }
  case OMPC_priority: {
    SgExpression *priority_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpPriorityClause(priority_expression);
    printf("Priority Clause added!\n");
    break;
  }
  case OMPC_hint: {
    SgExpression *hint_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpHintClause(hint_expression);
    printf("hint Clause added!\n");
    break;
  }
  case OMPC_safelen: {
    SgExpression *safelen_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpSafelenClause(safelen_expression);
    printf("Safelen Clause added!\n");
    break;
  }
  case OMPC_simdlen: {
    SgExpression *simdlen_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpSimdlenClause(simdlen_expression);
    printf("Simdlen Clause added!\n");
    break;
  }
  case OMPC_ordered: {
    SgExpression *ordered_expression =
        checkOmpExpressionClause(clause_expression, global, e_ordered_clause);
    result = new SgOmpOrderedClause(ordered_expression);
    printf("Ordered Clause added!\n");
    break;
  }
  case OMPC_collapse: {
    SgExpression *collapse_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpCollapseClause(collapse_expression);
    printf("Collapse Clause added!\n");
    break;
  }
  case OMPC_thread_limit: {
    SgExpression *thread_limit_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpThreadLimitClause(thread_limit_expression);
    printf("Thread_limit Clause added!\n");
    break;
  }
  case OMPC_device: {
    OpenMPDeviceClauseModifier modifier =
        ((OpenMPDeviceClause *)current_omp_clause)->getModifier();
    SgOmpClause::omp_device_modifier_enum sg_modifier =
        toSgOmpClauseDeviceModifier(modifier);
    clause_expression->set_parent(current_OpenMPIR_to_SageIII.first);
    SgExpression *device_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpDeviceClause(device_expression, sg_modifier);
    printf("Device Clause added!\n");
    break;
  }
  case OMPC_partial: {
    SgExpression *partial_expression =
        checkOmpExpressionClause(clause_expression, global, e_num_threads);
    result = new SgOmpPartialClause(partial_expression);
    printf("Partial Clause added!\n");
    break;
  }
  default: {
    printf("Unknown Clause!\n");
  }
  }
  setOneSourcePositionForTransformation(result);

  // reconsider the location of following code to attach clause
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_declare_simd) {
    ((SgOmpDeclareSimdStatement *)directive)->get_clauses().push_back(result);
  } else if (current_OpenMPIR_to_SageIII.second->getKind() ==
             OMPD_target_update) {
    ((SgOmpTargetUpdateStatement *)directive)->get_clauses().push_back(result);
  } else {
    addOmpClause(directive, result);
  }
  result->set_parent(directive);

  return result;
}

void parseOmpVariable(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClauseKind clause_kind, std::string expression) {
  // special handling for omp declare simd directive
  // It may have clauses referencing a variable declared in an immediately
  // followed function's parameter list
  bool look_forward = false;
  if (current_OpenMPIR_to_SageIII.second->getKind() == OMPD_declare_simd &&
      (clause_kind == OMPC_linear || clause_kind == OMPC_simdlen ||
       clause_kind == OMPC_aligned || clause_kind == OMPC_uniform)) {
    look_forward = true;
  };
  std::string expr_string = std::string() + "varlist " + expression + "\n";
  parseExpression(current_OpenMPIR_to_SageIII.first, look_forward,
                  expr_string.c_str());
}

SgExpression *parseOmpExpression(SgPragmaDeclaration *directive,
                                 OpenMPClauseKind clause_kind,
                                 std::string expression) {
  // special handling for omp declare simd directive
  // It may have clauses referencing a variable declared in an immediately
  // followed function's parameter list
  bool look_forward = false;
  if (isSgOmpDeclareSimdStatement(directive) &&
      (clause_kind == OMPC_linear || clause_kind == OMPC_simdlen ||
       clause_kind == OMPC_aligned || clause_kind == OMPC_uniform)) {
    look_forward = true;
  };
  std::string expr_string = std::string() + "expr (" + expression + ")\n";
  SgExpression *sg_expression =
      parseExpression(directive, look_forward, expr_string.c_str());

  return sg_expression;
}

SgExpression *parseOmpArraySection(SgPragmaDeclaration *directive,
                                   OpenMPClauseKind clause_kind,
                                   std::string expression) {
  // special handling for omp declare simd directive
  // It may have clauses referencing a variable declared in an immediately
  // followed function's parameter list
  bool look_forward = false;
  if (isSgOmpDeclareSimdStatement(directive) &&
      (clause_kind == OMPC_linear || clause_kind == OMPC_simdlen ||
       clause_kind == OMPC_aligned || clause_kind == OMPC_uniform)) {
    look_forward = true;
  };
  std::string expr_string =
      std::string() + "array_section (" + expression + ")\n";
  SgExpression *sg_expression =
      parseArraySectionExpression(directive, look_forward, expr_string.c_str());

  return sg_expression;
}

void buildVariableList(SgOmpVariablesClause *current_omp_clause) {

  std::vector<std::pair<std::string, SgNode *>>::iterator iter;
  for (iter = omp_variable_list.begin(); iter != omp_variable_list.end();
       iter++) {
    if (SgInitializedName *iname = isSgInitializedName((*iter).second)) {
      SgVarRefExp *var_ref = buildVarRefExp(iname);
      current_omp_clause->get_variables()->get_expressions().push_back(var_ref);
      var_ref->set_parent(current_omp_clause);
    } else if (SgPntrArrRefExp *aref = isSgPntrArrRefExp((*iter).second)) {
      current_omp_clause->get_variables()->get_expressions().push_back(aref);
      aref->set_parent(current_omp_clause);
    } else if (SgVarRefExp *vref = isSgVarRefExp((*iter).second)) {
      current_omp_clause->get_variables()->get_expressions().push_back(vref);
      vref->set_parent(current_omp_clause);
    } else {
      cerr << "error: unhandled type of variable within a list:"
           << ((*iter).second)->class_name();
    }
  }
}

SgOmpParallelStatement *convertOmpParallelStatementFromCombinedDirectives(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII) {
  ROSE_ASSERT(current_OpenMPIR_to_SageIII.second != NULL);
  SgStatement *body = getOpenMPBlockBody(current_OpenMPIR_to_SageIII);
  removeStatement(body, false);
  ROSE_ASSERT(body != NULL);

  // build the 2nd directive node first
  SgStatement *second_stmt = NULL;
  switch (current_OpenMPIR_to_SageIII.second->getKind()) {
  case OMPD_parallel_do: {
    second_stmt = new SgOmpDoStatement(NULL, body);
    break;
  }
  case OMPD_parallel_for: {
    second_stmt = new SgOmpForStatement(NULL, body);
    break;
  }
  case OMPD_parallel_for_simd: {
    second_stmt = new SgOmpForSimdStatement(NULL, body);
    break;
  }
  case OMPD_parallel_sections: {
    second_stmt = new SgOmpSectionsStatement(NULL, body);
    break;
  }
  case OMPD_parallel_workshare: {
    second_stmt = new SgOmpWorkshareStatement(NULL, body);
    break;
  }
  default: {
    cerr << "error: unacceptable directive type in "
            "convertOmpParallelStatementFromCombinedDirectives() "
         << endl;
    ROSE_ASSERT(false);
  }
  }

  setOneSourcePositionForTransformation(second_stmt);

  ROSE_ASSERT(second_stmt);
  body->set_parent(second_stmt);

  copyStartFileInfo(current_OpenMPIR_to_SageIII.first, second_stmt);
  copyEndFileInfo(current_OpenMPIR_to_SageIII.first, second_stmt);
  SgOmpParallelStatement *first_stmt =
      new SgOmpParallelStatement(NULL, second_stmt);
  setOneSourcePositionForTransformation(first_stmt);
  copyStartFileInfo(current_OpenMPIR_to_SageIII.first, first_stmt);
  copyEndFileInfo(current_OpenMPIR_to_SageIII.first, first_stmt);
  second_stmt->set_parent(first_stmt);

  OpenMPClauseKind clause_kind;
  std::vector<OpenMPClause *> *clause_vector =
      current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
  std::vector<OpenMPClause *>::iterator citer;
  for (citer = clause_vector->begin(); citer != clause_vector->end(); citer++) {
    clause_kind = (*citer)->getKind();
    switch (clause_kind) {
    case OMPC_collapse:
    case OMPC_ordered:
    case OMPC_if:
    case OMPC_num_threads: {
      if (clause_kind == OMPC_collapse || clause_kind == OMPC_ordered) {
        convertExpressionClause(second_stmt, current_OpenMPIR_to_SageIII,
                                *citer);
      } else {
        convertExpressionClause(isSgOmpClauseBodyStatement(first_stmt),
                                current_OpenMPIR_to_SageIII, *citer);
      };
      break;
    }
    case OMPC_allocate:
    case OMPC_copyin:
    case OMPC_firstprivate:
    case OMPC_lastprivate:
    case OMPC_linear:
    case OMPC_private:
    case OMPC_reduction:
    case OMPC_shared:
    case OMPC_uniform: {
      if (clause_kind == OMPC_shared || clause_kind == OMPC_copyin) {
        convertClause(isSgOmpClauseBodyStatement(first_stmt),
                      current_OpenMPIR_to_SageIII, *citer);
      } else {
        convertClause(second_stmt, current_OpenMPIR_to_SageIII, *citer);
      };
      break;
    }
    case OMPC_default: {
      convertDefaultClause(isSgOmpClauseBodyStatement(first_stmt),
                           current_OpenMPIR_to_SageIII, *citer);
      break;
    }
    case OMPC_proc_bind: {
      convertProcBindClause(isSgOmpClauseBodyStatement(first_stmt),
                            current_OpenMPIR_to_SageIII, *citer);
      break;
    }
    case OMPC_schedule: {
      convertScheduleClause(second_stmt, current_OpenMPIR_to_SageIII, *citer);
      break;
    }
    case OMPC_parallel: {
      convertSimpleClause(second_stmt, current_OpenMPIR_to_SageIII, *citer);
      break;
    }
    default: {
      cerr << "error: unacceptable clause for combined parallel for directive"
           << endl;
      ROSE_ASSERT(false);
    }
    };
  };
  movePreprocessingInfo(body, first_stmt, PreprocessingInfo::before,
                        PreprocessingInfo::after, true);
  return first_stmt;
}

bool checkOpenMPIR(OpenMPDirective *directive) {

  if (directive == NULL) {
    return false;
  };
  OpenMPDirectiveKind directive_kind = directive->getKind();
  switch (directive_kind) {
  case OMPD_atomic:
  case OMPD_barrier:
  case OMPD_cancel:
  case OMPD_cancellation_point:
  case OMPD_critical:
  case OMPD_declare_mapper:
  case OMPD_declare_simd:
  case OMPD_declare_target:
  case OMPD_end_declare_target:
  case OMPD_depobj:
  case OMPD_distribute:
  case OMPD_do:
  case OMPD_flush:
  case OMPD_allocate:
  case OMPD_for:
  case OMPD_for_simd:
  case OMPD_loop:
  case OMPD_master:
  case OMPD_metadirective:
  case OMPD_ordered:
  case OMPD_parallel:
  case OMPD_parallel_do:
  case OMPD_parallel_for:
  case OMPD_parallel_for_simd:
  case OMPD_parallel_sections:
  case OMPD_parallel_workshare:
  case OMPD_scan:
  case OMPD_section:
  case OMPD_sections:
  case OMPD_simd:
  case OMPD_single:
  case OMPD_target:
  case OMPD_target_data:
  case OMPD_target_enter_data:
  case OMPD_target_exit_data:
  case OMPD_target_parallel_for:
  case OMPD_target_parallel:
  case OMPD_distribute_simd:
  case OMPD_distribute_parallel_for:
  case OMPD_distribute_parallel_for_simd:
  case OMPD_taskloop_simd:
  case OMPD_target_update:
  case OMPD_requires:
  case OMPD_target_parallel_for_simd:
  case OMPD_target_parallel_loop:
  case OMPD_target_simd:
  case OMPD_target_teams:
  case OMPD_target_teams_distribute:
  case OMPD_target_teams_distribute_simd:
  case OMPD_target_teams_loop:
  case OMPD_target_teams_distribute_parallel_for:
  case OMPD_target_teams_distribute_parallel_for_simd:
  case OMPD_master_taskloop_simd:
  case OMPD_parallel_master_taskloop:
  case OMPD_parallel_master_taskloop_simd:
  case OMPD_teams_distribute:
  case OMPD_teams_distribute_simd:
  case OMPD_teams_distribute_parallel_for:
  case OMPD_teams_distribute_parallel_for_simd:
  case OMPD_teams_loop:
  case OMPD_parallel_master:
  case OMPD_master_taskloop:
  case OMPD_parallel_loop:
  case OMPD_task:
  case OMPD_taskgroup:
  case OMPD_taskloop:
  case OMPD_taskwait:
  case OMPD_taskyield:
  case OMPD_teams:
  case OMPD_threadprivate:
  case OMPD_workshare:
  case OMPD_tile:
  case OMPD_unroll: {
    break;
  }
  default: {
    return false;
  }
  };
  std::map<OpenMPClauseKind, std::vector<OpenMPClause *> *> *clauses =
      directive->getAllClauses();
  if (clauses != NULL) {
    std::map<OpenMPClauseKind, std::vector<OpenMPClause *> *>::iterator it;
    for (it = clauses->begin(); it != clauses->end(); it++) {
      switch (it->first) {
      case OMPC_acq_rel:
      case OMPC_acquire:
      case OMPC_aligned:
      case OMPC_allocate:
      case OMPC_allocator:
      case OMPC_bind:
      case OMPC_to:
      case OMPC_from:
      case OMPC_capture:
      case OMPC_collapse:
      case OMPC_copyin:
      case OMPC_copyprivate:
      case OMPC_default:
      case OMPC_defaultmap:
      case OMPC_depend:
      case OMPC_affinity:
      case OMPC_depobj_update:
      case OMPC_destroy:
      case OMPC_detach:
      case OMPC_device:
      case OMPC_dist_schedule:
      case OMPC_exclusive:
      case OMPC_final:
      case OMPC_firstprivate:
      case OMPC_for:
      case OMPC_grainsize:
      case OMPC_hint:
      case OMPC_if:
      case OMPC_in_reduction:
      case OMPC_inbranch:
      case OMPC_inclusive:
      case OMPC_is_device_ptr:
      case OMPC_lastprivate:
      case OMPC_linear:
      case OMPC_map:
      case OMPC_mergeable:
      case OMPC_nogroup:
      case OMPC_nontemporal:
      case OMPC_notinbranch:
      case OMPC_nowait:
      case OMPC_num_tasks:
      case OMPC_num_teams:
      case OMPC_num_threads:
      case OMPC_order:
      case OMPC_ordered:
      case OMPC_parallel:
      case OMPC_priority:
      case OMPC_private:
      case OMPC_proc_bind:
      case OMPC_read:
      case OMPC_reverse_offload:
      case OMPC_unified_address:
      case OMPC_unified_shared_memory:
      case OMPC_dynamic_allocators:
      case OMPC_atomic_default_mem_order:
      case OMPC_ext_implementation_defined_requirement:
      case OMPC_reduction:
      case OMPC_relaxed:
      case OMPC_release:
      case OMPC_safelen:
      case OMPC_schedule:
      case OMPC_sections:
      case OMPC_seq_cst:
      case OMPC_shared:
      case OMPC_simdlen:
      case OMPC_task_reduction:
      case OMPC_taskgroup:
      case OMPC_thread_limit:
      case OMPC_uniform:
      case OMPC_untied:
      case OMPC_update:
      case OMPC_use_device_addr:
      case OMPC_use_device_ptr:
      case OMPC_uses_allocators:
      case OMPC_when:
      case OMPC_threads:
      case OMPC_simd:
      case OMPC_write:
      case OMPC_full:
      case OMPC_partial:
      case OMPC_sizes: {
        break;
      }
      default: {
        return false;
      }
      };
    };
  };
  return true;
}
