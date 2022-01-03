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
std::vector<std::pair<std::string, SgNode*> > acc_variable_list;
static void buildVariableList(SgOmpVariablesClause*);


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
void parseAccVariable(std::pair<SgPragmaDeclaration*, OpenACCDirective*>, OpenACCClauseKind, std::string);

SgStatement* convertOpenACCDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII) {
    printf("accparser directive is ready.\n");
    OpenACCDirectiveKind directive_kind = current_OpenACCIR_to_SageIII.second->getKind();
    SgStatement* result = NULL;

    switch (directive_kind) {
        case ACCD_parallel: {
            result = convertOpenACCBodyDirective(current_OpenACCIR_to_SageIII);
            break;
        }
        case ACCD_parallel_loop: {
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

SgUpirBodyStatement* convertOpenACCBodyDirective(std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII) {
    
    OpenACCDirectiveKind directive_kind = current_OpenACCIR_to_SageIII.second->getKind();
    // directives like parallel and for have a following code block beside the pragma itself.
    SgStatement* body = SageInterface::getNextStatement(current_OpenACCIR_to_SageIII.first);
    SageInterface::removeStatement(body,false);
    SgUpirBodyStatement* result = NULL;
    OpenACCClauseKind clause_kind;

    switch (directive_kind) {
        // TODO: insert SgUpirTaskStatement first
        case ACCD_parallel: {
            result = new SgUpirSpmdStatement(NULL, body);
            //not correct
            //should be target teams + parallel
            break;
        }
        case ACCD_parallel_loop: {
            result = new SgOmpTargetTeamsDistributeParallelForStatement(NULL, body);
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
            case ACCC_collapse:
            case ACCC_num_gangs:
            case ACCC_num_workers: {
                convertOpenACCExpressionClause(isSgUpirFieldBodyStatement(result), current_OpenACCIR_to_SageIII, *clause_iter);
                break;
            }
            case ACCC_copyin:
            case ACCC_copyout:
            case ACCC_copy: {
                convertOpenACCClause(isSgUpirFieldBodyStatement(result), current_OpenACCIR_to_SageIII, *clause_iter);
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
            result = new SgUpirNumUnitsField(clause_expression);
            printf("num_units Clause added!\n");
            break;
        }
        case ACCC_num_gangs: {            
            result = new SgOmpNumTeamsClause(clause_expression);
            printf("num_gangs Clause added!\n");
            break;
        }
        case ACCC_collapse: {            
            result = new SgOmpCollapseClause(clause_expression);
            printf("collapse Clause added!\n");
            break;
        }
        default: {
            printf("Unknown OpenACC Clause!\n");
        }
    }
    SageInterface::setOneSourcePositionForTransformation(result);

    SgOmpClause* sg_clause = result;
    ((SgUpirFieldBodyStatement*)directive)->get_clauses().push_back(sg_clause);
 
    sg_clause->set_parent(directive);

    return result;
}

SgOmpVariablesClause* convertOpenACCClause(SgStatement* directive, std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII, OpenACCClause* current_acc_clause) {
    printf("accparser variables clause is ready.\n");
    acc_variable_list.clear();
    SgOmpVariablesClause* result = NULL;
    
    SgGlobal* global = SageInterface::getGlobalScope(current_OpenACCIR_to_SageIII.first);
    OpenACCClauseKind clause_kind = current_acc_clause->getKind();
    std::vector<std::string>* current_expressions = current_acc_clause->getExpressions();
    if (current_expressions->size() != 0) {
        std::vector<std::string>::iterator iter;
        for (iter = current_expressions->begin(); iter != current_expressions->end(); iter++) {
            parseAccVariable(current_OpenACCIR_to_SageIII, current_acc_clause->getKind(), *iter);
        }
    }

    SgExprListExp* explist = SageBuilder::buildExprListExp();

    switch (clause_kind) {
        case ACCC_copy: {
            result = new SgOmpMapClause(explist, SgOmpClause::e_omp_map_tofrom);
            printf("copy Clause added!\n");
            break;
        }
        case ACCC_copyin: {
            result = new SgOmpMapClause(explist, SgOmpClause::e_omp_map_to);
            printf("copyin Clause added!\n");
            break;
        }
        case ACCC_copyout: {
            result = new SgOmpMapClause(explist, SgOmpClause::e_omp_map_from);
            printf("copyout Clause added!\n");
            break;
        }
        default: {
            printf("Unknown OpenACC Clause!\n");
        }
    }
    SageInterface::setOneSourcePositionForTransformation(result);
    buildVariableList(result);
    explist->set_parent(result);
    SgOmpClause* sg_clause = result;
    ((SgUpirFieldBodyStatement*)directive)->get_clauses().push_back(sg_clause);
 
    sg_clause->set_parent(directive);
    omp_variable_list.clear();
    return result;
}

SgExpression* parseAccExpression(SgPragmaDeclaration* directive, std::string expression) {

    // TODO: merge OpenMP and OpenACC expression parsing helpers
    bool look_forward = false;

    std::string expr_string = std::string() + "expr (" + expression + ")\n";
    SgExpression* sg_expression = parseExpression(directive, look_forward, expr_string.c_str());

    return sg_expression;
}

void parseAccVariable(std::pair<SgPragmaDeclaration*, OpenACCDirective*> current_OpenACCIR_to_SageIII, OpenACCClauseKind clause_kind, std::string expression) {
    // special handling for omp declare simd directive
    // It may have clauses referencing a variable declared in an immediately followed function's parameter list
    bool look_forward = false;
    std::string expr_string = std::string() + "varlist " + expression + "\n";
    parseExpression(current_OpenACCIR_to_SageIII.first, look_forward, expr_string.c_str());
}


void buildVariableList(SgOmpVariablesClause* current_omp_clause) {

    std::vector<std::pair<std::string, SgNode*> >::iterator iter;
    for (iter = omp_variable_list.begin(); iter != omp_variable_list.end(); iter++) {
        if (SgInitializedName* iname = isSgInitializedName((*iter).second)) {
            SgVarRefExp * var_ref = SageBuilder::buildVarRefExp(iname);
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
            std::cerr << "error: unhandled type of variable within a list:" << ((*iter).second)->class_name();
        }
    }
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
        case ACCD_parallel_loop: {
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
                case ACCC_num_gangs:
                case ACCC_collapse:
                case ACCC_copy:
                case ACCC_copyin:
                case ACCC_copyout:
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

