// Put here code used to construct SgOmp* nodes
// Liao 10/8/2010

#include "sage3basic.h"
#include "rose_paths.h"
#include "astPostProcessing.h"
#include "sageBuilder.h"
#include "accAstConstruction.h"

//#include "OpenACCIR.h"
#include <tuple>

//using namespace OmpSupport;

extern OpenACCDirective* parseOpenACC(std::string);

// the vector of pairs of OpenACC pragma and accparser IR.
extern std::vector<std::pair<SgPragmaDeclaration*, OpenACCDirective*> > OpenACCIR_list;

extern int omp_exprparser_parse();
extern SgExpression* parseExpression(SgNode*, bool, const char*);
extern SgExpression* parseArraySectionExpression(SgNode*, bool, const char*);
extern void omp_exprparser_parser_init(SgNode* aNode, const char* str);

extern bool copyStartFileInfo(SgNode*, SgNode*);
extern bool copyEndFileInfo(SgNode*, SgNode*);
extern void replaceOmpPragmaWithOmpStatement(SgPragmaDeclaration*, SgStatement*);
extern SgExpression* checkOmpExpressionClause(SgExpression*, SgGlobal*, OmpSupport::omp_construct_enum);

// TODO: Fortran OpenACC parser interface

// store temporary expression pairs for ompparser.
extern std::vector<std::pair<std::string, SgNode*> > omp_variable_list;
extern std::map<SgSymbol*, std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions;
extern SgExpression* omp_expression;
static SgExpression* parseAccExpression(SgPragmaDeclaration*, std::string);

SgStatement* convertOpenACCDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII) {
    printf("accparser directive is ready.\n");
    OpenACCDirectiveKind directive_kind = current_OpenACCIR_to_SageIII.second->getKind();
    SgStatement* result = NULL;

    switch (directive_kind) {
        case ACCD_parallel: {
            result = convertOpenACCBodyDirective(current_OpenACCIR_to_SageIII);
            break;
        }
        default: {
            printf("Unknown directive is found.\n");
        }
    }
    SageInterface::setOneSourcePositionForTransformation(result);
    copyStartFileInfo(current_OpenACCIR_to_SageIII.first, result);
    copyEndFileInfo(current_OpenACCIR_to_SageIII.first, result);
    replaceOmpPragmaWithOmpStatement(current_OpenACCIR_to_SageIII.first, result);

    return result;
}

SgUpirBaseStatement* convertOpenACCBodyDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII) {
    
    OpenACCDirectiveKind directive_kind = current_OpenACCIR_to_SageIII.second->getKind();
    // directives like parallel and for have a following code block beside the pragma itself.
    SgStatement* body = SageInterface::getNextStatement(current_OpenACCIR_to_SageIII.first);
    SageInterface::removeStatement(body,false);
    SgUpirBaseStatement* result = NULL;
    OpenACCClauseKind clause_kind;

    switch (directive_kind) {
        // TODO: insert SgUpirTaskStatement first
        case ACCD_parallel: {
            result = new SgUpirSpmdStatement(NULL, body);
            break;
        }
        default: {
            printf("Unknown directive is found.\n");
            assert(0);
        }
    }
    body->set_parent(result);
    // extract all the clauses based on the vector of clauses in the original order
    std::vector<OpenACCClause*>* all_clauses = current_OpenACCIR_to_SageIII.second->getClausesInOriginalOrder();
    std::vector<OpenACCClause*>::iterator clause_iter;
    for (clause_iter = all_clauses->begin(); clause_iter != all_clauses->end(); clause_iter++) {
        clause_kind = (*clause_iter)->getKind();
        switch (clause_kind) {
            case ACCC_num_workers: {
                convertOpenACCExpressionClause(isSgOmpClauseBodyStatement(result), current_OpenACCIR_to_SageIII, *clause_iter);
                break;
            }
            default: {
               printf("Unknown OpenACC clause is found.\n");
               assert(0);
            }
        };
    };

    return result;
}

SgOmpExpressionClause* convertOpenACCExpressionClause(SgStatement* directive, std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII, OpenACCClause* current_acc_clause) {
    printf("accparser expression clause is ready.\n");
    SgOmpExpressionClause* result = NULL;
    SgExpression* clause_expression = NULL;
    SgGlobal* global = SageInterface::getGlobalScope(current_OpenACCIR_to_SageIII.first);
    OpenACCClauseKind clause_kind = current_acc_clause->getKind();
    std::vector<std::string>* current_expressions = current_acc_clause->getExpressions();
    if (current_expressions->size() != 0) {
        std::vector<std::string>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            clause_expression = parseAccExpression(current_OpenACCIR_to_SageIII.first, (*iter).c_str());
        }
    }

    switch (clause_kind) {
        case ACCC_num_workers: {
            //SgExpression* num_units_expression = checkOmpExpressionClause(clause_expression, global, OmpSupport::e_num_threads);
            SgExpression* num_units_expression = NULL;
            result = new SgUpirNumUnitsField(num_units_expression);
            printf("num_units Clause added!\n");
            break;
        }
        default: {
            printf("Unknown OpenACC Clause!\n");
        }
    }
    SageInterface::setOneSourcePositionForTransformation(result);

    SgOmpClause* sg_clause = result;
    ((SgOmpClauseBodyStatement*)directive)->get_clauses().push_back(sg_clause);
 
    sg_clause->set_parent(directive);

    return result;
}

SgExpression* parseAccExpression(SgPragmaDeclaration* directive, std::string expression) {

    // TODO: merge OpenMP and OpenACC expression parsing helpers
    bool look_forward = false;

    std::string expr_string = std::string() + "expr (" + expression + ")\n";
    SgExpression* sg_expression = parseExpression(directive, look_forward, expr_string.c_str());

    return sg_expression;
}

bool checkOpenACCIR(OpenACCDirective* directive) {

    if (directive == NULL) {
        return false;
    };
    OpenACCDirectiveKind directive_kind = directive->getKind();
    switch (directive_kind) {
        case ACCD_parallel: {
            break;
        }
        default: {
            return false;
        }
    };
    std::map<OpenACCClauseKind, std::vector<OpenACCClause*>* >* clauses = directive->getAllClauses();
    if (clauses != NULL) {
        std::map<OpenACCClauseKind, std::vector<OpenACCClause*>* >::iterator it;
        for (it = clauses->begin(); it != clauses->end(); it++) {
            switch (it->first) {
                case ACCC_num_workers: {
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

