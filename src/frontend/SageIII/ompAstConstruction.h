#ifndef _OMP_AST_CONSTRUCTION
#define _OMP_AST_CONSTRUCTION

#include "AstSimpleProcessing.h"
#include "OpenACCIR.h"
#include "OpenMPIR.h"

namespace OmpSupport {
class SgVarRefExpVisitor : public AstSimpleProcessing {
private:
  std::vector<SgExpression *> expressions;

public:
  SgVarRefExpVisitor();
  std::vector<SgExpression *> get_expressions();
  void visit(SgNode *node);
};

void processOpenMP(SgSourceFile *sageFilePtr);

// Special handling to wrap statements in between "target begin" and "target
// end" OmpAttribute into a block. "target end" attribute will later be skipped
// when creating dedicate OMP statement.
void postParsingProcessing(SgSourceFile *sageFilePtr);
} // namespace OmpSupport

extern std::vector<std::pair<std::string, SgNode *>> omp_variable_list;
extern std::map<SgSymbol *,
                std::vector<std::pair<SgExpression *, SgExpression *>>>
    array_dimensions;
extern OpenMPDirective *parseOpenMP(const char *,
                                    void *_exprParse(const char *));
extern OpenACCDirective *parseOpenACC(std::string);

extern bool checkOpenACCIR(OpenACCDirective *);
extern SgStatement *convertOpenACCDirective(
    std::pair<SgPragmaDeclaration *, OpenACCDirective *>);

// Liao, 10/27/2008: parsing OpenMP pragma here
// Handle OpenMP pragmas. This should be called after preprocessing information
// is attached since macro calls may exist within pragmas, Liao, 3/31/2009
extern int omp_exprparser_parse();
extern SgExpression *parseExpression(SgNode *, bool, const char *);
extern SgExpression *parseArraySectionExpression(SgNode *, bool, const char *);
extern void omp_exprparser_parser_init(SgNode *aNode, const char *str);

// Fortran OpenMP parser interface
extern void parseOpenMPFortran(SgSourceFile *);
extern bool isFortranPairedDirective(OpenMPDirective *node);

bool checkOpenMPIR(OpenMPDirective *);
SgStatement *
getOpenMPBlockBody(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                       current_OpenMPIR_to_SageIII);
SgExpression *parseOmpArraySection(SgPragmaDeclaration *directive,
                                   OpenMPClauseKind clause_kind,
                                   std::string expression);
SgExpression *parseOmpExpression(SgPragmaDeclaration *directive,
                                 OpenMPClauseKind clause_kind,
                                 std::string expression);
void buildVariableList(SgOmpVariablesClause *current_omp_clause);
void parseOmpVariable(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClauseKind clause_kind, std::string expression);

SgOmpParallelStatement *convertOmpParallelStatementFromCombinedDirectives(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII);
SgStatement *
convertOmpTaskwaitDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII);
SgStatement *
convertOmpRequiresDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII);
SgStatement *
convertNonBodyDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII);
SgStatement *
    convertDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertVariantDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *
    convertBodyDirective(std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgOmpBodyStatement *convertCombinedBodyDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertVariantBodyDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertOmpDeclareSimdDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertOmpDeclareTargetDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertOmpEndDeclareTargetDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertOmpFlushDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertOmpAllocateDirective(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>);
SgStatement *convertOmpThreadprivateStatement(
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII);

SgOmpVariablesClause *
convertClause(SgStatement *directive,
              std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                  current_OpenMPIR_to_SageIII,
              OpenMPClause *current_omp_clause);
SgOmpWhenClause *
convertWhenClause(SgOmpClauseBodyStatement *clause_body,
                  std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                      current_OpenMPIR_to_SageIII,
                  OpenMPClause *current_omp_clause);
SgOmpBindClause *
convertBindClause(SgOmpClauseBodyStatement *clause_body,
                  std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                      current_OpenMPIR_to_SageIII,
                  OpenMPClause *current_omp_clause);
SgOmpOrderClause *
convertOrderClause(SgStatement *directive,
                   std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                       current_OpenMPIR_to_SageIII,
                   OpenMPClause *current_omp_clause);
SgOmpProcBindClause *
convertProcBindClause(SgOmpClauseBodyStatement *clause_body,
                      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClause *current_omp_clause);
SgOmpDefaultClause *
convertDefaultClause(SgOmpClauseBodyStatement *clause_body,
                     std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                         current_OpenMPIR_to_SageIII,
                     OpenMPClause *current_omp_clause);
SgOmpExpressionClause *
convertExpressionClause(SgStatement *directive,
                        std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII,
                        OpenMPClause *current_omp_clause);
SgOmpAllocatorClause *
convertAllocatorClause(SgOmpClauseStatement *clause_body,
                       std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                           current_OpenMPIR_to_SageIII,
                       OpenMPClause *current_omp_clause);
SgOmpDependClause *
convertDependClause(SgStatement *clause_body,
                    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                        current_OpenMPIR_to_SageIII,
                    OpenMPClause *current_omp_clause);
SgOmpExtImplementationDefinedRequirementClause *
convertExtImplementationDefinedRequirementClause(
    SgStatement *directive,
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII,
    OpenMPClause *current_omp_clause);
SgOmpAtomicDefaultMemOrderClause *convertAtomicDefaultMemOrderClause(
    SgStatement *directive,
    std::pair<SgPragmaDeclaration *, OpenMPDirective *>
        current_OpenMPIR_to_SageIII,
    OpenMPClause *current_omp_clause);
SgOmpDepobjUpdateClause *
convertDepobjUpdateClause(SgOmpClauseBodyStatement *clause_body,
                          std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                              current_OpenMPIR_to_SageIII,
                          OpenMPClause *current_omp_clause);
SgOmpAffinityClause *
convertAffinityClause(SgStatement *clause_body,
                      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClause *current_omp_clause);
SgOmpMapClause *
convertMapClause(SgOmpClauseBodyStatement *clause_body,
                 std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                     current_OpenMPIR_to_SageIII,
                 OpenMPClause *current_omp_clause);
SgOmpDefaultmapClause *
convertDefaultmapClause(SgOmpClauseBodyStatement *clause_body,
                        std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                            current_OpenMPIR_to_SageIII,
                        OpenMPClause *current_omp_clause);
SgOmpDistScheduleClause *
convertDistScheduleClause(SgOmpClauseBodyStatement *clause_body,
                          std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                              current_OpenMPIR_to_SageIII,
                          OpenMPClause *current_omp_clause);
SgOmpScheduleClause *
convertScheduleClause(SgStatement *directive,
                      std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                          current_OpenMPIR_to_SageIII,
                      OpenMPClause *current_omp_clause);
SgOmpUsesAllocatorsClause *
convertUsesAllocatorsClause(SgOmpClauseBodyStatement *clause_body,
                            std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                                current_OpenMPIR_to_SageIII,
                            OpenMPClause *current_omp_clause);
SgOmpFromClause *
convertFromClause(SgStatement *clause_body,
                  std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                      current_OpenMPIR_to_SageIII,
                  OpenMPClause *current_omp_clause);
SgOmpSizesClause *
convertSizesClause(SgStatement *directive,
                   std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                       current_OpenMPIR_to_SageIII,
                   OpenMPClause *current_omp_clause);
SgOmpToClause *
convertToClause(SgStatement *clause_body,
                std::pair<SgPragmaDeclaration *, OpenMPDirective *>
                    current_OpenMPIR_to_SageIII,
                OpenMPClause *current_omp_clause);
#endif
