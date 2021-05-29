// Put here code used to construct SgOmp* nodes
// Liao 10/8/2010

#include "sage3basic.h"

#include "rose_paths.h"

#include "astPostProcessing.h"
#include "sageBuilder.h"
#include "OmpAttribute.h"
#include "ompAstConstruction.h"

#include "OpenMPIR.h"
#include <deque>
#include <tuple>

extern OpenMPDirective* parseOpenMP(const char*, void * _exprParse(const char*));

//Liao, 10/27/2008: parsing OpenMP pragma here
//Handle OpenMP pragmas. This should be called after preprocessing information is attached since macro calls may exist within pragmas, Liao, 3/31/2009
extern int omp_exprparser_parse();
extern SgExpression* parseExpression(SgNode*, bool, const char*);
extern SgExpression* parseArraySectionExpression(SgNode*, bool, const char*);
extern void omp_exprparser_parser_init(SgNode* aNode, const char* str);
//Fortran OpenMP parser interface
void parse_fortran_openmp(SgSourceFile *sageFilePtr);
static OpenMPDirective* ompparser_OpenMPIR;

static bool use_ompparser = false;
static bool checkOpenMPIR(OpenMPDirective*);
static void parseFortran(SgSourceFile*);
static SgStatement* convertDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgStatement* convertVariantDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgOmpBodyStatement* convertBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgOmpBodyStatement* convertCombinedBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgOmpBodyStatement* convertVariantBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgStatement* convertOmpThreadprivateStatement(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII);
static SgStatement* getOpenMPBlockBody(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgOmpVariablesClause* convertClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static void buildVariableList(SgOmpVariablesClause*);
static SgOmpExpressionClause* convertExpressionClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpClause* convertSimpleClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpDepobjUpdateClause *convertDepobjUpdateClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause);
static SgOmpScheduleClause* convertScheduleClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpDistScheduleClause* convertDistScheduleClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpDefaultmapClause* convertDefaultmapClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpDefaultClause* convertDefaultClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpProcBindClause* convertProcBindClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpOrderClause* convertOrderClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpBindClause* convertBindClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpWhenClause* convertWhenClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
// store temporary expression pairs for ompparser.
extern std::vector<std::pair<std::string, SgNode*> > omp_variable_list;
extern std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions;
extern SgExpression* omp_expression;
static SgExpression* parseOmpExpression(SgPragmaDeclaration*, OpenMPClauseKind, std::string);
static SgExpression* parseOmpArraySection(SgPragmaDeclaration*, OpenMPClauseKind, std::string);
static SgOmpParallelStatement* convertOmpParallelStatementFromCombinedDirectives(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgStatement* convertNonBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*>);
static SgOmpMapClause* convertMapClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static SgOmpDependClause* convertDependClause(SgOmpClauseBodyStatement*, std::pair<SgPragmaDeclaration*, OpenMPDirective*>, OpenMPClause*);
static std::map<SgPragmaDeclaration*, OpenMPDirective*> fortran_paired_pragma_list;
static std::vector<std::tuple<SgLocatedNode*, PreprocessingInfo*, OpenMPDirective*>> fortran_omp_pragma_list;

// Fortran
static const char* c_char = NULL; // current characters being scanned
static SgNode* c_sgnode = NULL; // current SgNode associated with the OpenMP directive

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

// Liao 4/23/2011, special function to copy file info of the original SgPragma or Fortran comments
static bool copyStartFileInfo (SgNode* src, SgNode* dest, OmpAttribute* oa)
{
  bool result = false;
  ROSE_ASSERT (src && dest);
  // same src and dest, no copy is needed
  if (src == dest) return true;

  SgLocatedNode* lsrc = isSgLocatedNode(src);
  ROSE_ASSERT (lsrc);
  SgLocatedNode* ldest= isSgLocatedNode(dest);
  ROSE_ASSERT (ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if (lsrc->get_startOfConstruct()->get_filename() == ldest->get_startOfConstruct()->get_filename()
      && lsrc->get_startOfConstruct()->get_line() == ldest->get_startOfConstruct()->get_line()
      && lsrc->get_startOfConstruct()->get_col() == ldest->get_startOfConstruct()->get_col())
    return true; 

  Sg_File_Info* copy = new Sg_File_Info (*(lsrc->get_startOfConstruct())); 
  ROSE_ASSERT (copy != NULL);

   // delete old start of construct
  Sg_File_Info *old_info = ldest->get_startOfConstruct();
  if (old_info) delete (old_info);

  ldest->set_startOfConstruct(copy);
  copy->set_parent(ldest);
//  cout<<"debug: set ldest@"<<ldest <<" with file info @"<< copy <<endl;

  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_filename() == ldest->get_startOfConstruct()->get_filename());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_line() == ldest->get_startOfConstruct()->get_line());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_col() == ldest->get_startOfConstruct()->get_col());

  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_filename() == ldest->get_file_info()->get_filename());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_line() == ldest->get_file_info()->get_line());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_col() == ldest->get_file_info()->get_col());

  ROSE_ASSERT (ldest->get_file_info() == copy);
// Adjustment for Fortran, the AST node attaching the Fortran comment will not actual give out the accurate line number for the comment
  if (is_Fortran_language())
  {
    int commentLine = ompparser_OpenMPIR->getLine();
    ldest->get_file_info()->set_line(commentLine);
  }
    
  return result;
}
// Liao 3/11/2013, special function to copy end file info of the original SgPragma or Fortran comments (src) to OpenMP node (dest)
// If the OpenMP node is a body statement, we have to use the body's end file info as the node's end file info.
static bool copyEndFileInfo (SgNode* src, SgNode* dest, OmpAttribute* oa)
{
  bool result = false;
  ROSE_ASSERT (src && dest);
  
  if (isSgOmpBodyStatement(dest))
    src = isSgOmpBodyStatement(dest)->get_body();

  // same src and dest, no copy is needed
  if (src == dest) return true;

  SgLocatedNode* lsrc = isSgLocatedNode(src);
  ROSE_ASSERT (lsrc);
  SgLocatedNode* ldest= isSgLocatedNode(dest);
  ROSE_ASSERT (ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if    (lsrc->get_endOfConstruct()->get_filename() == ldest->get_endOfConstruct()->get_filename()
      && lsrc->get_endOfConstruct()->get_line()     == ldest->get_endOfConstruct()->get_line()
      && lsrc->get_endOfConstruct()->get_col()      == ldest->get_endOfConstruct()->get_col())
    return true; 

  Sg_File_Info* copy = new Sg_File_Info (*(lsrc->get_endOfConstruct())); 
  ROSE_ASSERT (copy != NULL);

   // delete old start of construct
  Sg_File_Info *old_info = ldest->get_endOfConstruct();
  if (old_info) delete (old_info);

  ldest->set_endOfConstruct(copy);
  copy->set_parent(ldest);
//  cout<<"debug: set ldest@"<<ldest <<" with file info @"<< copy <<endl;

  ROSE_ASSERT (lsrc->get_endOfConstruct()->get_filename() == ldest->get_endOfConstruct()->get_filename());
  ROSE_ASSERT (lsrc->get_endOfConstruct()->get_line()     == ldest->get_endOfConstruct()->get_line());
  ROSE_ASSERT (lsrc->get_endOfConstruct()->get_col()      == ldest->get_endOfConstruct()->get_col());


  ROSE_ASSERT (ldest->get_endOfConstruct() == copy);
// Adjustment for Fortran, the AST node attaching the Fortran comment will not actual give out the accurate line number for the comment
  if (is_Fortran_language())
  { //TODO fortran support of end file info
//    cerr<<"Error. ompAstConstruction.cpp: copying end file info is not yet implemented for Fortran."<<endl;
//    ROSE_ASSERT (false);
//    ROSE_ASSERT (oa != NULL);
//    PreprocessingInfo *currentPreprocessingInfoPtr = oa->getPreprocessingInfo();
//    ROSE_ASSERT (currentPreprocessingInfoPtr != NULL);
//    int commentLine = currentPreprocessingInfoPtr->getLineNumber(); 
//    ldest->get_file_info()->set_line(commentLine);
  }
    
  return result;
}


namespace OmpSupport
{ 
  // an internal data structure to avoid redundant AST traversal to find OpenMP pragmas
  static std::list<SgPragmaDeclaration* > omp_pragma_list;

    // the vector of pairs of OpenMP pragma and Ompparser IR.
    static std::vector<std::pair<SgPragmaDeclaration*, OpenMPDirective*> > OpenMPIR_list;
    //static OpenMPDirective* ompparser_OpenMPIR;

  // a similar list to save encountered Fortran comments which are OpenMP directives
  std::list<OmpAttribute* > omp_comment_list; 
  // A pragma list to store the dangling pragmas for Fortran end directives. 
  // There are stored to ensure correct unparsing after converting Fortran comments into pragmas
  // But they should be immediately removed during the OpenMP lowering phase
 //  static std::list<SgPragmaDeclaration* > omp_end_pragma_list; 

  // find all SgPragmaDeclaration nodes within a file and parse OpenMP pragmas into OmpAttribute info.
  void attachOmpAttributeInfo(SgSourceFile *sageFilePtr)
  {
    ROSE_ASSERT(sageFilePtr != NULL);
    if (sageFilePtr->get_openmp() == false)
      return;
    // For Fortran, search comments for OpenMP directives
    if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
        sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
    {
        // use ompparser to process Fortran.
        parseFortran(sageFilePtr);
    } //end if (fortran)
    else
    {
      // For C/C++, search pragma declarations for OpenMP directives 
      std::vector <SgNode*> all_pragmas = NodeQuery::querySubTree (sageFilePtr, V_SgPragmaDeclaration);
      std::vector<SgNode*>::iterator iter;
      for(iter=all_pragmas.begin();iter!=all_pragmas.end();iter++)
      {
        SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(*iter);
        ROSE_ASSERT(pragmaDeclaration != NULL);
#if 0 // We should not enforce this since the pragma may come from transformation-generated node
        if ((pragmaDeclaration->get_file_info()->isTransformation()
            && pragmaDeclaration->get_file_info()->get_filename()==string("transformation")))
        {
          cout<<"Found a pragma which is transformation generated. @"<< pragmaDeclaration;
          cout<<pragmaDeclaration->unparseToString()<<endl;
          pragmaDeclaration->get_file_info()->display("debug transformation generated pragma declaration.");
          // Liao 4/23/2011
          // #pragma omp task can shown up before a single statement body of a for loop, 
          // In this case, the frontend will insert a basic block under the loop
          // and put both the pragma and the single statement into the block.

          // AstPostProcessing() will reset the transformation flag for the pragma
          // since its parent(the block) is transformation generated, not in the original code
          ROSE_ASSERT(pragmaDeclaration->get_file_info()->isTransformation() ==false  || pragmaDeclaration->get_file_info()->get_filename()!=string("transformation"));
        }
#endif  
        SageInterface::replaceMacroCallsWithExpandedStrings(pragmaDeclaration);
        string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
        istringstream istr(pragmaString);
        std::string key;
        istr >> key;
        if (key == "omp")
        {
          // Liao, 3/12/2009
          // Outliner may move pragma statements to a new file
          // after the pragma has been attached OmpAttribute.
          // We have to skip generating the attribute again in the new file
          OmpAttributeList* previous = getOmpAttributeList(pragmaDeclaration);
          // store them into a buffer, reused by build_OpenMP_AST()
          omp_pragma_list.push_back(pragmaDeclaration);

          if (previous == NULL )
          {
            // Call parser
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT

            // parse expression
            // Get the object that ompparser IR.
            ompparser_OpenMPIR = parseOpenMP(pragmaString.c_str(), NULL);
            use_ompparser = checkOpenMPIR(ompparser_OpenMPIR);
            if (use_ompparser) {
                OpenMPIR_list.push_back(std::make_pair(pragmaDeclaration, ompparser_OpenMPIR));
            } else {
                omp_exprparser_parser_init(pragmaDeclaration, pragmaString.c_str());
                omp_exprparser_parse();
            };
#endif
            if (!use_ompparser) {
                //OmpAttribute* attribute = getParsedDirective();
                OmpAttribute* attribute = NULL;
            //cout<<"sage_gen_be.C:23758 debug:\n"<<pragmaString<<endl;
            //attribute->print();//debug only for now
                addOmpAttribute(attribute,pragmaDeclaration);
            //cout<<"debug: attachOmpAttributeInfo() for a pragma:"<<pragmaString<<"at address:"<<pragmaDeclaration<<endl;
            //cout<<"file info for it is:"<<pragmaDeclaration->get_file_info()->get_filename()<<endl;
#if 1 // Liao, 2/12/2010, this could be a bad idea. It causes trouble in comparing 
            //user-defined and compiler-generated OmpAttribute.
            // We attach the attribute redundantly on affected loops also
            // for easier loop handling later on in autoTuning's outlining step (reproducing lost pragmas)
                if (attribute->getOmpDirectiveType() ==e_for ||attribute->getOmpDirectiveType() ==e_parallel_for) {
                    SgForStatement* forstmt = isSgForStatement(getNextStatement(pragmaDeclaration));
                    ROSE_ASSERT(forstmt != NULL);
              //forstmt->addNewAttribute("OmpAttribute",attribute);
                    addOmpAttribute(attribute,forstmt);
                };
#endif
            };
          }
        }
      }// end for
    }
  }
  // Clause node builders
  //----------------------------------------------------------

  // Sara Royuela ( Nov 2, 2012 ): Check for clause parameters that can be defined in macros
  // This adds support for the use of macro definitions in OpenMP clauses
  // We need a traversal over SgExpression to support macros in any position of an "assignment_expr"
  // F.i.:   #define THREADS_1 16
  //         #define THREADS_2 8
  //         int main( int arg, char** argv ) {
  //         #pragma omp parallel num_threads( THREADS_1 + THREADS_2 )
  //           {}
  //         }
  SgVarRefExpVisitor::SgVarRefExpVisitor()
        : expressions()
  {}
  
  std::vector<SgExpression*> SgVarRefExpVisitor::get_expressions()
  {
      return expressions;
  }
  
  void SgVarRefExpVisitor::visit(SgNode* node)
  {
      SgExpression* expr = isSgVarRefExp(node);
      if(expr != NULL)
      {
          expressions.push_back(expr);
      }
  }
  
  SgExpression* replace_expression_with_macro_value( std::string define_macro, SgExpression* old_exp, 
                                                     bool& macro_replaced, omp_construct_enum clause_type )
  {
      SgExpression* newExp = old_exp;
      // Parse the macro: we are only interested in macros with the form #define MACRO_NAME MACRO_VALUE, the constant macro
      size_t parenthesis = define_macro.find("(");
      if(parenthesis == string::npos)
      {   // Non function macro, constant macro
          unsigned int macroNameInitPos = (unsigned int)(define_macro.find("define")) + 6;
          while(macroNameInitPos<define_macro.size() && define_macro[macroNameInitPos]==' ')
              macroNameInitPos++;
          unsigned int macroNameEndPos = define_macro.find(" ", macroNameInitPos);
          std::string macroName = define_macro.substr(macroNameInitPos, macroNameEndPos-macroNameInitPos);
                                  
          if(macroName == isSgVarRefExp(old_exp)->get_symbol()->get_name().getString())
          {   // Clause is defined in a macro
              size_t comma = define_macro.find(",");
              if(comma == string::npos)       // Macros like "#define MACRO_NAME VALUE1, VALUE2" are not accepted
              {   // We create here an expression with the value of the clause defined in the macro
                  unsigned int macroValueInitPos = macroNameEndPos + 1;
                  while(macroValueInitPos<define_macro.size() && define_macro[macroValueInitPos]==' ')
                      macroValueInitPos++;
                  unsigned int macroValueEndPos = macroValueInitPos; 
                  while(macroValueEndPos<define_macro.size() && 
                        define_macro[macroValueEndPos]!=' ' && define_macro[macroValueEndPos]!='\n')
                      macroValueEndPos++;        
                  std::string macroValue = define_macro.substr(macroValueInitPos, macroValueEndPos-macroValueInitPos);
                  
                  // Check whether the value is a valid integer
                  std::string::const_iterator it = macroValue.begin();
                  while (it != macroValue.end() && std::isdigit(*it)) 
                      ++it;
                  ROSE_ASSERT(!macroValue.empty() && it == macroValue.end());
                  
                  newExp = buildIntVal(atoi(macroValue.c_str()));
                  if(!isSgPragmaDeclaration(old_exp->get_parent()))
                      replaceExpression(old_exp, newExp);
                  macro_replaced = true;
              }
          }
      }
      return newExp;
  }
  
  SgExpression* checkOmpExpressionClause( SgExpression* clause_expression, SgGlobal* global, omp_construct_enum clause_type )
  {
      SgExpression* newExp = clause_expression;
      // ordered (n): optional (n)
      if (clause_expression == NULL && clause_type == e_ordered_clause)
         return NULL; 
      ROSE_ASSERT(clause_expression != NULL);
      bool returnNewExpression = false;
      if( isSgTypeUnknown( clause_expression->get_type( ) ) )
      {
          SgVarRefExpVisitor v;
          v.traverse(clause_expression, preorder);
          std::vector<SgExpression*> expressions = v.get_expressions();
          if( !expressions.empty() )
          {
              if( expressions.size() == 1 )
              {   // create the new expression and return it
                  // otherwise, replace the expression and return the original, which is now modified 
                  returnNewExpression = true;
              }
              
              bool macroReplaced;
              SgDeclarationStatementPtrList& declarations = global->get_declarations();
              while( !expressions.empty() )
              {
                  macroReplaced = false;
                  SgExpression* oldExp = expressions.back();
                  for(SgDeclarationStatementPtrList::iterator declIt = declarations.begin(); declIt != declarations.end() && !macroReplaced; ++declIt) 
                  {
                      SgDeclarationStatement * declaration = *declIt;
                      AttachedPreprocessingInfoType * preprocInfo = declaration->getAttachedPreprocessingInfo();
                      if( preprocInfo != NULL )
                      {   // There is preprocessed info attached to the current node
                          for(AttachedPreprocessingInfoType::iterator infoIt = preprocInfo->begin(); 
                              infoIt != preprocInfo->end() && !macroReplaced; infoIt++)
                          {
                              if((*infoIt)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
                              {
                                  newExp = replace_expression_with_macro_value( (*infoIt)->getString(), oldExp, macroReplaced, clause_type );
                              }
                          }
                      }
                  }
                  
                  // When a macro is defined in a header without any statement, the preprocessed information is attached to the SgFile
                  if(!macroReplaced)
                  {
                      SgProject* project = SageInterface::getProject();
                      int nFiles = project->numberOfFiles();
                      for(int fileIt=0; fileIt<nFiles && !macroReplaced; fileIt++)
                      {
                          SgFile& file = project->get_file(fileIt);
                          ROSEAttributesListContainerPtr filePreprocInfo = file.get_preprocessorDirectivesAndCommentsList();
                          if( filePreprocInfo != NULL )
                          {
                              std::map<std::string, ROSEAttributesList*> preprocInfoMap =  filePreprocInfo->getList();
                              for(std::map<std::string, ROSEAttributesList*>::iterator mapIt=preprocInfoMap.begin(); 
                                  mapIt!=preprocInfoMap.end() && !macroReplaced; mapIt++)
                              {
                                  std::vector<PreprocessingInfo*> preprocInfoList = mapIt->second->getList();
                                  for(std::vector<PreprocessingInfo*>::iterator infoIt=preprocInfoList.begin(); 
                                      infoIt!=preprocInfoList.end() && !macroReplaced; infoIt++)
                                  {
                                      if((*infoIt)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
                                      {
                                          newExp = replace_expression_with_macro_value( (*infoIt)->getString(), oldExp, macroReplaced, clause_type );
                                      }
                                  }
                              }
                          }
                      }
                  }
                  
                  expressions.pop_back();
              }
          }
          else
          {
              printf("error in checkOmpExpressionClause(): no expression found in an expression clause\n");
              ROSE_ASSERT(false);
          }
      }
      
      return (returnNewExpression ? newExp : clause_expression);
  }

  //TODO: move this builder functions to SageBuilder namespace
  SgOmpEndClause * buildOmpEndClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    // check if input attribute has e_end clause
    if (!att->hasClause(e_end))
      return NULL;
    SgOmpEndClause* result = new SgOmpEndClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpBeginClause * buildOmpBeginClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    // check if input attribute has e_end clause
    if (!att->hasClause(e_begin))
      return NULL;
    SgOmpBeginClause* result = new SgOmpBeginClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpInbranchClause * buildOmpInbranchClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_inbranch))
      return NULL;
    SgOmpInbranchClause* result = new SgOmpInbranchClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpNotinbranchClause * buildOmpNotinbranchClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_notinbranch))
      return NULL;
    SgOmpNotinbranchClause* result = new SgOmpNotinbranchClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  //! A helper function to convert OpenMPIfClause modifier to SgClause if modifier
  static SgOmpClause::omp_if_modifier_enum toSgOmpClauseIfModifier(OpenMPIfClauseModifier modifier)
  {
    SgOmpClause::omp_if_modifier_enum result;
    switch (modifier)
    {
      case OMPC_IF_MODIFIER_parallel:
        {
          result = SgOmpClause::e_omp_if_parallel;
          break;
        }
      case OMPC_IF_MODIFIER_simd:
        {
          result = SgOmpClause::e_omp_if_simd;
          break;
        }
      case OMPC_IF_MODIFIER_cancel:
        {
          result = SgOmpClause::e_omp_if_cancel;
          break;
        }
      case OMPC_IF_MODIFIER_taskloop:
        {
          result = SgOmpClause::e_omp_if_taskloop;
          break;
        }
      case OMPC_IF_MODIFIER_target_enter_data:
        {
          result = SgOmpClause::e_omp_if_target_enter_data;
          break;
        }
      case OMPC_IF_MODIFIER_target_exit_data:
        {
          result = SgOmpClause::e_omp_if_target_exit_data;
          break;
        }
      case OMPC_IF_MODIFIER_task:
        {
          result = SgOmpClause::e_omp_if_task;
          break;
        }
      case OMPC_IF_MODIFIER_target_data:
        {
          result = SgOmpClause::e_omp_if_target_data;
          break;
        }
      case OMPC_IF_MODIFIER_target:
        {
          result = SgOmpClause::e_omp_if_target;
          break;
        }
      case OMPC_IF_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_if_modifier_unknown;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for if modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_lastprivate_modifier_enum toSgOmpClauseLastprivateModifier(OpenMPLastprivateClauseModifier modifier)
  {
    SgOmpClause::omp_lastprivate_modifier_enum result = SgOmpClause::e_omp_lastprivate_modifier_unspecified;
    switch (modifier)
    {
      case OMPC_LASTPRIVATE_MODIFIER_conditional:
        {
          result = SgOmpClause::e_omp_lastprivate_conditional;
          break;
        }
      case OMPC_LASTPRIVATE_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_lastprivate_modifier_unspecified;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for lastprivate modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_device_modifier_enum toSgOmpClauseDeviceModifier(OpenMPDeviceClauseModifier modifier)
  {
    SgOmpClause::omp_device_modifier_enum result = SgOmpClause::e_omp_device_modifier_unspecified;
    switch (modifier)
    {
      case OMPC_DEVICE_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_device_modifier_unspecified;
          break;
        }
      case OMPC_DEVICE_MODIFIER_ancestor:
        {
          result = SgOmpClause::e_omp_device_modifier_ancestor;
          break;
        }
      case OMPC_DEVICE_MODIFIER_device_num:
        {
          result = SgOmpClause::e_omp_device_modifier_device_num;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for device modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_schedule_modifier_enum toSgOmpClauseScheduleModifier(OpenMPScheduleClauseModifier modifier)
  {
    SgOmpClause::omp_schedule_modifier_enum result = SgOmpClause::e_omp_schedule_modifier_unspecified;
    switch (modifier)
    {
      case OMPC_SCHEDULE_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_schedule_modifier_unspecified;
          break;
        }
      case OMPC_SCHEDULE_MODIFIER_monotonic:
        {
          result = SgOmpClause::e_omp_schedule_modifier_monotonic;
          break;
        }
      case OMPC_SCHEDULE_MODIFIER_nonmonotonic:
        {
          result = SgOmpClause::e_omp_schedule_modifier_nonmonotonic;
          break;
        }
      case OMPC_SCHEDULE_MODIFIER_simd:
        {
          result = SgOmpClause::e_omp_schedule_modifier_simd;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for schedule modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_schedule_kind_enum toSgOmpClauseScheduleKind(OpenMPScheduleClauseKind kind)
  {
    SgOmpClause::omp_schedule_kind_enum result = SgOmpClause::e_omp_schedule_kind_unspecified;
    switch (kind)
    {
      case OMPC_SCHEDULE_KIND_unspecified:
        {
          result = SgOmpClause::e_omp_schedule_kind_unspecified;
          break;
        }
      case OMPC_SCHEDULE_KIND_static:
        {
          result = SgOmpClause::e_omp_schedule_kind_static;
          break;
        }
      case OMPC_SCHEDULE_KIND_dynamic:
        {
          result = SgOmpClause::e_omp_schedule_kind_dynamic;
          break;
        }
      case OMPC_SCHEDULE_KIND_guided:
        {
          result = SgOmpClause::e_omp_schedule_kind_guided;
          break;
        }
      case OMPC_SCHEDULE_KIND_auto:
        {
          result = SgOmpClause::e_omp_schedule_kind_auto;
          break;
        }
      case OMPC_SCHEDULE_KIND_runtime:
        {
          result = SgOmpClause::e_omp_schedule_kind_runtime;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for schedule kind conversion:%d\n", kind);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_defaultmap_behavior_enum toSgOmpClauseDefaultmapBehavior(OpenMPDefaultmapClauseBehavior behavior)
  {
    SgOmpClause::omp_defaultmap_behavior_enum result = SgOmpClause::e_omp_defaultmap_behavior_unspecified;
    switch (behavior)
    {
      case OMPC_DEFAULTMAP_BEHAVIOR_alloc:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_alloc;
          break;
        }
      case OMPC_DEFAULTMAP_BEHAVIOR_to:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_to;
          break;
        }
      case OMPC_DEFAULTMAP_BEHAVIOR_from:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_from;
          break;
        }
      case OMPC_DEFAULTMAP_BEHAVIOR_tofrom:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_tofrom;
          break;
        }
      case OMPC_DEFAULTMAP_BEHAVIOR_firstprivate:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_firstprivate;
          break;
        }
      case OMPC_DEFAULTMAP_BEHAVIOR_none:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_none;
          break;
        }
      case OMPC_DEFAULTMAP_BEHAVIOR_default:
        {
          result = SgOmpClause::e_omp_defaultmap_behavior_default;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for defaultmap behavior conversion:%d\n", behavior);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_map_operator_enum toSgOmpClauseMapOperator(OpenMPMapClauseType at_op)
  {
    SgOmpClause::omp_map_operator_enum result = SgOmpClause::e_omp_map_unknown;
    switch (at_op)
    {
      case OMPC_MAP_TYPE_tofrom: 
        {
          result = SgOmpClause::e_omp_map_tofrom;
          break;
        }
      case OMPC_MAP_TYPE_to: 
        {
          result = SgOmpClause::e_omp_map_to;
          break;
        }
      case OMPC_MAP_TYPE_from: 
        {
          result = SgOmpClause::e_omp_map_from;
          break;
        }
      case OMPC_MAP_TYPE_alloc: 
        {
          result = SgOmpClause::e_omp_map_alloc;
          break;
        }
      default:
        {
         //printf("error: unacceptable omp construct enum for map operator conversion:%s\n", OmpSupport::toString(at_op).c_str());
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_map_unknown);
    return result;
  }

  static SgOmpClause::omp_defaultmap_category_enum toSgOmpClauseDefaultmapCategory(OpenMPDefaultmapClauseCategory category)
  {
    SgOmpClause::omp_defaultmap_category_enum result = SgOmpClause::e_omp_defaultmap_category_unspecified;
    switch (category)
    {
      case OMPC_DEFAULTMAP_CATEGORY_unspecified:
        {
          result = SgOmpClause::e_omp_defaultmap_category_unspecified;
          break;
        }
      case OMPC_DEFAULTMAP_CATEGORY_scalar:
        {
          result = SgOmpClause::e_omp_defaultmap_category_scalar;
          break;
        }
      case OMPC_DEFAULTMAP_CATEGORY_aggregate:
        {
          result = SgOmpClause::e_omp_defaultmap_category_aggregate;
          break;
        }
      case OMPC_DEFAULTMAP_CATEGORY_pointer:
        {
          result = SgOmpClause::e_omp_defaultmap_category_pointer;
          break;
        }
      case OMPC_DEFAULTMAP_CATEGORY_allocatable:
        {
          result = SgOmpClause::e_omp_defaultmap_category_allocatable;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for defaultmap category conversion:%d\n", category);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_dist_schedule_kind_enum toSgOmpClauseDistScheduleKind(OpenMPDistScheduleClauseKind kind)
  {
    SgOmpClause::omp_dist_schedule_kind_enum result = SgOmpClause::e_omp_dist_schedule_kind_unspecified;
    switch (kind)
    {
      case OMPC_DIST_SCHEDULE_KIND_static:
        {
          result = SgOmpClause::e_omp_dist_schedule_kind_static;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for dist_schedule kind conversion:%d\n", kind);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  static SgOmpClause::omp_linear_modifier_enum toSgOmpClauseLinearModifier(OpenMPLinearClauseModifier modifier)
  {
    SgOmpClause::omp_linear_modifier_enum result = SgOmpClause::e_omp_linear_modifier_unspecified;
    switch (modifier)
    {
      case OMPC_LINEAR_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_linear_modifier_unspecified;
          break;
        }
      case OMPC_LINEAR_MODIFIER_ref:
        {
          result = SgOmpClause::e_omp_linear_modifier_ref;
          break;
        }
      case OMPC_LINEAR_MODIFIER_val:
        {
          result = SgOmpClause::e_omp_linear_modifier_val;
          break;
        }
      case OMPC_LINEAR_MODIFIER_uval:
        {
          result = SgOmpClause::e_omp_linear_modifier_uval;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for linear modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  //! A helper function to convert OpenMPIR reduction modifier to SgClause reduction modifier
  static SgOmpClause::omp_reduction_modifier_enum toSgOmpClauseReductionModifier(OpenMPReductionClauseModifier modifier)
  {
    SgOmpClause::omp_reduction_modifier_enum result;
    switch (modifier)
    {
      case OMPC_REDUCTION_MODIFIER_inscan:
        {
          result = SgOmpClause::e_omp_reduction_inscan;
          break;
        }
      case OMPC_REDUCTION_MODIFIER_task:
        {
          result = SgOmpClause::e_omp_reduction_task;
          break;
        }
      case OMPC_REDUCTION_MODIFIER_default:
        {
          result = SgOmpClause::e_omp_reduction_default;
          break;
        }
      case OMPC_REDUCTION_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_reduction_modifier_unknown;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for reduction modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
        }
    }
    return result;
  }

  //! A helper function to convert OpenMPIR reduction identifier to SgClause reduction identifier
  static SgOmpClause::omp_reduction_identifier_enum toSgOmpClauseReductionIdentifier(OpenMPReductionClauseIdentifier identifier)
  {
    SgOmpClause::omp_reduction_identifier_enum result = SgOmpClause::e_omp_reduction_unknown;
    switch (identifier)
    {
      case OMPC_REDUCTION_IDENTIFIER_plus: //+
        {
          result = SgOmpClause::e_omp_reduction_plus;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_mul:  //*
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
      case OMPC_REDUCTION_IDENTIFIER_bitor:  // |
        {
          result = SgOmpClause::e_omp_reduction_bitor;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_bitxor:  // ^
        {
          result = SgOmpClause::e_omp_reduction_bitxor;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_logand:  // &&
        {
          result = SgOmpClause::e_omp_reduction_logand;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_logor:   // ||
        {
          result = SgOmpClause::e_omp_reduction_logor;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_max:
        {
          result = SgOmpClause::e_omp_reduction_max;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_min:
        {
          result = SgOmpClause::e_omp_reduction_min;
          break;
        }
      case OMPC_REDUCTION_IDENTIFIER_user:
        {
          result = SgOmpClause::e_omp_reduction_user_defined_identifier;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for reduction operator conversion:%d\n", identifier);
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_reduction_unknown);
    return result;
  }

  //! A helper function to convert OpenMPIR reduction identifier to SgClause reduction identifier
  static SgOmpClause::omp_in_reduction_identifier_enum toSgOmpClauseInReductionIdentifier(OpenMPInReductionClauseIdentifier identifier)
  {
    SgOmpClause::omp_in_reduction_identifier_enum result = SgOmpClause::e_omp_in_reduction_identifier_unspecified;
    switch (identifier)
    {
      case OMPC_IN_REDUCTION_IDENTIFIER_plus: //+
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_plus;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_mul:  //*
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
      case OMPC_IN_REDUCTION_IDENTIFIER_bitor:  // |
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_bitor;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_bitxor:  // ^
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_bitxor;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_logand:  // &&
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_logand;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_logor:   // ||
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_logor;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_max:
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_max;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_min:
        {
          result = SgOmpClause::e_omp_in_reduction_identifier_min;
          break;
        }
      case OMPC_IN_REDUCTION_IDENTIFIER_user:
        {
          result = SgOmpClause::e_omp_in_reduction_user_defined_identifier;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for in_reduction operator conversion:%d\n", identifier);
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_in_reduction_identifier_unspecified);
    return result;
  }

  //! A helper function to convert OpenMPIR reduction identifier to SgClause reduction identifier
  static SgOmpClause::omp_task_reduction_identifier_enum toSgOmpClauseTaskReductionIdentifier(OpenMPTaskReductionClauseIdentifier identifier)
  {
    SgOmpClause::omp_task_reduction_identifier_enum result = SgOmpClause::e_omp_task_reduction_identifier_unspecified;
    switch (identifier)
    {
      case OMPC_TASK_REDUCTION_IDENTIFIER_plus: //+
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_plus;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_mul:  //*
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
      case OMPC_TASK_REDUCTION_IDENTIFIER_bitor:  // |
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_bitor;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_bitxor:  // ^
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_bitxor;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_logand:  // &&
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_logand;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_logor:   // ||
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_logor;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_max:
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_max;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_min:
        {
          result = SgOmpClause::e_omp_task_reduction_identifier_min;
          break;
        }
      case OMPC_TASK_REDUCTION_IDENTIFIER_user:
        {
          result = SgOmpClause::e_omp_task_reduction_user_defined_identifier;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for task_reduction operator conversion:%d\n", identifier);
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_task_reduction_identifier_unspecified);
    return result;
  }

  //! A helper function to convert OpenMPIR ALLOCATE allocator to SgClause ALLOCATE modifier
  static SgOmpClause::omp_allocate_modifier_enum toSgOmpClauseAllocateAllocator(OpenMPAllocateClauseAllocator allocator)
  {
    SgOmpClause::omp_allocate_modifier_enum result;
    switch (allocator)
    {
      case OMPC_ALLOCATE_ALLOCATOR_default:
        {
          result = SgOmpClause::e_omp_allocate_default_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_large_cap:
        {
          result = SgOmpClause::e_omp_allocate_large_cap_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_cons_mem:
        {
          result = SgOmpClause::e_omp_allocate_const_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_high_bw:
        {
          result = SgOmpClause::e_omp_allocate_high_bw_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_low_lat:
        {
          result = SgOmpClause::e_omp_allocate_low_lat_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_cgroup:
        {
          result = SgOmpClause::e_omp_allocate_cgroup_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_pteam:
        {
          result = SgOmpClause::e_omp_allocate_pteam_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_thread:
        {
          result = SgOmpClause::e_omp_allocate_thread_mem_alloc;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_user:
        {
          result = SgOmpClause::e_omp_allocate_user_defined_modifier;
          break;
        }
      case OMPC_ALLOCATE_ALLOCATOR_unspecified:
        {
          result = SgOmpClause::e_omp_allocate_modifier_unknown;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for allocate modifier conversion:%d\n", allocator);
          ROSE_ASSERT(false);
          break;
        }
    }

    return result;
  }

  //A helper function to set SgVarRefExpPtrList  from OmpAttribute's construct-varlist map
  static void setClauseVariableList(SgOmpVariablesClause* target, OmpAttribute* att, omp_construct_enum key)
  {
    ROSE_ASSERT(target&&att);
    // build variable list
    std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(key);
#if 0  
    // Liao 6/10/2010 we relax this assertion to workaround 
    //  shared(num_threads),  a clause keyword is used as a variable 
    //  we skip variable list of shared() for now so shared clause will have empty variable list
#endif  
    ROSE_ASSERT(varlist.size()!=0);
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
    {
//      cout<<"debug setClauseVariableList: " << target <<":"<<(*iter).second->class_name()  <<endl;
      // We now start to use SgExpression* to store variables showing up in a varlist
      if (SgInitializedName* iname = isSgInitializedName((*iter).second))
      {
        //target->get_variables().push_back(iname);
        // Liao 1/27/2010, fix the empty parent pointer of the SgVarRefExp here
        SgVarRefExp * var_ref = buildVarRefExp(iname);
        target->get_variables()->get_expressions().push_back(var_ref);
        var_ref->set_parent(target);
      }
      else if (SgPntrArrRefExp* aref= isSgPntrArrRefExp((*iter).second))
      {
        target->get_variables()->get_expressions().push_back(aref);
        aref->set_parent(target);
      }
      else if (SgVarRefExp* vref = isSgVarRefExp((*iter).second))
      {
        target->get_variables()->get_expressions().push_back(vref);
        vref->set_parent(target);
      }
      else
      {
          cerr<<"error: unhandled type of variable within a list:"<< ((*iter).second)->class_name();
          ROSE_ASSERT(false);
      }
    }
  }

  static SgOmpClause::omp_depobj_modifier_enum toSgOmpClauseDepobjModifierType(OpenMPDepobjUpdateClauseDependeceType type)
  {
    SgOmpClause::omp_depobj_modifier_enum result = SgOmpClause::e_omp_depobj_modifier_unknown;
    switch (type)
    {
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_in:
        {
            result = SgOmpClause::e_omp_depobj_modifier_in;
            break;
        }
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_out:
        {
            result = SgOmpClause::e_omp_depobj_modifier_out;
            break;
        }
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_inout:
        {
            result = SgOmpClause::e_omp_depobj_modifier_inout;
            break;
        }
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_mutexinoutset:
        {
            result = SgOmpClause::e_omp_depobj_modifier_mutexinoutset;
            break;
        }
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_depobj:
        {
            result = SgOmpClause::e_omp_depobj_modifier_depobj;
            break;
        }
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_sink:
        {
            result = SgOmpClause::e_omp_depobj_modifier_sink;
            break;
        }
        case OMPC_DEPOBJ_UPDATE_DEPENDENCE_TYPE_source:
        {
            result = SgOmpClause::e_omp_depobj_modifier_source;
            break;
        }
        default:
        {
            printf("error: unacceptable omp construct enum for dependence type conversion:%d\n", type);
            ROSE_ASSERT(false);
            break;
        }
    }
    return result;
  }

  static SgOmpClause::omp_dependence_type_enum toSgOmpClauseDependenceType(OpenMPDependClauseType type)
  {
    SgOmpClause::omp_dependence_type_enum result = SgOmpClause::e_omp_depend_unspecified;
    switch (type)
    {
      case OMPC_DEPENDENCE_TYPE_in:
        {
          result = SgOmpClause::e_omp_depend_in;
          break;
        }
      case OMPC_DEPENDENCE_TYPE_out:
        {
          result = SgOmpClause::e_omp_depend_out;
          break;
        }
      case OMPC_DEPENDENCE_TYPE_inout:
        {
          result = SgOmpClause::e_omp_depend_inout;
          break;
        }
      case OMPC_DEPENDENCE_TYPE_mutexinoutset:
        {
          result = SgOmpClause::e_omp_depend_mutexinoutset;
          break;
        }
      case OMPC_DEPENDENCE_TYPE_depobj:
        {
          result = SgOmpClause::e_omp_depend_depobj;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for dependence type conversion:%d\n", type);
          ROSE_ASSERT(false);
          break;
        }
    }
    return result;
  }
  
    static SgOmpClause::omp_depend_modifier_enum toSgOmpClauseDependModifier(OpenMPDependClauseModifier modifier)
  {
    SgOmpClause::omp_depend_modifier_enum result = SgOmpClause::e_omp_depend_modifier_unspecified;
    switch (modifier)
    {
      case OMPC_DEPEND_MODIFIER_unspecified:
        {
          result = SgOmpClause::e_omp_depend_modifier_unspecified;
          break;
        }
      case OMPC_DEPEND_MODIFIER_iterator:
        {
          result = SgOmpClause::e_omp_depend_modifier_iterator;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for depend modifier conversion:%d\n", modifier);
          ROSE_ASSERT(false);
          break;
        }
    }
    return result;
  }

  //! Build a map clause with a given operation type from OmpAttribute
  // map may have several variants: tofrom, to, from, and alloc. 
  // the variables for each may have dimension info 
  /*SgOmpMapClause* buildOmpMapClause(OmpAttribute* att, omp_construct_enum map_op)
  {
    ROSE_ASSERT(att !=NULL);
    ROSE_ASSERT (att->isMapVariant(map_op));
    if (!att->hasMapVariant(map_op))
      return NULL;
    SgOmpClause::omp_map_operator_enum  sg_op = toSgOmpClauseMapOperator(map_op); 
    SgExprListExp* explist=buildExprListExp();
    SgOmpMapClause* result = new SgOmpMapClause(explist, sg_op);
    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    explist->set_parent(result);

    // build variable list
    setClauseVariableList(result, att, map_op); 

    //this is somewhat inefficient. 
    // since the attribute has dimension info for all map clauses
    //But we don't want to move the dimension info to directive level 
    result->set_array_dimensions(att->array_dimensions);

   //A translation from OmpSupport::omp_construct_enum to SgOmpClause::omp_map_dist_data_enum is needed here.
   std::map<SgSymbol*, std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> > > attDistMap = att->dist_data_policies;
   std::map<SgSymbol*, std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> > >::iterator iter;

   std::map<SgSymbol*, std::vector<std::pair<SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > convertedDistMap;
   for (iter= attDistMap.begin(); iter!=attDistMap.end(); iter++)
   {
     SgSymbol* s = (*iter).first; 
     std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> > src_vec = (*iter).second; 
     std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> >::iterator iter2;

     std::vector<std::pair<SgOmpClause::omp_map_dist_data_enum, SgExpression*> > converted_vec;
     for (iter2=src_vec.begin(); iter2!=src_vec.end(); iter2 ++ )
     {
       std::pair<OmpSupport::omp_construct_enum, SgExpression*>  src_pair = *iter2; 
       if (src_pair.first == OmpSupport::e_duplicate)
       {
         converted_vec.push_back(make_pair(SgOmpClause::e_omp_map_dist_data_duplicate, src_pair.second) );
       } else 
       if (src_pair.first == OmpSupport::e_cyclic)
       {
         converted_vec.push_back(make_pair(SgOmpClause::e_omp_map_dist_data_cyclic, src_pair.second) );
       } else 
       if (src_pair.first == OmpSupport::e_block)
       {
         converted_vec.push_back(make_pair(SgOmpClause::e_omp_map_dist_data_block, src_pair.second) );
       } else 
       {
         cerr<<"error. buildOmpMapClause() :unrecognized source dist data policy enum:"<<src_pair.first <<endl;
         ROSE_ASSERT (false);
      } // end for iter2
     } // end for iter
     convertedDistMap[s]= converted_vec;
   }
    result->set_dist_data_policies(convertedDistMap);
    return result;
  }*/

  //Build one of the clauses with a variable list
  SgOmpVariablesClause * buildOmpVariableClause(OmpAttribute* att, omp_construct_enum clause_type)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(clause_type))
      return NULL;
    SgOmpVariablesClause* result = NULL;  
    SgExprListExp * explist = buildExprListExp(); 
    ROSE_ASSERT(explist != NULL);
    switch (clause_type) 
    {
     case e_uniform:
        {
          result = new SgOmpUniformClause(explist);
          break;
        }
      default:
        {
          cerr<<"error: buildOmpVariableClause() Unacceptable clause type:"
            <<OmpSupport::toString(clause_type)<<endl;
          ROSE_ASSERT(false) ;  
        }
    } //end switch

    ROSE_ASSERT(result != NULL);
    explist->set_parent(result);
    //build varlist
    setClauseVariableList(result, att, clause_type);
    return result;
  }

  // Build a single SgOmpClause from OmpAttribute for type c_clause_type, excluding reduction clauses
  // Later on this function will be modified to cover all the clauses other than regular expression and variable list clauses.
  SgOmpClause* buildOmpNonReductionClause(OmpAttribute* att, omp_construct_enum c_clause_type)
  {
    SgOmpClause* result = NULL;
    ROSE_ASSERT(att != NULL);
    ROSE_ASSERT(isClause(c_clause_type));
    if (!att->hasClause(c_clause_type))
      return NULL;
    switch (c_clause_type) 
    {
      case e_inbranch:
        {
          result = buildOmpInbranchClause(att); 
          break;
        }
       case e_notinbranch:
        {
          result = buildOmpNotinbranchClause(att); 
          break;
        }
      case e_begin:
        {
          result = buildOmpBeginClause(att);
          break;
        }
      case e_end:
        {
          result = buildOmpEndClause(att);
          break;
        }
      default:
        {
          printf("Warning: buildOmpNoReductionClause(): unhandled clause type: %s\n", OmpSupport::toString(c_clause_type).c_str());
          ROSE_ASSERT(false);
          break;
        }

    }
    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  //! Get the affected structured block from an OmpAttribute
  SgStatement* getOpenMPBlockFromOmpAttribute (OmpAttribute* att)
  {
    SgStatement* result = NULL;
    ROSE_ASSERT(att != NULL);
    omp_construct_enum c_clause_type = att->getOmpDirectiveType();

    // Some directives have no followed statements/blocks 
    if (!isDirectiveWithBody(c_clause_type))
      return NULL;

    SgNode* snode = att-> getNode ();
    ROSE_ASSERT(snode != NULL); //? not sure for Fortran
    // Liao 10/19/2010 We convert Fortran comments into SgPragmaDeclarations
    // So we can reuse the same code to generate OpenMP AST from pragmas
#if 0     
    SgFile * file = getEnclosingFileNode (snode);
    if (file->get_Fortran_only()||file->get_F77_only()||file->get_F90_only()||
        file->get_F95_only() || file->get_F2003_only())
    { //Fortran check setNode()
      //printf("buildOmpParallelStatement() Fortran is not handled yet\n");
      //ROSE_ASSERT(false);
    }
    else // C/C++ must be pragma declaration statement
    {
      SgPragmaDeclaration* pragmadecl = att->getPragmaDeclaration();
      result = getNextStatement(pragmadecl);
    }
#endif
    SgPragmaDeclaration* pragmadecl = att->getPragmaDeclaration();
    result = getNextStatement(pragmadecl);
    // Not all pragma decl has a structured body. We check those which do have one
    // TODO: more types to be checked
    if (c_clause_type == e_task || 
        c_clause_type == e_parallel||
        c_clause_type == e_for||
        c_clause_type == e_do||
        c_clause_type == e_workshare||
        c_clause_type == e_sections||
        c_clause_type == e_section||
        c_clause_type == e_single||
        c_clause_type == e_master||
        c_clause_type == e_critical||
        c_clause_type == e_parallel_for||
        c_clause_type == e_parallel_for_simd||
        c_clause_type == e_parallel_do||
        c_clause_type == e_simd||
        c_clause_type == e_atomic
       )
    {
      ROSE_ASSERT(result!=NULL);
    }
    return result;
  }

  // a bit hack since declare simd is an outlier statement with clauses. 
  /*
     clause:
      simdlen(length)
      linear(linear-list[ : linear-step])
      aligned(argument-list[ : alignment])
      uniform(argument-list)
      inbranch
      notinbranch 
   * */
  static void appendOmpClauses(SgOmpDeclareSimdStatement* target, OmpAttribute* att)
  {
    ROSE_ASSERT(target && att);
    // must copy those clauses here, since they will be deallocated later on
    vector<omp_construct_enum> clause_vector = att->getClauses();
    std::vector<omp_construct_enum>::iterator citer;
    for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++)
    {
      omp_construct_enum c_clause = *citer;
      if (!isClause(c_clause))
      {
        //      printf ("Found a construct which is not a clause:%s\n within attr:%p\n", OmpSupport::toString(c_clause).c_str(), att);
        ROSE_ASSERT(isClause(c_clause));
        continue;
      }

      SgOmpClause* result = NULL; 
      //------------------ 

      if (!att->hasClause(c_clause))
        continue; 
      switch (c_clause) 
      {
        case e_inbranch:
          {
            result = buildOmpInbranchClause(att); 
            break;
          }
        case e_notinbranch:
          {
            result = buildOmpNotinbranchClause(att); 
            break;
          }
        case e_uniform: 
          {
            result = buildOmpVariableClause(att, c_clause);
            break;
          }
        default:
          {
            printf("Warning: buildOmpNoReductionClause(): unhandled clause type: %s\n", OmpSupport::toString(c_clause).c_str());
            ROSE_ASSERT(false);
            break;
          }
      }
      ROSE_ASSERT(result != NULL);
      setOneSourcePositionForTransformation(result);

      //cout<<"push a clause "<< result->class_name() <<endl;
      target->get_clauses().push_back(result);
      result->set_parent(target); // is This right?
    }
  }


  //add clauses to target based on OmpAttribute
  static void appendOmpClauses(SgOmpClauseBodyStatement* target, OmpAttribute* att)
  {
    ROSE_ASSERT(target && att);
    // for Omp statements with clauses
    // must copy those clauses here, since they will be deallocated later on
    vector<omp_construct_enum> clause_vector = att->getClauses();
    std::vector<omp_construct_enum>::iterator citer;
    for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++) {
        omp_construct_enum c_clause = *citer;
        if (!isClause(c_clause)) {
            ROSE_ASSERT(isClause(c_clause));
            continue;
        }
        // later on if loop should be rewritten to switch case for efficiency.
        // special handling for reduction
        switch (c_clause) {
            /*case e_map: {
                std::vector<omp_construct_enum> rops  = att->getMapVariants();
                ROSE_ASSERT(rops.size()!=0);
                std::vector<omp_construct_enum>::iterator iter;
                for (iter=rops.begin(); iter!=rops.end();iter++) {
                    omp_construct_enum rop = *iter;
                    SgOmpClause* sgclause = buildOmpMapClause(att, rop);
                    target->get_clauses().push_back(sgclause);
                    sgclause->set_parent(target);
                };
                break;
            }*/
            default: {
                SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
                target->get_clauses().push_back(sgclause);
                sgclause->set_parent(target); // is This right?
            }
        }
    }
  }

  // Directive statement builders
  //----------------------------------------------------------
  //! Build a SgOmpBodyStatement
  // handle body and optional clauses for it
  SgOmpBodyStatement * buildOmpBodyStatement(OmpAttribute* att)
  {
    SgStatement* body = getOpenMPBlockFromOmpAttribute(att);
    //Must remove the body from its previous parent first before attaching it 
    //to the new parent statement.
    // We want to keep its preprocessing information during this relocation
    // so we don't auto keep preprocessing information in its original places.
    removeStatement(body,false);

    if (body==NULL)
    {
      cerr<<"error: buildOmpBodyStatement() found empty body for "<<att->toOpenMPString()<<endl;
      ROSE_ASSERT(body != NULL);
    }
    SgOmpBodyStatement* result = NULL;
    switch (att->getOmpDirectiveType())
    {
      case e_ordered_directive:
        result = new SgOmpOrderedStatement(NULL, body); 
        break;
 
       //Fortran  
      case e_do:
        result = new SgOmpDoStatement(NULL, body); 
        break;
      case e_workshare:
        result = new SgOmpWorkshareStatement(NULL, body); 
        break;
      default:
        {
          cerr<<"error: unacceptable omp construct for buildOmpBodyStatement():"<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
          ROSE_ASSERT(false);
        }
    }
    ROSE_ASSERT(result != NULL);
    //setOneSourcePositionForTransformation(result);
    // copyStartFileInfo (att->getNode(), result); // No need here since its caller will set file info again
//    body->get_startOfConstruct()->display();
//    body->get_endOfConstruct()->display();
    //set the current parent
    body->set_parent(result);
    // add clauses for those SgOmpClauseBodyStatement
    if (isSgOmpClauseBodyStatement(result))
      appendOmpClauses(isSgOmpClauseBodyStatement(result), att);
      
   // Liao 1/9/2013, ensure the body is a basic block for some OpenMP constructs
   if (isSgOmpSingleStatement(result)) 
    ensureBasicBlockAsBodyOfOmpBodyStmt (result); 
//    result->get_file_info()->display("debug after building ..");
    return result;
  }

  SgOmpFlushStatement* buildOmpFlushStatement(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    SgOmpFlushStatement* result = new SgOmpFlushStatement();
    ROSE_ASSERT(result !=NULL);
    setOneSourcePositionForTransformation(result);
    // build variable list
    std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(e_flush);
    // ROSE_ASSERT(varlist.size()!=0); // can have empty variable list
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
    {
      SgInitializedName* iname = isSgInitializedName((*iter).second);
      ROSE_ASSERT(iname !=NULL);
      SgVarRefExp* varref = buildVarRefExp(iname);
      result->get_variables().push_back(varref);
      varref->set_parent(result);
    }
    return result;
  }

  SgOmpDeclareSimdStatement* buildOmpDeclareSimdStatement(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    SgOmpDeclareSimdStatement* result = new SgOmpDeclareSimdStatement();
    result->set_firstNondefiningDeclaration(result);
    ROSE_ASSERT(result !=NULL);
    setOneSourcePositionForTransformation(result);

    appendOmpClauses(isSgOmpDeclareSimdStatement(result), att);
    return result;
  }

  //! For C/C++ replace OpenMP pragma declaration with an SgOmpxxStatement
  void replaceOmpPragmaWithOmpStatement(SgPragmaDeclaration* pdecl, SgStatement* ompstmt)
  {
    ROSE_ASSERT(pdecl != NULL);
    ROSE_ASSERT(ompstmt!= NULL);

    SgScopeStatement* scope = pdecl ->get_scope();
    ROSE_ASSERT(scope !=NULL);
    // replace the pragma
    moveUpPreprocessingInfo(ompstmt, pdecl); // keep #ifdef etc attached to the pragma
    replaceStatement(pdecl, ompstmt);
  }

  //! Convert omp_pragma_list to SgOmpxxx nodes
  void convert_OpenMP_pragma_to_AST (SgSourceFile *sageFilePtr)
  {
    list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
    ROSE_ASSERT (sageFilePtr != NULL);
    int OpenMPIR_index = OpenMPIR_list.size()-1;
    for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter ++)
    {
      // Liao, 11/18/2009
      // It is possible that several source files showing up in a single compilation line
      // We have to check if the pragma declaration's file information matches the current file being processed
      // Otherwise we will process the same pragma declaration multiple times!!
      SgPragmaDeclaration* decl = *iter; 
      // Liao, 2/8/2010
      // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
      // e.g if ()
      //      #pragma
      //        do_sth()
      //  will be changed to
      //     if ()
      //     {
      //       #pragma
      //        do_sth()
      //     }
      // So we process a pragma if it is either within the same file or marked as transformation
      if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(decl->get_file_info()->isTransformation()))
        continue;

      // OmpAttibute* could be empty due to ompparser.
      if (getOmpAttribute(decl) == NULL) {
          convertDirective(OpenMPIR_list[OpenMPIR_index]);
          OpenMPIR_index--;
          continue;
      };

       // Liao 10/19/2010
       // We now support OpenMP AST construction for both C/C++ and Fortran
       // But we allow Fortran End directives to exist after -rose:openmp:ast_only
       // Otherwise the code unparsed will be illegal Fortran code (No {} blocks in Fortran)
       if (isFortranEndDirective(getOmpAttribute(decl)->getOmpDirectiveType()))
          continue; 
      ROSE_ASSERT (decl->get_scope() !=NULL);    
      ROSE_ASSERT (decl->get_parent() !=NULL);    
      //cout<<"debug: convert_OpenMP_pragma_to_AST() handling pragma at "<<decl<<endl;  
      //ROSE_ASSERT (decl->get_file_info()->get_filename() != string("transformation"));
      OmpAttributeList* oattlist= getOmpAttributeList(decl);

      ROSE_ASSERT (oattlist != NULL) ;
      vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
      ROSE_ASSERT (ompattlist.size() != 0) ;
      ROSE_ASSERT (ompattlist.size() == 1) ; // when do we have multiple directives associated with one pragma?

      // Liao 12/21/2015 special handling to support target begin and target end aimed for MPI code generation
      // In this case, we already use postParsingProcessing () to wrap the statements in between into a basic block after "target begin" 
      // The "target end" attribute should be ignored.
      // Skip "target end" here. 
      OmpAttribute* oa = ompattlist[0];
      ROSE_ASSERT (oa!=NULL);
      if (oa->hasClause(e_end))
      {
      // This assertion does not hold. The pragma is removed. But it is still accessible from omp_pragma_list  
      //  cerr<<"Error. unexpected target end directive is encountered in convert_OpenMP_pragma_to_AST(). It should have been removed by postParsingProcessing()."<<endl;
       // ROSE_ASSERT (false);
        //removeStatement(decl);
         continue;
      }

      vector <OmpAttribute* >::iterator i = ompattlist.begin();
      for (; i!=ompattlist.end(); i++)
      {
        // reset complex clause index for each OmpAttribute object.
        OmpAttribute* oa = *i;
        omp_construct_enum omp_type = oa->getOmpDirectiveType();
        ROSE_ASSERT(isDirective(omp_type));
        SgStatement* omp_stmt = NULL;
        switch (omp_type)
        {
            // with variable list
          case e_flush:
            {
              omp_stmt = buildOmpFlushStatement(oa);
              break;
            }
          case e_declare_simd:
            {
              omp_stmt = buildOmpDeclareSimdStatement(oa);
              break;
            }
            // with a structured block/statement followed
          case e_ordered_directive:
            //fortran
          case e_do:
          case e_workshare:
            {
              omp_stmt = buildOmpBodyStatement(oa);
              break;
            }
          case e_parallel_sections:
          case e_parallel_workshare://fortran
          case e_parallel_do:
            {
              //omp_stmt = buildOmpParallelStatementFromCombinedDirectives(oa);
              break;
            }
          default:
            { 
               cerr<<"Error: convert_OpenMP_pragma_to_AST(): unhandled OpenMP directive type:"<<OmpSupport::toString(omp_type)<<endl;
                assert (false);
               break;
            }
        }
        setOneSourcePositionForTransformation(omp_stmt);
        copyStartFileInfo (oa->getNode(), omp_stmt, oa);
        copyEndFileInfo (oa->getNode(), omp_stmt, oa);
        //ROSE_ASSERT (omp_stmt->get_file_info()->isTransformation() != true);
        replaceOmpPragmaWithOmpStatement(decl, omp_stmt);

      } // end for (OmpAttribute)
    }// end for (omp_pragma_list)
  }

  //! A helper function to ensure a sequence statements either has only one statement
  //  or all are put under a single basic block.
  //  begin_decl is the begin directive which is immediately in front of the list of statements
  //  Return the single statement or the basic block.
  //  This function is used to wrap all statement between begin and end Fortran directives into a block,
  //  if necessary(more than one statement)
   static SgStatement * ensureSingleStmtOrBasicBlock (SgPragmaDeclaration* begin_decl, const std::vector <SgStatement*>& stmt_vec)
   {
     ROSE_ASSERT (begin_decl != NULL);
     SgStatement * result = NULL; 
     ROSE_ASSERT (stmt_vec.size() > 0);
     if (stmt_vec.size() ==1)
     {
       result = stmt_vec[0];
       ROSE_ASSERT (getNextStatement(begin_decl) == result);
     }
     else
     {  
       result = buildBasicBlock();
       // Have to remove them from their original scope first. 
       // Otherwise they will show up twice in the unparsed code: original place and under the new block
       // I tried to merge this into appendStatement() but it broke other transformations I don't want debug
       for (std::vector <SgStatement*>::const_iterator iter = stmt_vec.begin(); iter != stmt_vec.end(); iter++)
         removeStatement(*iter);
       appendStatementList (stmt_vec, isSgScopeStatement(result));
       insertStatementAfter (begin_decl, result, false);
     }
     return result; 
   }

  //! Merge clauses from end directives to the corresponding begin directives
  // dowait clause:  end do, end sections, end single, end workshare
  // copyprivate clause: end single
 void mergeEndClausesToBeginDirective (SgPragmaDeclaration* begin_decl, SgPragmaDeclaration* end_decl)
 {
   ROSE_ASSERT (begin_decl!=NULL);
   ROSE_ASSERT (end_decl!=NULL);

   // Make sure they match
   omp_construct_enum begin_type, end_type;
   begin_type = getOmpConstructEnum (begin_decl);
   end_type = getOmpConstructEnum (end_decl);
   ROSE_ASSERT (begin_type == getBeginOmpConstructEnum(end_type));

#if 0
   // Make sure they are at the same level ??
   // Fortran do loop may have wrong file info, which cause comments to be attached to another scope
   // Consequently, the end pragma will be in a higher/ different scope
   // A workaround for bug 495: https://outreach.scidac.gov/tracker/?func=detail&atid=185&aid=495&group_id=24
   if (SageInterface::is_Fortran_language() )
   {
     if (begin_decl->get_parent() != end_decl->get_parent())
     {
       ROSE_ASSERT (isAncestor (end_decl->get_parent(), begin_decl->get_parent()));
     }
   }
   else  
#endif     
    ROSE_ASSERT (begin_decl->get_parent() == end_decl->get_parent()); 

   // merge end directive's clause to the begin directive.
   OmpAttribute* begin_att = getOmpAttribute (begin_decl); 
   OmpAttribute* end_att = getOmpAttribute (end_decl); 

   // Merge possible nowait clause
   switch (end_type)
   {
     case e_end_do:
     case e_end_sections:
     case e_end_single:
     case e_end_workshare:
       {
         if (end_att->hasClause(e_nowait))
         {
           begin_att->addComplexClause(e_nowait);
         }
         break;
       }
     default:
       break; // there should be no clause for other cases
   }
   // Merge possible copyrpivate (list) from end single
   if ((end_type == e_end_single) && end_att ->hasClause(e_copyprivate)) 
   {
     ComplexClause* current_clause = begin_att->addComplexClause(e_copyprivate);
     std::vector<std::pair<std::string,SgNode* > > varList = end_att->getComplexClauses(e_copyprivate)->front().variable_list;
     std::vector<std::pair<std::string,SgNode* > >::iterator iter;
     for (iter = varList.begin(); iter != varList.end(); iter++)
     {
       std::pair<std::string,SgNode* > element = *iter;
       SgInitializedName* i_name = isSgInitializedName(element.second);
       ROSE_ASSERT (i_name != NULL);
       begin_att->addComplexClauseVariable(current_clause, element.first, i_name);
     }
   }  
 }
  //! This function will Find a (optional) end pragma for an input pragma (decl)
  //  and merge clauses from the end pragma to the beginning pragma
  //  statements in between will be put into a basic block if there are more than one statements
  void merge_Matching_Fortran_Pragma_pairs(SgPragmaDeclaration* decl)
  {
    SgPragmaDeclaration* end_decl = NULL; 
    SgStatement* next_stmt = getNextStatement(decl);
    OpenMPDirectiveKind begin_directive_kind = fortran_paired_pragma_list[decl]->getKind();

    std::vector<SgStatement*> affected_stmts; // statements which are inside the begin .. end pair

    // Find possible end directives attached to a pragma declaration
    while (next_stmt!= NULL)
    {
      end_decl = isSgPragmaDeclaration (next_stmt);
      if ((end_decl) && (((OpenMPEndDirective*)fortran_paired_pragma_list[end_decl])->getPairedDirective()) == fortran_paired_pragma_list[decl])
        break;
      else
        end_decl = NULL; // MUST reset to NULL if not a match
      affected_stmts.push_back(next_stmt);
      next_stmt = getNextStatement (next_stmt);
#if 0
      // Liao 1/21/2011
      // A workaround of wrong file info for Do loop body
      // See bug 495 https://outreach.scidac.gov/tracker/?func=detail&atid=185&aid=495&group_id=24
      // Comments will not be attached before ENDDO, but some parent located node instead.
      // SageInterface::getNextStatement() will not climb out current scope and find a matching end directive attached to a parent node.
      //
      // For example 
      //        do i = 1, 10
      //     !$omp task 
      //        call process(item(i))
      //     !$omp end task
      //          enddo
      // The !$omp end task comments will not be attached before ENDDO , but inside SgBasicBlock, which is an ancestor node  
     if (SageInterface::is_Fortran_language() )
     {
      // try to climb up one statement level, until reaching the function body
       SgStatement* parent_stmt  = getEnclosingStatement(prev_stmt->get_parent());
       // getNextStatement() cannot take SgFortranDo's body as input (the body is not a child of its scope's declaration list)
       // So we climb up to the parent do loop
       if (isSgFortranDo(parent_stmt->get_parent()))  
         parent_stmt = isSgFortranDo(parent_stmt->get_parent());
       else if (isSgWhileStmt(parent_stmt->get_parent()))  
         parent_stmt = isSgWhileStmt(parent_stmt->get_parent());

       if (parent_stmt != func_body) 
         next_stmt = getNextStatement (parent_stmt);
     }
#endif     
    }  // end while

    // mandatory end directives for most begin directives, except for two cases:
    // !$omp end do
    // !$omp end parallel do
    if (end_decl == NULL) 
    {
      if ((begin_directive_kind != OMPD_parallel) && (begin_directive_kind != OMPD_do))
      {
        cerr<<"merge_Matching_Fortran_Pragma_pairs(): cannot find required end directive for: "<< endl;
        cerr<<decl->get_pragma()->get_pragma()<<endl;
        ROSE_ASSERT (false);
      }
      else 
        return; // There is nothing further to do if the optional end directives do not exist
    } // end if sanity check

    // at this point, we have found a matching end directive/pragma
    ROSE_ASSERT (end_decl);
    ensureSingleStmtOrBasicBlock(decl, affected_stmts);
    //mergeEndClausesToBeginDirective (decl,end_decl);

    // SgBasicBlock is not unparsed in Fortran 
    //
    // To ensure the unparsed Fortran code is correct for debugging -rose:openmp:ast_only
    //  after converting Fortran comments to Pragmas. 
    // x.  We should not tweak the original text for the pragmas. 
    // x.  We should not remove the end pragma declaration since SgBasicBlock is not unparsed.
    // In the end , the pragmas don't matter too much, the OmpAttributes attached to them 
    // are used to guide translations. 
     removeStatement(end_decl);
    // we should save those useless end pragmas to a list
    // and remove them as one of the first steps in OpenMP lowering for Fortran
    // omp_end_pragma_list.push_back(end_decl); 
  } // end merge_Matching_Fortran_Pragma_pairs()
  
static bool isFortranPairedDirective(OpenMPDirective* node) {
    bool result = false;
    switch (node->getKind()) {
        case OMPD_barrier:
        case OMPD_end:
        case OMPD_flush:
        case OMPD_section: {
            break;
        }
        default: {
            result = true;
        }
    }
    return result;
}

  //! This function will 
  //   x. Find matching OpenMP directive pairs
  //      an inside out order is used to handle nested regions
  //   x. Put statements in between into a basic block
  //   x. Merge clauses from the ending directive to the beginning directives
  //  The result is an Fortran OpenMP AST with C/C++ pragmas
  //  so we can simply reuse convert_OpenMP_pragma_to_AST() to generate 
  //  OpenMP AST nodes for Fortran programs
  void convert_Fortran_Pragma_Pairs (SgSourceFile *sageFilePtr)
  {
    ROSE_ASSERT (sageFilePtr != NULL);
    list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
    for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter++)
    {
      // It is possible that several source files showing up in a single compilation line
      // We have to check if the pragma declaration's file information matches the current file being processed
      // Otherwise we will process the same pragma declaration multiple times!!
      SgPragmaDeclaration* decl = *iter;
      // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
      // e.g if ()
      //      #pragma
      //        do_sth()
      //  will be changed to
      //     if ()
      //     {
      //       #pragma
      //        do_sth()
      //     }
      // So we process a pragma if it is either within the same file or marked as transformation
      if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(decl->get_file_info()->isTransformation()))
        continue;
      if (isFortranPairedDirective(fortran_paired_pragma_list[decl])) {
         merge_Matching_Fortran_Pragma_pairs(decl);
      }
    } // end for omp_pragma_list

  } // end convert_Fortran_Pragma_Pairs()

  //! Convert OpenMP Fortran comments to pragmas
  //  main purpose is to 
  //     x. Generate pragmas from OmpAttributes and insert them into the right places
  //        since the floating comments are very difficult to work with
  //        we move them to the fake pragmas to ease later translations. 
  //        The backend has been extended to unparse the pragma in order to debug this step.
  //     x. Enclose affected Fortran statement into a basic block
  //     x. Merge clauses from END directives to the begin directive
  // This will temporarily introduce C/C++-like AST with pragmas attaching OmpAttributes.
  // This should be fine since we have SgBasicBlock in Fortran AST also.
  //
  // The benefit is that pragma-to-AST conversion written for C/C++ can 
  // be reused for Fortran after this pass.
  // Liao 10/18/2010
  void convert_Fortran_OMP_Comments_to_Pragmas (SgSourceFile *sageFilePtr)
  {
    // We reuse the pragma list for C/C++ here
    //ROSE_ASSERT  (omp_pragma_list.size() ==0);
    ROSE_ASSERT (sageFilePtr != NULL);
    // step 1: Each OmpAttribute will have a dedicated SgPragmaDeclaration for it
    std::vector<std::tuple<SgLocatedNode*, PreprocessingInfo*, OpenMPDirective*>>::iterator iter;
    for (iter = fortran_omp_pragma_list.begin(); iter != fortran_omp_pragma_list.end(); iter++)
    {
      SgLocatedNode* loc_node = std::get<0>(*iter);
      SgStatement* stmt = isSgStatement(loc_node);
      OpenMPDirective* ompparser_directive_ir = std::get<2>(*iter);
      // TODO verify this assertion is true for Fortran OpenMP comments
      ROSE_ASSERT (stmt != NULL);
      //cout<<"debug at ompAstConstruction.cpp:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
      ROSE_ASSERT (stmt->getAttachedPreprocessingInfo ()->size() != 0);
      // So we process the directive if it's anchor node is either within the same file or marked as transformation
      if (stmt->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(stmt->get_file_info()->isTransformation()))
        continue;
      SgScopeStatement * scope = stmt->get_scope();
      ROSE_ASSERT (scope != NULL);
      // the pragma will have string to ease debugging
      std::string pragma_string = ompparser_directive_ir->generatePragmaString("omp ", "", "");
      SgPragmaDeclaration* p_decl = buildPragmaDeclaration(pragma_string, scope);
      //preserve the original source file info ,TODO complex cases , use real preprocessing info's line information !!
      copyStartFileInfo (loc_node, p_decl, NULL);

      if (ompparser_directive_ir->getKind() != OMPD_end) {
          OpenMPIR_list.push_back(std::make_pair(p_decl, ompparser_directive_ir));
          omp_pragma_list.push_back(p_decl);
      }
      fortran_paired_pragma_list[p_decl] = ompparser_directive_ir;

      PreprocessingInfo* info = std::get<1>(*iter);
      ROSE_ASSERT (info != NULL);
      // We still keep the peprocessingInfo. its line number will be used later to set file info object
      AttachedPreprocessingInfoType *comments = stmt ->getAttachedPreprocessingInfo ();
      ROSE_ASSERT (comments != NULL);
      ROSE_ASSERT (comments->size() !=0);
      AttachedPreprocessingInfoType::iterator m_pos = find (comments->begin(), comments->end(), info);
      if (m_pos == comments->end())
      {
        cerr<<"Cannot find a Fortran comment from a node: "<<endl;
        cerr<<"The comment is "<<info->getString()<<endl;
        cerr<<"The AST Node is "<<stmt->class_name()<<endl;
        stmt->get_file_info()->display("debug here");
        AttachedPreprocessingInfoType::iterator i;
        for (i = comments->begin(); i!= comments->end(); i++)
        {
          cerr<<(*i)->getString()<<endl;
        }
        //cerr<<"The AST Node is at line:"<<stmt->get_file_info().get_line()<<endl;
        ROSE_ASSERT (m_pos != comments->end());
      }
      comments->erase (m_pos);

      // two cases for where to insert the pragma, depending on where the preprocessing info is attached to stmt
      //  1. PreprocessingInfo::before
      //     insert the pragma right before the original Fortran statement
      //  2. PreprocessingInfo::inside
      //      insert it as the last statement within stmt
      PreprocessingInfo::RelativePositionType position = info->getRelativePosition ();      
      if (position == PreprocessingInfo::before)
      { 
        // Don't automatically move comments here!
        if (isSgBasicBlock(stmt) && isSgFortranDo (stmt->get_parent()))
        {// special handling for the body of SgFortranDo.  The comments will be attached before the body
         // But we cannot insert the pragma before the body. So we prepend it into the body instead
          prependStatement(p_decl, isSgBasicBlock(stmt));
        }
        else
          insertStatementBefore (stmt, p_decl, false);
      }
      else if (position == PreprocessingInfo::inside)
      {
        SgScopeStatement* scope = isSgScopeStatement(stmt);
        ROSE_ASSERT (scope != NULL);
        appendStatement(p_decl, scope);
      }
      else if (position == PreprocessingInfo::after)
      {
        insertStatementAfter(stmt, p_decl, false);
      }
      else
      {
        cerr<<"ompAstConstruction.cpp , illegal PreprocessingInfo::RelativePositionType:"<<position<<endl;
        ROSE_ASSERT (false);
      }
      //cout<<"debug at after appendStmt:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
    } // end for omp_comment_list

    convert_Fortran_Pragma_Pairs(sageFilePtr);
  } // end convert_Fortran_OMP_Comments_to_Pragmas ()


  void build_OpenMP_AST(SgSourceFile *sageFilePtr)
  {
    // build AST for OpenMP directives and clauses 
    // by converting OmpAttributeList to SgOmpxxx Nodes 
    if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
        sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
    {
      convert_Fortran_OMP_Comments_to_Pragmas (sageFilePtr);
      // end if (fortran)
    }
    else
    {
      // for  C/C++ pragma's OmpAttributeList --> SgOmpxxx nodes
      if (SgProject::get_verbose() > 1)
      {
        printf ("Calling convert_OpenMP_pragma_to_AST() \n");
      }
    }
    // We can turn this off to debug the convert_Fortran_OMP_Comments_to_Pragmas()
    convert_OpenMP_pragma_to_AST( sageFilePtr);
  }

  // Liao 12/21/2015 special handling to support target begin and target end aimed for MPI code generation
  // In this case, we already use postParsingProcessing () to wrap the statements in between into a basic block after "target begin" 
  // The "target end" attribute should be ignored.
  // Skip "target end" here. 
  void postParsingProcessing (SgSourceFile *sageFilePtr)
  {
    // This experimental support should only happen to C/C++ code for now
    if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
        sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
    {
      return; 
    }

    list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
    ROSE_ASSERT (sageFilePtr != NULL);
   for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter ++)
    {
      // Liao, 11/18/2009
      // It is possible that several source files showing up in a single compilation line
      // We have to check if the pragma declaration's file information matches the current file being processed
      // Otherwise we will process the same pragma declaration multiple times!!
      SgPragmaDeclaration* decl = *iter;
      // Liao, 2/8/2010
      // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
      // e.g if ()
      //      #pragma
      //        do_sth()
      //  will be changed to
      //     if ()
      //     {
      //       #pragma
      //        do_sth()
      //     }
      // So we process a pragma if it is either within the same file or marked as transformation
      if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(decl->get_file_info()->isTransformation()))
        continue;
       // Liao 10/19/2010
       // We now support OpenMP AST construction for both C/C++ and Fortran
       // But we allow Fortran End directives to exist after -rose:openmp:ast_only
       // Otherwise the code unparsed will be illegal Fortran code (No {} blocks in Fortran)
      // if (isFortranEndDirective(getOmpAttribute(decl)->getOmpDirectiveType()))
       //    continue;
      ROSE_ASSERT (decl->get_scope() !=NULL);
      ROSE_ASSERT (decl->get_parent() !=NULL);
      //cout<<"debug: convert_OpenMP_pragma_to_AST() handling pragma at "<<decl<<endl;  
      //ROSE_ASSERT (decl->get_file_info()->get_filename() != string("transformation"));
      OmpAttributeList* oattlist= getOmpAttributeList(decl);                                                                               
      // oattlist could be empty due to ompparser
      // need a way to determine the actual cause.
      if (oattlist == NULL) {
          continue;
      };
      ROSE_ASSERT (oattlist != NULL) ;                                                                                                     
      vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;                                                                         
      ROSE_ASSERT (ompattlist.size() != 0) ;                                                                                               
      ROSE_ASSERT (ompattlist.size() == 1) ; // when do we have multiple directives associated with one pragma?                            
                                                                                                                                           
      // Liao 12/21/2015 special handling to support target begin and target end aimed for MPI code generation                             
      // In this case, we already use postParsingProcessing () to wrap the statements in between into a basic block after "target begin"   
      // The "target end" attribute should be ignored.                                                                                     
      // Skip "target end" here.                                                                                                           
      // find omp target begin
      OmpAttribute* oa = getOmpAttribute (decl);
      ROSE_ASSERT (oa != NULL);
      omp_construct_enum omp_type = oa->getOmpDirectiveType();
      //   The first attribute should always be the directive type
      ROSE_ASSERT(isDirective(omp_type));
      
      if ( omp_type == e_target && oa->hasClause(e_begin)) 
      { 
        // find the matching end decl with "target end" attribute
        SgPragmaDeclaration* end_decl = NULL; 
        SgStatement* next_stmt = getNextStatement(decl);
        std::vector<SgStatement*> affected_stmts; // statements which are inside the begin .. end pair
        while (next_stmt!= NULL)
        {
          end_decl = isSgPragmaDeclaration (next_stmt);
          if (end_decl)  // candidate pragma declaration
           if (getOmpConstructEnum(end_decl) == e_target) // It is target directive
           {
             OmpAttribute* oa2 = getOmpAttribute (end_decl);
             ROSE_ASSERT (oa2 != NULL);
             if (oa2->hasClause (e_end))
                break; // found  the matching target end, break out the while loop
           }

          // No match found yet? store the current stmt into the affected stmt list
          end_decl = NULL; // MUST reset to NULL if not a match
          affected_stmts.push_back(next_stmt);
          //   prev_stmt = next_stmt; // save previous statement
          next_stmt = getNextStatement (next_stmt);
        } // end while  

        if (end_decl == NULL) 
        {
          cerr<<"postParsingProcessing(): cannot find required end directive for: "<< endl;
          cerr<<decl->get_pragma()->get_pragma()<<endl;
          ROSE_ASSERT (false);
        } // end if sanity check

        //at this point, we have found a matching end directive/pragma
        ROSE_ASSERT (end_decl);
        ensureSingleStmtOrBasicBlock(decl, affected_stmts);
        removeStatement(end_decl);

      } // end if "target begin"                          
    } // end for 
  } // end postParsingProcessing()

  // Liao, 5/31/2009 an entry point for OpenMP related processing
  // including parsing, AST construction, and later on translation
  void processOpenMP(SgSourceFile *sageFilePtr)
  {
    // DQ (4/4/2010): This function processes both C/C++ and Fortran code.
    // As a result of the Fortran processing some OMP pragmas will cause
    // transformation (e.g. declaration of private variables will add variables
    // to the local scope).  So this function has side-effects for all languages.

    if (SgProject::get_verbose() > 1)
    {
      printf ("Processing OpenMP directives \n");
    }

    ROSE_ASSERT(sageFilePtr != NULL);
    if (sageFilePtr->get_openmp() == false)
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp() = %s \n",sageFilePtr->get_openmp() ? "true" : "false");
      }
      return;
    }

    // parse OpenMP directives and attach OmpAttributeList to relevant SgNode
    attachOmpAttributeInfo(sageFilePtr);

    // Additional processing of the AST after parsing
    postParsingProcessing (sageFilePtr);

    // stop here if only OpenMP parsing is requested
    if (sageFilePtr->get_openmp_parse_only())
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp_parse_only() = %s \n",sageFilePtr->get_openmp_parse_only() ? "true" : "false");
      }
      return;
    }

    // Build OpenMP AST nodes based on parsing results
    build_OpenMP_AST(sageFilePtr);

    // stop here if only OpenMP AST construction is requested
    if (sageFilePtr->get_openmp_ast_only())
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to analyze/lower OpenMP sageFilePtr->get_openmp_ast_only() = %s \n",sageFilePtr->get_openmp_ast_only() ? "true" : "false");
      }
      return;
    }

    // Analyze OpenMP AST
    analyze_omp(sageFilePtr);

    // stop here if only OpenMP AST analyzing is requested
    if (sageFilePtr->get_openmp_analyzing())
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp_analyzing() = %s \n",sageFilePtr->get_openmp_analyzing() ? "true" : "false");
      }
      return;
    }

    lower_omp(sageFilePtr);
  }

} // end of the namespace

SgStatement* convertDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {
    printf("ompparser directive is ready.\n");
    OpenMPDirectiveKind directive_kind = current_OpenMPIR_to_SageIII.second->getKind();
    SgStatement* result = NULL;

    switch (directive_kind) {
        case OMPD_metadirective:
        case OMPD_teams:
        case OMPD_atomic:
        case OMPD_taskgroup:
        case OMPD_master:
        case OMPD_distribute:
        case OMPD_loop:
        case OMPD_scan:
        case OMPD_taskloop:
        case OMPD_target_enter_data:
        case OMPD_target_exit_data:
        case OMPD_target_parallel_for:
        case OMPD_task:
        case OMPD_taskwait:
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
        case OMPD_parallel: {
            result = convertBodyDirective(current_OpenMPIR_to_SageIII);
            break;
        }
        case OMPD_parallel_for:
        case OMPD_parallel_sections:        
        case OMPD_parallel_for_simd: {
            result = convertCombinedBodyDirective(current_OpenMPIR_to_SageIII);
            break;
        }
        case OMPD_declare_mapper:
        case OMPD_cancellation_point:
        case OMPD_cancel: {
            result = convertNonBodyDirective(current_OpenMPIR_to_SageIII);
            break;
        }
        case OMPD_barrier: {
            result = new SgOmpBarrierStatement();
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
    copyStartFileInfo (current_OpenMPIR_to_SageIII.first, result, NULL);
    copyEndFileInfo (current_OpenMPIR_to_SageIII.first, result, NULL);
    replaceOmpPragmaWithOmpStatement(current_OpenMPIR_to_SageIII.first, result);

    return result;
}

SgStatement* convertVariantDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {
    printf("ompparser variant directive is ready.\n");
    OpenMPDirectiveKind directive_kind = current_OpenMPIR_to_SageIII.second->getKind();
    SgStatement* result = NULL;

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
    // handle the SgFilePtr
    //copyStartFileInfo (current_OpenMPIR_to_SageIII.first, result, NULL);
    //copyEndFileInfo (current_OpenMPIR_to_SageIII.first, result, NULL);
    //replaceOmpPragmaWithOmpStatement(current_OpenMPIR_to_SageIII.first, result);

    return result;
}

SgOmpBodyStatement* convertCombinedBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {
    
    OpenMPDirectiveKind directive_kind = current_OpenMPIR_to_SageIII.second->getKind();
    // directives like parallel and for have a following code block beside the pragma itself.
    SgOmpBodyStatement* result = NULL;
    OpenMPClauseKind clause_kind;

    switch (directive_kind) {
        case OMPD_parallel_for:
        case OMPD_parallel_sections:
        case OMPD_parallel_for_simd: {
            result = convertOmpParallelStatementFromCombinedDirectives(current_OpenMPIR_to_SageIII);
            break;
        }
        default: {
            printf("Unknown directive is found.\n");
        }
    }
    return result;
}

SgOmpClause* convertSimpleClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser simple clause is ready.\n");
    SgOmpClause* sg_clause = NULL;
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
        case OMPC_write: {
            sg_clause = new SgOmpWriteClause();
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
        } break;
        default: {
            cerr<<"error: unknown clause "<<endl;
            ROSE_ASSERT(false);
        }
    };
    setOneSourcePositionForTransformation(sg_clause);
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    return sg_clause;
}

SgStatement* convertNonBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {
    
    OpenMPDirectiveKind directive_kind = current_OpenMPIR_to_SageIII.second->getKind();
    SgStatement* result = NULL;
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
        default: {
            printf("Unknown directive is found.\n");
        }
    }
    // extract all the clauses based on the vector of clauses in the original order
    SgOmpClause* sg_clause = NULL;
    std::vector<OpenMPClause*>* all_clauses = current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
    std::vector<OpenMPClause*>::iterator clause_iter;
    for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end(); clause_iter++) {
        clause_kind = (*clause_iter)->getKind();
        switch (clause_kind) {
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
            default: {
                cerr<<"error: unknown clause "<<endl;
                ROSE_ASSERT(false);
            }
        };
        ROSE_ASSERT(result);
        setOneSourcePositionForTransformation(sg_clause);
        ((SgOmpClauseStatement*)result)->get_clauses().push_back(sg_clause);
        sg_clause->set_parent(((SgOmpClauseStatement*)result));
    };

    return result;
}

SgOmpBodyStatement* convertBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {
    
    OpenMPDirectiveKind directive_kind = current_OpenMPIR_to_SageIII.second->getKind();
    // directives like parallel and for have a following code block beside the pragma itself.
    SgStatement* body = getOpenMPBlockBody(current_OpenMPIR_to_SageIII);
    removeStatement(body,false);
    SgOmpBodyStatement* result = NULL;
    OpenMPClauseKind clause_kind;

    switch (directive_kind) {
        case OMPD_parallel: {
            result = new SgOmpParallelStatement(NULL, body);
            break;
        }
        case OMPD_taskwait: {
            result = new SgOmpTaskwaitStatement(NULL, body);
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
        case OMPD_for: {
            result = new SgOmpForStatement(NULL, body);
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
            std::string name = ((OpenMPCriticalDirective*)(current_OpenMPIR_to_SageIII.second))->getCriticalName();
            result = new SgOmpCriticalStatement(NULL, body, SgName(name));
            break;
        }
        case OMPD_depobj: {
            std::string name = ((OpenMPDepobjDirective*)(current_OpenMPIR_to_SageIII.second))->getDepobj();
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
        case OMPD_end: {
            return result;
        }
        default: {
            printf("Unknown directive is found.\n");
        }
    }
    body->set_parent(result);
    // extract all the clauses based on the vector of clauses in the original order
    std::vector<OpenMPClause*>* all_clauses = current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
    std::vector<OpenMPClause*>::iterator clause_iter;
    for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end(); clause_iter++) {
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
            case OMPC_num_threads: {
                convertExpressionClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_default: {
                convertDefaultClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_proc_bind: {
                convertProcBindClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_order: {
                convertOrderClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_bind: {
                convertBindClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_when: {
                convertWhenClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_read:
            case OMPC_write:
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
            case OMPC_nowait: {
                convertSimpleClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_schedule: {
                convertScheduleClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_dist_schedule: {
                convertDistScheduleClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_defaultmap: {
                convertDefaultmapClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_map: {
                convertMapClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_depend: {
                convertDependClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_depobj_update: {
                convertDepobjUpdateClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            default: {
                convertClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
            }
        };
    };

    return result;
}

// Convert an OpenMPIR Threadprivate Directive to a ROSE node
// Because we have to do some non-standard things, I'm putting this in a separate function
SgStatement* convertOmpThreadprivateStatement(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {
    SgOmpThreadprivateStatement *statement = new SgOmpThreadprivateStatement();        
    OpenMPThreadprivateDirective *current_ir = static_cast<OpenMPThreadprivateDirective *>(current_OpenMPIR_to_SageIII.second);

    std::vector<const char*>* current_expressions = current_ir->getThreadprivateList();
    if (current_expressions->size() != 0) {
        std::vector<const char*>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            std::string expr_string = std::string() + "varlist " + *iter + "\n";
            omp_exprparser_parser_init(current_OpenMPIR_to_SageIII.first, expr_string.c_str());
            omp_exprparser_parse();
        }
    }

    std::vector<std::pair<std::string, SgNode*> >::iterator iter;
    for (iter = omp_variable_list.begin(); iter != omp_variable_list.end(); iter++) {
        if (SgInitializedName* iname = isSgInitializedName((*iter).second)) {
            SgVarRefExp * var_ref = buildVarRefExp(iname);
            statement->get_variables().push_back(var_ref);
            var_ref->set_parent(statement);
        }
        else if (SgVarRefExp* vref = isSgVarRefExp((*iter).second)) {
            statement->get_variables().push_back(vref);
            vref->set_parent(statement);
        }
        else {
            cerr << "error: unhandled type of variable within a list:" << ((*iter).second)->class_name();
        }
    }

    statement->set_definingDeclaration(statement);
    return statement;
}

SgOmpDepobjUpdateClause *convertDepobjUpdateClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser depobj update clause is ready.\n");
    
    OpenMPDepobjUpdateClauseDependeceType modifier = ((OpenMPDepobjUpdateClause*)current_omp_clause)->getType();
    SgOmpClause::omp_depobj_modifier_enum sg_type = toSgOmpClauseDepobjModifierType(modifier);
    SgOmpDepobjUpdateClause *result = new SgOmpDepobjUpdateClause(sg_type);
    ROSE_ASSERT(result);
    
    setOneSourcePositionForTransformation(result);
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    
    printf("ompparser depobj update clause added!\n");
    return result;
}

SgOmpScheduleClause* convertScheduleClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser schedule clause is ready.\n");

    OpenMPScheduleClauseModifier modifier1 = ((OpenMPScheduleClause*)current_omp_clause)->getModifier1();
    SgOmpClause::omp_schedule_modifier_enum sg_modifier1 = toSgOmpClauseScheduleModifier(modifier1);
    OpenMPScheduleClauseModifier modifier2 = ((OpenMPScheduleClause*)current_omp_clause)->getModifier2();
    SgOmpClause::omp_schedule_modifier_enum sg_modifier2 = toSgOmpClauseScheduleModifier(modifier2);
    OpenMPScheduleClauseKind kind = ((OpenMPScheduleClause*)current_omp_clause)->getKind();
    SgOmpClause::omp_schedule_kind_enum sg_kind = toSgOmpClauseScheduleKind(kind);

    SgExpression* chunk_size = NULL;
    if ( (((OpenMPScheduleClause*)current_omp_clause)->getChunkSize()) != "" ) {
        chunk_size = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),((OpenMPScheduleClause*)current_omp_clause)->getChunkSize());
    }

    SgOmpScheduleClause* result = new SgOmpScheduleClause( sg_modifier1, sg_modifier2, sg_kind, chunk_size );
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    printf("ompparser schedule clause is added.\n");
    return result;
}

SgOmpDistScheduleClause* convertDistScheduleClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser dist_schedule clause is ready.\n");

    OpenMPDistScheduleClauseKind kind = ((OpenMPDistScheduleClause*)current_omp_clause)->getKind();
    SgOmpClause::omp_dist_schedule_kind_enum sg_kind = toSgOmpClauseDistScheduleKind(kind);

    SgExpression* chunk_size = NULL;
    if ( (((OpenMPDistScheduleClause*)current_omp_clause)->getChunkSize()) != "" ) {
        chunk_size = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(),((OpenMPDistScheduleClause*)current_omp_clause)->getChunkSize());
    }

    SgOmpDistScheduleClause* result = new SgOmpDistScheduleClause( sg_kind, chunk_size );
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    printf("ompparser dist_schedule clause is added.\n");
    return result;
}

SgOmpDefaultmapClause* convertDefaultmapClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser defaultmap clause is ready.\n");

    OpenMPDefaultmapClauseBehavior behavior = ((OpenMPDefaultmapClause*)current_omp_clause)->getBehavior();
    SgOmpClause::omp_defaultmap_behavior_enum sg_behavior = toSgOmpClauseDefaultmapBehavior(behavior);

    OpenMPDefaultmapClauseCategory category = ((OpenMPDefaultmapClause*)current_omp_clause)->getCategory();
    SgOmpClause::omp_defaultmap_category_enum sg_category = toSgOmpClauseDefaultmapCategory(category);

    SgOmpDefaultmapClause* result = new SgOmpDefaultmapClause( sg_behavior, sg_category );
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    printf("ompparser defaultmap clause is added.\n");
    return result;
}

SgOmpMapClause* convertMapClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser map clause is ready.\n");
    SgOmpMapClause* result = NULL;
    SgExpression* clause_expression = NULL;
    OpenMPMapClauseType type = ((OpenMPMapClause*)current_omp_clause)->getType();
    SgOmpClause::omp_map_operator_enum sg_type = toSgOmpClauseMapOperator(type);

    std::vector<const char*>* current_expressions = current_omp_clause->getExpressions();
    if (current_expressions->size() != 0) {
        std::vector<const char*>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            clause_expression = parseOmpArraySection(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), *iter);
        }
    }
    SgExprListExp* explist=buildExprListExp();

    result = new SgOmpMapClause(explist, sg_type);
    ROSE_ASSERT(result != NULL);
    buildVariableList(result);
    explist->set_parent(result);
    result->set_array_dimensions(array_dimensions);

    setOneSourcePositionForTransformation(result);
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    array_dimensions.clear();
    omp_variable_list.clear();
    printf("ompparser map clause is added.\n");
    return result;
}

SgOmpBodyStatement* convertVariantBodyDirective(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {

    OpenMPDirectiveKind directive_kind = current_OpenMPIR_to_SageIII.second->getKind();
    // directives like parallel and for have a following code block beside the pragma itself.
    //SgStatement* body = getOpenMPBlockBody(current_OpenMPIR_to_SageIII);
    //removeStatement(body,false);
    SgOmpBodyStatement* result = NULL;
    OpenMPClauseKind clause_kind;

    switch (directive_kind) {
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
        case OMPD_taskwait: {
            result = new SgOmpTaskwaitStatement(NULL, NULL);
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
            std::string name = ((OpenMPCriticalDirective*)(current_OpenMPIR_to_SageIII.second))->getCriticalName();
            result = new SgOmpCriticalStatement(NULL, NULL, SgName(name));
            break;
        }
        case OMPD_depobj: {
            std::string name = ((OpenMPDepobjDirective*)(current_OpenMPIR_to_SageIII.second))->getDepobj();
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
        case OMPD_end: {
            return result;
        }
        default: {
            printf("Unknown directive is found.\n");
        }
    }
    //body->set_parent(result);
    // extract all the clauses based on the vector of clauses in the original order
    std::vector<OpenMPClause*>* all_clauses = current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
    std::vector<OpenMPClause*>::iterator clause_iter;
    for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end(); clause_iter++) {
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
                convertExpressionClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_default: {
                convertDefaultClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_proc_bind: {
                convertProcBindClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_order: {
                convertOrderClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_bind: {
                convertBindClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            case OMPC_when: {
                convertWhenClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
                break;
            }
            default: {
                convertClause(isSgOmpClauseBodyStatement(result), current_OpenMPIR_to_SageIII, *clause_iter);
            }
        };
    };
    
    return result;
}

SgStatement* getOpenMPBlockBody(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII) {

    SgStatement* result = NULL;
    result = getNextStatement(current_OpenMPIR_to_SageIII.first);
    return result;

}

  //! Build SgOmpDefaultClause from OpenMPIR
SgOmpDefaultClause* convertDefaultClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    OpenMPDefaultClauseKind default_kind = ((OpenMPDefaultClause*)current_omp_clause)->getDefaultClauseKind();
    SgOmpClause::omp_default_option_enum sg_dv;
    SgStatement* variant_directive = NULL;
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
        OpenMPDirective* variant_OpenMPIR = ((OpenMPDefaultClause*)current_omp_clause)->getVariantDirective();
        std::pair<SgPragmaDeclaration*, OpenMPDirective*> paired_variant_OpenMPIR = make_pair(current_OpenMPIR_to_SageIII.first, variant_OpenMPIR);
        variant_directive = convertVariantDirective(paired_variant_OpenMPIR);
        break;
      }
      default: {
          cerr << "error: buildOmpDefaultClase() Unacceptable default option from OpenMPIR:" << default_kind;
          ROSE_ASSERT(false);
      }
    }; //end switch
    SgOmpDefaultClause* result = new SgOmpDefaultClause(sg_dv, variant_directive);
    setOneSourcePositionForTransformation(result);

    if (variant_directive != NULL) {
        variant_directive->set_parent(result);
    };

    // reconsider the location of following code to attach clause
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);

    return result;
}

  //! Build SgOmpProcBindClause from OpenMPIR
SgOmpProcBindClause* convertProcBindClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    OpenMPProcBindClauseKind proc_bind_kind = ((OpenMPProcBindClause*)current_omp_clause)->getProcBindClauseKind();
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
        cerr << "error: buildOmpProcBindClause () Unacceptable default option from OpenMPIR:" << proc_bind_kind;
        ROSE_ASSERT(false);
      }
    }; //end switch
    SgOmpProcBindClause* result = new SgOmpProcBindClause(sg_dv);
    setOneSourcePositionForTransformation(result);

    // reconsider the location of following code to attach clause
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);

    return result;
}

SgOmpOrderClause* convertOrderClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    OpenMPOrderClauseKind order_kind = ((OpenMPOrderClause*)current_omp_clause)->getOrderClauseKind();
    SgOmpClause::omp_order_kind_enum sg_dv = SgOmpClause::e_omp_order_kind_unspecified;
    switch (order_kind) {
      case OMPC_ORDER_concurrent: {
        sg_dv = SgOmpClause::e_omp_order_kind_concurrent;
        break;
      }
      default: {
        cerr << "error: buildOmpOrderClause () Unacceptable default option from OpenMPIR:" << order_kind;
      }
    }; //end switch
    SgOmpOrderClause* result = new SgOmpOrderClause(sg_dv);
    setOneSourcePositionForTransformation(result);

    // reconsider the location of following code to attach clause
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);

    return result;
}

SgOmpBindClause* convertBindClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    OpenMPBindClauseBinding bind_binding = ((OpenMPBindClause*)current_omp_clause)->getBindClauseBinding();
    SgOmpClause::omp_bind_binding_enum sg_dv = SgOmpClause::e_omp_bind_binding_unspecified;
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
        cerr << "error: buildOmpBindClause () Unacceptable default option from OpenMPIR:" << bind_binding;
      }
    }; //end switch
    SgOmpBindClause* result = new SgOmpBindClause(sg_dv);
    setOneSourcePositionForTransformation(result);

    // reconsider the location of following code to attach clause
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);

    return result;
}

SgOmpWhenClause* convertWhenClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("when clause is coming.\n");
    SgStatement* variant_directive = NULL;
    OpenMPDirective* variant_OpenMPIR = ((OpenMPWhenClause*)current_omp_clause)->getVariantDirective();
    if (variant_OpenMPIR) {
        std::pair<SgPragmaDeclaration*, OpenMPDirective*> paired_variant_OpenMPIR = make_pair(current_OpenMPIR_to_SageIII.first, variant_OpenMPIR);
        variant_directive = convertVariantDirective(paired_variant_OpenMPIR);
    };

    SgExpression* user_condition = NULL;
    std::string user_condition_string = ((OpenMPWhenClause*)current_omp_clause)->getUserCondition()->second;
    if (user_condition_string.size()) {
        user_condition = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), user_condition_string.c_str());
    };
    SgExpression* user_condition_score = NULL;
    std::string user_condition_score_string = ((OpenMPWhenClause*)current_omp_clause)->getUserCondition()->first;
    if (user_condition_score_string.size()) {
        user_condition_score = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), user_condition_score_string.c_str());
    };

    SgExpression* device_arch = NULL;
    std::string device_arch_string = ((OpenMPWhenClause*)current_omp_clause)->getArchExpression()->second;
    if (device_arch_string.size()) {
        device_arch = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), device_arch_string.c_str());
    };

    SgExpression* device_isa = NULL;
    std::string device_isa_string = ((OpenMPWhenClause*)current_omp_clause)->getIsaExpression()->second;
    if (device_isa_string.size()) {
        device_isa = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), device_isa_string.c_str());
    };

    SgOmpClause::omp_when_context_kind_enum sg_device_kind = SgOmpClause::e_omp_when_context_kind_unknown;
    OpenMPClauseContextKind device_kind = ((OpenMPWhenClause*)current_omp_clause)->getContextKind()->second;
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
    SgOmpClause::omp_when_context_vendor_enum sg_implementation_vendor = SgOmpClause::e_omp_when_context_vendor_unspecified;
    OpenMPClauseContextVendor implementation_vendor = ((OpenMPWhenClause*)current_omp_clause)->getImplementationKind()->second;
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

    SgExpression* implementation_user_defined = NULL;
    std::string implementation_user_defined_string = ((OpenMPWhenClause*)current_omp_clause)->getImplementationExpression()->second;
    if (implementation_user_defined_string.size()) {
        implementation_user_defined = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), implementation_user_defined_string.c_str());
    };

    SgExpression* implementation_extension = NULL;
    std::string implementation_extension_string = ((OpenMPWhenClause*)current_omp_clause)->getExtensionExpression()->second;
    if (implementation_extension_string.size()) {
        implementation_extension = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), implementation_extension_string.c_str());
    };

    SgOmpWhenClause* result = new SgOmpWhenClause(user_condition, user_condition_score, device_arch, device_isa, sg_device_kind, sg_implementation_vendor, implementation_user_defined, implementation_extension, variant_directive);
    std::vector<std::pair<std::string, OpenMPDirective*> >* construct_directive = ((OpenMPWhenClause*)current_omp_clause)->getConstructDirective();
    if (construct_directive->size()) {
        std::list<SgStatement*> sg_construct_directives;
        SgStatement* sg_construct_directive = NULL;
        for (unsigned int i = 0; i < construct_directive->size(); i++) {
            std::pair<SgPragmaDeclaration*, OpenMPDirective*> paired_construct_OpenMPIR = make_pair(current_OpenMPIR_to_SageIII.first, construct_directive->at(i).second);
            sg_construct_directive = convertVariantDirective(paired_construct_OpenMPIR);
            sg_construct_directives.push_back(sg_construct_directive);
        };
        result->set_construct_directives(sg_construct_directives);
    };

    setOneSourcePositionForTransformation(result);
    if (variant_directive != NULL) {
        variant_directive->set_parent(result);
    };

    // reconsider the location of following code to attach clause
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);

    return result;
}


SgOmpVariablesClause* convertClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
   // printf("ompparser clause is ready.\n");
    omp_variable_list.clear();
    SgOmpVariablesClause* result = NULL;
    OpenMPClauseKind clause_kind = current_omp_clause->getKind();
    SgGlobal* global = SageInterface::getGlobalScope(current_OpenMPIR_to_SageIII.first);
    std::vector<const char*>* current_expressions = current_omp_clause->getExpressions();
    if (current_expressions->size() != 0) {
        std::vector<const char*>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            std::string expr_string = std::string() + "varlist " + *iter + "\n";
            omp_exprparser_parser_init(current_OpenMPIR_to_SageIII.first, expr_string.c_str());
            omp_exprparser_parse();
        }
    }

    SgExprListExp* explist = buildExprListExp();
    switch (clause_kind) {
        case OMPC_allocate: {
            OpenMPAllocateClauseAllocator allocate_allocator = ((OpenMPAllocateClause*)current_omp_clause)->getAllocator();
            SgOmpClause::omp_allocate_modifier_enum sg_modifier = toSgOmpClauseAllocateAllocator(allocate_allocator);
            SgExpression* user_defined_parameter = NULL;
            if (sg_modifier == SgOmpClause::e_omp_allocate_user_defined_modifier) {
                SgExpression* clause_expression = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), ((OpenMPAllocateClause*)current_omp_clause)->getUserDefinedAllocator());
                user_defined_parameter = checkOmpExpressionClause(clause_expression, global, e_allocate);
            }
            result = new SgOmpAllocateClause(explist, sg_modifier, user_defined_parameter);
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
            OpenMPReductionClauseModifier modifier = ((OpenMPReductionClause*)current_omp_clause)->getModifier();
            SgOmpClause::omp_reduction_modifier_enum sg_modifier = toSgOmpClauseReductionModifier(modifier);
            OpenMPReductionClauseIdentifier identifier = ((OpenMPReductionClause*)current_omp_clause)->getIdentifier();
            SgOmpClause::omp_reduction_identifier_enum sg_identifier = toSgOmpClauseReductionIdentifier(identifier);
            SgExpression* user_defined_identifier = NULL;
            if (sg_identifier == SgOmpClause::e_omp_reduction_user_defined_identifier) {
                SgExpression* clause_expression = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), ((OpenMPReductionClause*)current_omp_clause)->getUserDefinedIdentifier());
                user_defined_identifier = checkOmpExpressionClause(clause_expression, global, e_reduction);
            }
            result = new SgOmpReductionClause(explist, sg_modifier, sg_identifier, user_defined_identifier);
            printf("Reduction Clause added!\n");
            break;
        }
        case OMPC_in_reduction: {
            OpenMPInReductionClauseIdentifier identifier = ((OpenMPInReductionClause*)current_omp_clause)->getIdentifier();
            SgOmpClause::omp_in_reduction_identifier_enum sg_identifier = toSgOmpClauseInReductionIdentifier(identifier);
            SgExpression* user_defined_identifier = NULL;
            if (sg_identifier == SgOmpClause::e_omp_in_reduction_user_defined_identifier) {
                SgExpression* clause_expression = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), ((OpenMPInReductionClause*)current_omp_clause)->getUserDefinedIdentifier());
                user_defined_identifier = checkOmpExpressionClause(clause_expression, global, e_reduction);
            }
            result = new SgOmpInReductionClause(explist, sg_identifier, user_defined_identifier);
            printf("In_reduction Clause added!\n");
            break;
        }
        case OMPC_task_reduction: {
            OpenMPTaskReductionClauseIdentifier identifier = ((OpenMPTaskReductionClause*)current_omp_clause)->getIdentifier();
            SgOmpClause::omp_task_reduction_identifier_enum sg_identifier = toSgOmpClauseTaskReductionIdentifier(identifier);
            SgExpression* user_defined_identifier = NULL;
            if (sg_identifier == SgOmpClause::e_omp_task_reduction_user_defined_identifier) {
                SgExpression* clause_expression = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), ((OpenMPTaskReductionClause*)current_omp_clause)->getUserDefinedIdentifier());
                user_defined_identifier = checkOmpExpressionClause(clause_expression, global, e_reduction);
            }
            result = new SgOmpTaskReductionClause(explist, sg_identifier, user_defined_identifier);
            printf("Task_reduction Clause added!\n");
            break;
        }
        case OMPC_linear: {
            OpenMPLinearClauseModifier modifier = ((OpenMPLinearClause*)current_omp_clause)->getModifier();
            SgOmpClause::omp_linear_modifier_enum sg_modifier = toSgOmpClauseLinearModifier(modifier);
            SgExpression* stepExp = NULL;
            if ( (((OpenMPLinearClause*)current_omp_clause)->getUserDefinedStep()) != "" ) {
                stepExp = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), ((OpenMPLinearClause*)current_omp_clause)->getUserDefinedStep());
            }
            result = new SgOmpLinearClause(explist, stepExp, sg_modifier);
            printf("Linear Clause added!\n");
            break;
        }
        case OMPC_aligned: {
            SgExpression* alignExp = NULL;
            if ( (((OpenMPAlignedClause*)current_omp_clause)->getUserDefinedAlignment()) != "" ) {
                alignExp = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), ((OpenMPAlignedClause*)current_omp_clause)->getUserDefinedAlignment());
            }
            result = new SgOmpAlignedClause(explist, alignExp);
            printf("Aligned Clause added!\n");
            break;
        }
        case OMPC_lastprivate: {
            OpenMPLastprivateClauseModifier modifier = ((OpenMPLastprivateClause*)current_omp_clause)->getModifier();
            SgOmpClause::omp_lastprivate_modifier_enum sg_modifier = toSgOmpClauseLastprivateModifier(modifier);
            result = new SgOmpLastprivateClause(explist, sg_modifier);
            printf("Lastprivate Clause added!\n");
            break;
        }
        case OMPC_shared: {
            result = new SgOmpSharedClause(explist);
            printf("Shared Clause added!\n");
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
    SgOmpClause* sg_clause = result; 
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    omp_variable_list.clear();
    return result;

}

SgOmpDependClause* convertDependClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser depend clause is ready.\n");
    SgOmpDependClause* result = NULL;
    SgExpression* clause_expression = NULL;
    
    SgExpression* iterator_type = NULL;
    SgExpression* identifier = NULL;
    SgExpression* begin = NULL;
    SgExpression* end = NULL;
    SgExpression* step = NULL;
    
    OpenMPDependClauseModifier modifier = ((OpenMPDependClause*)current_omp_clause)->getModifier();
    std::vector<vector<const char*>* > *omp_depend_iterators_definition_class = NULL;
    std::list<std::list<SgExpression*> > depend_iterators_definition_class;
    if(modifier == OMPC_DEPEND_MODIFIER_iterator) {
        omp_depend_iterators_definition_class = ((OpenMPDependClause*)current_omp_clause)->getDependIteratorsDefinitionClass();
        for (unsigned int i = 0; i < omp_depend_iterators_definition_class->size(); i++) {
            std::list<SgExpression*> iterator_expressions;
            if ((string)(omp_depend_iterators_definition_class->at(i)->at(0)) != "") {
                iterator_type = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), std::string(omp_depend_iterators_definition_class->at(i)->at(0)));
                iterator_expressions.push_back(iterator_type); 
            }
            else {
                iterator_type = NULL;
                iterator_expressions.push_back(iterator_type);   
            }
            identifier = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), std::string(omp_depend_iterators_definition_class->at(i)->at(1)));
            iterator_expressions.push_back(identifier);
            begin = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), std::string(omp_depend_iterators_definition_class->at(i)->at(2)));
            iterator_expressions.push_back(begin);
            end = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), std::string(omp_depend_iterators_definition_class->at(i)->at(3)));
            iterator_expressions.push_back(end);
            
            if((string)(omp_depend_iterators_definition_class->at(i)->at(4)) != "") {
                step = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), std::string(omp_depend_iterators_definition_class->at(i)->at(4)));
                iterator_expressions.push_back(step);
            } else {
                step = NULL;
                iterator_expressions.push_back(step);
            }
            depend_iterators_definition_class.push_back(iterator_expressions);
        }
    }  
    SgOmpClause::omp_depend_modifier_enum sg_modifier = toSgOmpClauseDependModifier(modifier);
    OpenMPDependClauseType type = ((OpenMPDependClause*)current_omp_clause)->getType();
    SgOmpClause::omp_dependence_type_enum sg_type = toSgOmpClauseDependenceType(type);

    std::vector<const char*>* current_expressions = current_omp_clause->getExpressions();
    if (current_expressions->size() != 0) {
        std::vector<const char*>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            clause_expression = parseOmpArraySection(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), *iter);
        }
    }
    SgExprListExp* explist=buildExprListExp();

    result = new SgOmpDependClause(explist, sg_modifier, sg_type);
    ROSE_ASSERT(result != NULL);
    buildVariableList(result);
    explist->set_parent(result);
    result->set_array_dimensions(array_dimensions);
    result->set_iterator(depend_iterators_definition_class);
    setOneSourcePositionForTransformation(result);
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);
    array_dimensions.clear();
    omp_variable_list.clear();
    printf("ompparser depend clause is added.\n");
    return result;
}

SgOmpExpressionClause* convertExpressionClause(SgOmpClauseBodyStatement* clause_body, std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII, OpenMPClause* current_omp_clause) {
    printf("ompparser expression clause is ready.\n");
    SgOmpExpressionClause* result = NULL;
    SgExpression* clause_expression = NULL;
    SgGlobal* global = SageInterface::getGlobalScope(current_OpenMPIR_to_SageIII.first);
    OpenMPClauseKind clause_kind = current_omp_clause->getKind();
    std::vector<const char*>* current_expressions = current_omp_clause->getExpressions();
    if (current_expressions->size() != 0) {
        std::vector<const char*>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            clause_expression = parseOmpExpression(current_OpenMPIR_to_SageIII.first, current_omp_clause->getKind(), *iter);
        }
    }

    switch (clause_kind) {
        case OMPC_if: {
            OpenMPIfClauseModifier if_modifier = ((OpenMPIfClause*)current_omp_clause)->getModifier();
            SgOmpClause::omp_if_modifier_enum sg_modifier = toSgOmpClauseIfModifier(if_modifier);
            clause_expression->set_parent(current_OpenMPIR_to_SageIII.first);
            SgExpression* if_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpIfClause(if_expression, sg_modifier);
            printf("If Clause added!\n");
            break;
        }
        case OMPC_num_threads: {
            SgExpression* num_threads_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpNumThreadsClause(num_threads_expression);
            printf("Num_threads Clause added!\n");
            break;
        }
        case OMPC_num_teams: {
            SgExpression* num_teams_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpNumTeamsClause(num_teams_expression);
            printf("Num_teams Clause added!\n");
            break;
        }
        case OMPC_grainsize: {
            SgExpression* grainsize_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpGrainsizeClause(grainsize_expression);
            printf("Grainsize Clause added!\n");
            break;
        }
        case OMPC_detach: {
            SgExpression* detach_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpDetachClause(detach_expression);
            printf("Detach Clause added!\n");
            break;
        }
        case OMPC_num_tasks: {
            SgExpression* num_tasks_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpNumTasksClause(num_tasks_expression);
            printf("Num_tasks Clause added!\n");
            break;
        }
        case OMPC_final: {
            SgExpression* final_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpFinalClause(final_expression);
            printf("Final Clause added!\n");
            break;
        }
        case OMPC_priority: {
            SgExpression* priority_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpPriorityClause(priority_expression);
            printf("Priority Clause added!\n");
            break;
        }
        case OMPC_hint: {
            SgExpression* hint_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpHintClause(hint_expression);
            printf("hint Clause added!\n");
            break;
        }
        case OMPC_safelen: {
            SgExpression* safelen_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpSafelenClause(safelen_expression);
            printf("Safelen Clause added!\n");
            break;
        }
        case OMPC_simdlen: {
            SgExpression* simdlen_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpSimdlenClause(simdlen_expression);
            printf("Simdlen Clause added!\n");
            break;
        }
        case OMPC_ordered: {
            SgExpression* ordered_expression = checkOmpExpressionClause(clause_expression, global, e_ordered_clause);
            result = new SgOmpOrderedClause(ordered_expression);
            printf("Ordered Clause added!\n");
            break;
        }
        case OMPC_collapse: {
            SgExpression* collapse_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpCollapseClause(collapse_expression);
            printf("Collapse Clause added!\n");
            break;
        }
        case OMPC_thread_limit: {
            SgExpression* thread_limit_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpThreadLimitClause(thread_limit_expression);
            printf("Thread_limit Clause added!\n");
            break;
        }
        case OMPC_device: {
            OpenMPDeviceClauseModifier modifier = ((OpenMPDeviceClause*)current_omp_clause)->getModifier();
            SgOmpClause::omp_device_modifier_enum sg_modifier = toSgOmpClauseDeviceModifier(modifier);
            clause_expression->set_parent(current_OpenMPIR_to_SageIII.first);
            SgExpression* device_expression = checkOmpExpressionClause(clause_expression, global, e_num_threads);
            result = new SgOmpDeviceClause(device_expression, sg_modifier);
            printf("Device Clause added!\n");
            break;
        }
        default: {
            printf("Unknown Clause!\n");
        }
    }
    setOneSourcePositionForTransformation(result);

    // reconsider the location of following code to attach clause
    SgOmpClause* sg_clause = result;
    clause_body->get_clauses().push_back(sg_clause);
    sg_clause->set_parent(clause_body);

    return result;
}

SgExpression* parseOmpExpression(SgPragmaDeclaration* directive, OpenMPClauseKind clause_kind, std::string expression) {
    // special handling for omp declare simd directive
    // It may have clauses referencing a variable declared in an immediately followed function's parameter list
    bool look_forward = false;
    if (isSgOmpDeclareSimdStatement(directive) && (clause_kind == OMPC_linear ||
        clause_kind == OMPC_simdlen ||
        clause_kind == OMPC_aligned ||
        clause_kind == OMPC_uniform)) {
        look_forward = true;
    };
    std::string expr_string = std::string() + "expr (" + expression + ")\n";
    SgExpression* sg_expression = parseExpression(directive, look_forward, expr_string.c_str());

    return sg_expression;
}

SgExpression* parseOmpArraySection(SgPragmaDeclaration* directive, OpenMPClauseKind clause_kind, std::string expression) {
    // special handling for omp declare simd directive
    // It may have clauses referencing a variable declared in an immediately followed function's parameter list
    bool look_forward = false;
    if (isSgOmpDeclareSimdStatement(directive) && (clause_kind == OMPC_linear ||
        clause_kind == OMPC_simdlen ||
        clause_kind == OMPC_aligned ||
        clause_kind == OMPC_uniform)) {
        look_forward = true;
    };
    std::string expr_string = std::string() + "array_section (" + expression + ")\n";
    SgExpression* sg_expression = parseArraySectionExpression(directive, look_forward, expr_string.c_str());

    return sg_expression;
}


void buildVariableList(SgOmpVariablesClause* current_omp_clause) {

    std::vector<std::pair<std::string, SgNode*> >::iterator iter;
    for (iter = omp_variable_list.begin(); iter != omp_variable_list.end(); iter++) {
        if (SgInitializedName* iname = isSgInitializedName((*iter).second)) {
            SgVarRefExp * var_ref = buildVarRefExp(iname);
            current_omp_clause->get_variables()->get_expressions().push_back(var_ref);
            var_ref->set_parent(current_omp_clause);
        }
        else if (SgPntrArrRefExp* aref= isSgPntrArrRefExp((*iter).second)) {
            current_omp_clause->get_variables()->get_expressions().push_back(aref);
            aref->set_parent(current_omp_clause);
        }
        else if (SgVarRefExp* vref = isSgVarRefExp((*iter).second)) {
            current_omp_clause->get_variables()->get_expressions().push_back(vref);
            vref->set_parent(current_omp_clause);
        }
        else {
            cerr << "error: unhandled type of variable within a list:" << ((*iter).second)->class_name();
        }
    }
}

SgOmpParallelStatement* convertOmpParallelStatementFromCombinedDirectives(std::pair<SgPragmaDeclaration*, OpenMPDirective*> current_OpenMPIR_to_SageIII)
  {
    ROSE_ASSERT(current_OpenMPIR_to_SageIII.second != NULL);
    SgStatement* body = getOpenMPBlockBody(current_OpenMPIR_to_SageIII);
    removeStatement(body,false);
    ROSE_ASSERT(body != NULL);

    // build the 2nd directive node first
    SgStatement * second_stmt = NULL; 
    switch (current_OpenMPIR_to_SageIII.second->getKind())  
    {
      case OMPD_parallel_for:
        {
          second_stmt = new SgOmpForStatement(NULL, body);
          break;
        }
      case OMPD_parallel_for_simd:
        {
            second_stmt = new SgOmpForSimdStatement(NULL, body);
            break;
        }
      case OMPD_parallel_sections:
        {
            second_stmt = new SgOmpSectionsStatement(NULL, body);
            break;
        }
      default:
        {
          cerr<<"error: unacceptable directive type in convertOmpParallelStatementFromCombinedDirectives() "<<endl;
          ROSE_ASSERT(false);
        }
    }
    
    setOneSourcePositionForTransformation(second_stmt);

    ROSE_ASSERT(second_stmt);
    body->set_parent(second_stmt);

    copyStartFileInfo (current_OpenMPIR_to_SageIII.first, second_stmt, NULL);
    copyEndFileInfo (current_OpenMPIR_to_SageIII.first, second_stmt, NULL);
    SgOmpParallelStatement* first_stmt = new SgOmpParallelStatement(NULL, second_stmt); 
    setOneSourcePositionForTransformation(first_stmt);
    copyStartFileInfo (current_OpenMPIR_to_SageIII.first, first_stmt, NULL);
    copyEndFileInfo (current_OpenMPIR_to_SageIII.first, first_stmt, NULL);
    second_stmt->set_parent(first_stmt);

    OpenMPClauseKind clause_kind;
    std::vector<OpenMPClause *> * clause_vector = current_OpenMPIR_to_SageIII.second->getClausesInOriginalOrder();
    SgOmpClause* sgclause = NULL;
    std::vector<OpenMPClause*>::iterator citer;
    for (citer = clause_vector->begin(); citer != clause_vector->end(); citer++) {
        clause_kind = (*citer)->getKind();
        switch (clause_kind) {
            case OMPC_collapse:
            case OMPC_ordered:
            case OMPC_if:
            case OMPC_num_threads: {
                if (clause_kind == OMPC_collapse || clause_kind == OMPC_ordered) {
                    sgclause = convertExpressionClause(isSgOmpClauseBodyStatement(second_stmt), current_OpenMPIR_to_SageIII, *citer);
                }
                else {
                    sgclause = convertExpressionClause(isSgOmpClauseBodyStatement(first_stmt), current_OpenMPIR_to_SageIII, *citer);
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
            case OMPC_shared: {
                if (clause_kind == OMPC_shared || clause_kind == OMPC_copyin) {
                    sgclause = convertClause(isSgOmpClauseBodyStatement(first_stmt), current_OpenMPIR_to_SageIII, *citer);
                }
                else {
                    sgclause = convertClause(isSgOmpClauseBodyStatement(second_stmt), current_OpenMPIR_to_SageIII, *citer);
                };
                break;
            }
            case OMPC_default: {
                sgclause = convertDefaultClause(isSgOmpClauseBodyStatement(first_stmt), current_OpenMPIR_to_SageIII, *citer);
            break;
            }
            case OMPC_proc_bind: {
                sgclause = convertProcBindClause(isSgOmpClauseBodyStatement(first_stmt), current_OpenMPIR_to_SageIII, *citer);
            break;
            }
            case OMPC_schedule: {
                sgclause = convertScheduleClause(isSgOmpClauseBodyStatement(second_stmt), current_OpenMPIR_to_SageIII, *citer);
            break;
            }
            case OMPC_nowait: {
                sgclause = convertSimpleClause(isSgOmpClauseBodyStatement(second_stmt), current_OpenMPIR_to_SageIII, *citer);
            break;
            }
            case OMPC_parallel: {
                sgclause = convertSimpleClause(isSgOmpClauseBodyStatement(second_stmt), current_OpenMPIR_to_SageIII, *citer);
            break;
            }
            default: {
                cerr<<"error: unacceptable clause for combined parallel for directive"<<endl;
                ROSE_ASSERT(false);
            }
        };
    };
    movePreprocessingInfo(body, first_stmt, PreprocessingInfo::before, PreprocessingInfo::after, true);
    return first_stmt;
  }

  //! For C/C++ replace OpenMP pragma declaration with an SgOmpxxStatement
  void replaceOmpPragmaWithOmpStatement(SgPragmaDeclaration* pdecl, SgStatement* ompstmt)
  {
    ROSE_ASSERT(pdecl != NULL);
    ROSE_ASSERT(ompstmt!= NULL);

    SgScopeStatement* scope = pdecl ->get_scope();
    ROSE_ASSERT(scope !=NULL);
    // replace the pragma
    moveUpPreprocessingInfo(ompstmt, pdecl); // keep #ifdef etc attached to the pragma
    replaceStatement(pdecl, ompstmt);
  }


bool checkOpenMPIR(OpenMPDirective* directive) {

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
        case OMPD_depobj:
        case OMPD_distribute:
        case OMPD_for:
        case OMPD_for_simd:
        case OMPD_loop:
        case OMPD_master:
        case OMPD_metadirective:
        case OMPD_parallel:
        case OMPD_parallel_for:
        case OMPD_parallel_sections:
        case OMPD_parallel_for_simd:
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
        case OMPD_task:
        case OMPD_taskgroup:
        case OMPD_taskloop:
        case OMPD_taskwait:
        case OMPD_taskyield:
        case OMPD_teams:
        case OMPD_threadprivate: {
            break;
        }
        default: {
            return false;
        }
    };
    std::map<OpenMPClauseKind, std::vector<OpenMPClause*>* >* clauses = directive->getAllClauses();
    if (clauses != NULL) {
        std::map<OpenMPClauseKind, std::vector<OpenMPClause*>* >::iterator it;
        for (it = clauses->begin(); it != clauses->end(); it++) {
            switch (it->first) {
                case OMPC_acq_rel:
                case OMPC_acquire:
                case OMPC_aligned:
                case OMPC_allocate:
                case OMPC_bind:
                case OMPC_capture:
                case OMPC_collapse:
                case OMPC_copyin:
                case OMPC_copyprivate:
                case OMPC_default:
                case OMPC_defaultmap:
                case OMPC_depend:
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
                case OMPC_inclusive:
                case OMPC_is_device_ptr:
                case OMPC_lastprivate:
                case OMPC_linear:
                case OMPC_map:
                case OMPC_mergeable:
                case OMPC_nogroup:
                case OMPC_nontemporal:
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
                case OMPC_untied:
                case OMPC_update:
                case OMPC_use_device_addr:
                case OMPC_use_device_ptr:
                case OMPC_when:
                case OMPC_write: {
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

static bool ofs_skip_whitespace()
{
  bool result = false;
  while ((*c_char)==' '||(*c_char)=='\t')
  {
    c_char++;
    result= true;
  }
  return result;
}

static bool ofs_match_substr(const char* substr, bool checkTrail = true)
{
  bool result = true;
  const char* old_char = c_char;
  // we skip leading space from the target string
  ofs_skip_whitespace();
  size_t len =strlen(substr);
  for (size_t i =0; i<len; i++)
  {
    if ((*c_char)==substr[i])
    {
      c_char++;
    }
    else
    {
      result = false;
      c_char = old_char;
      break;
    }
  }
  // handle the next char after the substr match:
  // could only be either space or \n, \0, \t, !comments
  // or the match is revoked, e.g: "parallel1" match sub str "parallel" but
  // the trail is not legal
  // TODO: any other characters?
  if (checkTrail)
  {
    if (*c_char!=' '&&*c_char!='\0'&&*c_char!='\n'&&*c_char!='\t' &&*c_char!='!')
    {
      result = false;
      c_char = old_char;
    }
  }
  return result;
}

//! Check if the current Fortran SgFile has fixed source form
static bool isFixedSourceForm()
{
  bool result = false;
  SgFile * file = SageInterface::getEnclosingFileNode(c_sgnode);
  ROSE_ASSERT(file != NULL);

  // Only make sense for Fortran files
  ROSE_ASSERT (file->get_Fortran_only());
  if (file->get_inputFormat()==SgFile::e_unknown_output_format )
  { // default case: only f77 has fixed form
    if (file->get_F77_only())
      result = true;
    else
      result = false;
  }
  else // explicit case: any Fortran could be set to fixed form
  {
    if (file->get_inputFormat()==SgFile::e_fixed_form_output_format)
      result = true;
    else
      result = false;
  }
  return result;
}


static bool ofs_is_omp_sentinels()
{
  bool result = false;
  // two additional case for fixed form
  if (isFixedSourceForm())
  {
    if (ofs_match_substr("c$omp")||ofs_match_substr("*$omp"))
      result = true;
  }
  // a common case for all situations
  if (ofs_match_substr("!$omp"))
    result = true;
  return result;
}

static bool ofs_is_omp_sentinels(const char* str, SgNode* node)
{
  bool result;
  assert (node&&str);
  // make sure it is side effect free
  const char* old_char = c_char;
  SgNode* old_node = c_sgnode;

  c_char = str;
  c_sgnode = node;
  result = ofs_is_omp_sentinels();

  c_char = old_char;
  c_sgnode = old_node;

  return result;
}

//! A helper function to remove Fortran '!comments', but not '!$omp ...' from a string
// handle complex cases like:
// ... !... !$omp .. !...
static void removeFortranComments(string &buffer)
{
  size_t pos1 ;
  size_t pos2;
  size_t pos3=string::npos;

  pos1= buffer.rfind("!", pos3);
  while (pos1!=string::npos)
  {
    pos2= buffer.rfind("!$omp",pos3);
    if (pos1!=pos2) // is a real comment if not !$omp
    {
      buffer.erase(pos1);
    }
    else // find "!$omp", cannot stop here since there might have another '!' before it
      //limit the search range
    {
      if (pos2>=1)
        pos3= pos2-1;
      else
        break;
    }
    pos1= buffer.rfind("!", pos3);
  }
}

//!  A helper function to tell if a line has an ending '&', followed by optional space , tab , '\n',
// "!comments" should be already removed  (call removeFortranComments()) before calling this function
static bool hasFortranLineContinuation(const string& buffer)
{
  // search backwards for '&'
  size_t pos = buffer.rfind("&");
  if (pos ==string::npos)
    return false;
  else
  {
    // make sure the characters after & is legal
    for (size_t i = ++pos; i<buffer.length(); i++)
    {
      char c= buffer[i];
      if ((c!=' ')&&(c!='\t'))
      {
        return false;
      }
    }
  }
  return true;
}


//!Assume two Fortran OpenMP comment lines are just merged, remove" & !$omp [&]" within them
// Be careful about the confusing case
//   the 2nd & and  the end & as another continuation character
static void postProcessingMergedContinuedLine(std::string & buffer)
{
  size_t first_pos, second_pos, last_pos, next_cont_pos;
  removeFortranComments(buffer);
  // locate the first &
  first_pos = buffer.find("&");
  assert(first_pos!=string::npos);

  // locate the !$omp, must have it for OpenMP directive
  second_pos = buffer.find("$omp",first_pos);
  assert(second_pos!=string::npos);
  second_pos +=3; //shift to the end 'p' of "$omp"
  // search for the optional next '&'
  last_pos = buffer.find("&",second_pos);

  // locate the possible real cont &
  // If it is also the found optional next '&'
  // discard it as the next '&'
  if (hasFortranLineContinuation(buffer))
  {
    next_cont_pos = buffer.rfind("&");
    if (last_pos == next_cont_pos)
      last_pos = string::npos;
  }

  if (last_pos==string::npos)
    last_pos = second_pos;
  // we can now remove from first to last pos from the buffer
  buffer.erase(first_pos, last_pos - first_pos + 1);
}

void parseFortran(SgSourceFile *sageFilePtr) {

  std::vector<OpenMPDirective *> ompparser_OpenMP_pairing_list;
  std::vector<SgNode *> loc_nodes =
      NodeQuery::querySubTree(sageFilePtr, V_SgLocatedNode);
  std::vector<SgNode *>::iterator iter;
  for (iter = loc_nodes.begin(); iter != loc_nodes.end(); iter++) {
    SgLocatedNode *locNode = isSgLocatedNode(*iter);
    ROSE_ASSERT(locNode);
    AttachedPreprocessingInfoType *comments =
        locNode->getAttachedPreprocessingInfo();
    if (comments) {
      AttachedPreprocessingInfoType::iterator iter, previter = comments->end();

      for (iter = comments->begin(); iter != comments->end(); iter++) {
        PreprocessingInfo *pinfo = *iter;
        if (pinfo->getTypeOfDirective() ==
            PreprocessingInfo::FortranStyleComment) {
          string buffer = pinfo->getString();
          // Change to lower case
          std::transform(buffer.begin(), buffer.end(), buffer.begin(),
                         ::tolower);
          // We are not interested in other comments
          if (!ofs_is_omp_sentinels(buffer.c_str(), locNode)) {
            if (previter != comments->end()) {
              printf("error: Found a none-OpenMP comment after a pending "
                     "OpenMP comment with a line continuation\n");
              assert(false);
            }
            continue;
          }

          // remove possible comments also:
          removeFortranComments(buffer);
          // merge with possible previous line with &
          if (previter != comments->end()) {
            //            cout<<"previous
            //            line:"<<(*previter)->getString()<<endl;
            buffer = (*previter)->getString() + buffer;
            // remove "& !omp [&]" within the merged line
            postProcessingMergedContinuedLine(buffer);
            //            cout<<"merged line:"<<buffer<<endl;
            (*previter)->setString(""); // erase previous line with & at the end
          }

          pinfo->setString(buffer); // save the changed buffer back

          if (hasFortranLineContinuation(buffer)) {
            // delay the handling of the current line to the next line
            previter = iter;
          } else { // Now we have a line without line-continuation & , we can
                   // proceed to parse it
            previter =
                comments->end(); // clear this flag for a pending line with &
            // use ompparser to process Fortran
            ompparser_OpenMPIR = parseOpenMP(pinfo->getString().c_str(), NULL);
            ompparser_OpenMPIR->setLine(pinfo->getLineNumber());

            // set paired directives
            if (isFortranPairedDirective(ompparser_OpenMPIR)) {
              ompparser_OpenMP_pairing_list.push_back(ompparser_OpenMPIR);
            }
            if (ompparser_OpenMPIR->getKind() == OMPD_end) {
              OpenMPDirective *begin_directive =
                  ompparser_OpenMP_pairing_list.back();
              assert(((OpenMPEndDirective *)ompparser_OpenMPIR)
                         ->getPairedDirective()
                         ->getKind() == begin_directive->getKind());
              ((OpenMPEndDirective *)ompparser_OpenMPIR)
                  ->setPairedDirective(begin_directive);
              ompparser_OpenMP_pairing_list.pop_back();
            }
            fortran_omp_pragma_list.push_back(
                std::make_tuple(locNode, pinfo, ompparser_OpenMPIR));
          }
        }
      } // end for all preprocessing info
    }
  } // end for located nodes
}
