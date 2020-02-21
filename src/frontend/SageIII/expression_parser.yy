/* OpenMP C and C++ Grammar */
/* Author: Markus Schordan, 2003 */
/* Modified by Christian Biesinger 2006 for OpenMP 2.0 */
/* Modified by Chunhua Liao for OpenMP 3.0 and connect to OmpAttribute, 2008 */
/* Updated by Chunhua Liao for OpenMP 4.5,  2017 */

/*
To debug bison conflicts, use the following command line in the build tree

/bin/sh ../../../../sourcetree/config/ylwrap ../../../../sourcetree/src/frontend/Sab.h `echo ompparser.cc | sed -e s/cc$/hh/ -e s/cpp$/hpp/ -e s/cxx$/hxx/ -e s/c++$/h++/ -e s/c$/h/` y.output ompparser.output -- bison -y -d -r state
in the build tree
*/
%name-prefix "omp_"
%defines
%error-verbose

%{
/* DQ (2/10/2014): IF is conflicting with Boost template IF. */
#undef IF

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "sage3basic.h" // Sage Interface and Builders
#include "sageBuilder.h"
#include "OmpAttribute.h"

#ifdef _MSC_VER
  #undef IN
  #undef OUT
  #undef DUPLICATE
#endif

using namespace OmpSupport;
using namespace SageInterface;

/* Parser - BISON */

/*the scanner function*/
extern int omp_lex(); 

/*A customized initialization function for the scanner, str is the string to be scanned.*/
extern void omp_lexer_init(const char* str);

//! Initialize the parser with the originating SgPragmaDeclaration and its pragma text
extern void omp_parser_init(SgNode* aNode, const char* str);

//The result AST representing the annotation
extern OmpAttribute* getParsedDirective();

static int omp_error(const char*);

//Insert variable into var_list of some clause
static bool addVar(const char* var);


// The current AST annotation being built
static OmpAttribute* ompattribute = NULL;

// The current OpenMP construct or clause type which is being parsed
// It is automatically associated with the current ompattribute
// Used to indicate the OpenMP directive or clause to which a variable list or an expression should get added for the current OpenMP pragma being parsed.
static omp_construct_enum omptype = e_unknown;

// the clause where variables will be added.
static ComplexClause* current_clause;
// The node to which vars/expressions should get added
//static OmpAttribute* omptype = 0;

// The context node with the pragma annotation being parsed
//
// We attach the attribute to the pragma declaration directly for now, 
// A few OpenMP directive does not affect the next structure block
// This variable is set by the prefix_parser_init() before prefix_parse() is called.
//Liao
static SgNode* gNode;

static const char* orig_str; 

// The current expression node being generated 
static SgExpression* current_exp = NULL;
bool b_within_variable_list  = false;  // a flag to indicate if the program is now processing a list of variables

// We now follow the OpenMP 4.0 standard's C-style array section syntax: [lower-bound:length] or just [length]
// the latest variable symbol being parsed, used to help parsing the array dimensions associated with array symbol
// such as a[0:n][0:m]
static SgVariableSymbol* array_symbol; 
static SgExpression* lower_exp = NULL;
static SgExpression* length_exp = NULL;
// check if the parsed a[][] is an array element access a[i][j] or array section a[lower:length][lower:length]
// 
static bool arraySection=true; 

// mark whether it is complex clause.
static bool is_complex_clause = false;

static bool addComplexVar(const char* var);

// mark whether it is for ompparser
static bool is_ompparser_variable = false;
static bool is_ompparser_expression = false;
// add ompparser var
static bool addOmpVariable(const char*);
std::vector<std::pair<std::string, SgNode*> > omp_variable_list;
static bool addOmpExpression(const char*);
SgExpression* omp_expression = NULL;
%}

%locations

/* The %union declaration specifies the entire collection of possible data types for semantic values. these names are used in the %token and %type declarations to pick one of the types for a terminal or nonterminal symbol
corresponding C type is union name defaults to YYSTYPE.
*/

%union {  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        }

/*Some operators have a suffix 2 to avoid name conflicts with ROSE's existing types, We may want to reuse them if it is proper. 
  experimental BEGIN END are defined by default, we use TARGET_BEGIN TARGET_END instead. 
  Liao*/
%token  '(' ')' ',' ':' '+' '*' '-' '&' '^' '|' LOGAND LOGOR SHLEFT SHRIGHT PLUSPLUS MINUSMINUS PTR_TO '.'
        LE_OP2 GE_OP2 EQ_OP2 NE_OP2 RIGHT_ASSIGN2 LEFT_ASSIGN2 ADD_ASSIGN2
        SUB_ASSIGN2 MUL_ASSIGN2 DIV_ASSIGN2 MOD_ASSIGN2 AND_ASSIGN2 
        XOR_ASSIGN2 OR_ASSIGN2 DEPEND IN OUT INOUT MERGEABLE
        LEXICALERROR IDENTIFIER MIN MAX
        VARLIST
/*We ignore NEWLINE since we only care about the pragma string , We relax the syntax check by allowing it as part of line continuation */
%token <itype> ICONSTANT   
%token <stype> EXPRESSION ID_EXPRESSION 

/* associativity and precedence */
%left '<' '>' '=' "!=" "<=" ">="
%left '+' '-'
%left '*' '/' '%'

/* nonterminals names, types for semantic values, only for nonterminals representing expressions!! not for clauses with expressions.
 */
%type <ptype> expression assignment_expr conditional_expr 
              logical_or_expr logical_and_expr
              inclusive_or_expr exclusive_or_expr and_expr
              equality_expr relational_expr 
              shift_expr additive_expr multiplicative_expr 
              primary_expr unary_expr postfix_expr

/* start point for the parsing */
%start openmp_expression

%%

/* NOTE: We can't use the EXPRESSION lexer token directly. Instead, we have
 * to first call omp_parse_expr, because we parse up to the terminating
 * paren.
 */

openmp_expression : omp_varlist
                  | omp_expression
                  ;

omp_varlist : VARLIST {
                    is_ompparser_variable = true;
                    omptype = e_unknown; 
                    cur_omp_directive = omptype; b_within_variable_list = true;} variable_list {b_within_variable_list = false; is_ompparser_variable = false; }
               ;

omp_expression : EXPRESSION {
                is_ompparser_expression = true;
                omptype = e_unknown;
                b_within_variable_list = true;
            } '(' expression ')' {
                addOmpExpression("");
                is_ompparser_expression = false;
                b_within_variable_list = false;
            }
            ;

/* Sara Royuela, 04/27/2012
 * Extending grammar to accept conditional expressions, arithmetic and bitwise expressions and member accesses
 */
expression : assignment_expr

assignment_expr : conditional_expr
                | logical_or_expr 
                | unary_expr '=' assignment_expr  {
                    current_exp = SageBuilder::buildAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr RIGHT_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildRshiftAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr LEFT_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildLshiftAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr ADD_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildPlusAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr SUB_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildMinusAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr MUL_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildMultAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr DIV_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildDivAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr MOD_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildModAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr AND_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildAndAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr XOR_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildXorAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr OR_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildIorAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                ;

conditional_expr : logical_or_expr '?' assignment_expr ':' assignment_expr {
                     current_exp = SageBuilder::buildConditionalExp(
                       (SgExpression*)($1),
                       (SgExpression*)($3),
                       (SgExpression*)($5)
                     );
                     $$ = current_exp;
                   }
                 ;

logical_or_expr : logical_and_expr
                | logical_or_expr LOGOR logical_and_expr {
                    current_exp = SageBuilder::buildOrOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    );
                    $$ = current_exp;
                  }
                ;

logical_and_expr : inclusive_or_expr
                 | logical_and_expr LOGAND inclusive_or_expr {
                     current_exp = SageBuilder::buildAndOp(
                       (SgExpression*)($1),
                       (SgExpression*)($3)
                     );
                   $$ = current_exp;
                 }
                 ;

inclusive_or_expr : exclusive_or_expr
                  | inclusive_or_expr '|' exclusive_or_expr {
                      current_exp = SageBuilder::buildBitOrOp(
                        (SgExpression*)($1),
                        (SgExpression*)($3)
                      );
                      $$ = current_exp;
                    }
                  ;

exclusive_or_expr : and_expr
                  | exclusive_or_expr '^' and_expr {
                      current_exp = SageBuilder::buildBitXorOp(
                        (SgExpression*)($1),
                        (SgExpression*)($3)
                      );
                      $$ = current_exp;
                    }
                  ;

and_expr : equality_expr
         | and_expr '&' equality_expr {
             current_exp = SageBuilder::buildBitAndOp(
               (SgExpression*)($1),
               (SgExpression*)($3)
             );
             $$ = current_exp;
           }
         ;  

equality_expr : relational_expr
              | equality_expr EQ_OP2 relational_expr {
                  current_exp = SageBuilder::buildEqualityOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp;
                }
              | equality_expr NE_OP2 relational_expr {
                  current_exp = SageBuilder::buildNotEqualOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp;
                }
              ;
              
relational_expr : shift_expr
                | relational_expr '<' shift_expr { 
                    current_exp = SageBuilder::buildLessThanOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp; 
                  // std::cout<<"debug: buildLessThanOp():\n"<<current_exp->unparseToString()<<std::endl;
                  }
                | relational_expr '>' shift_expr {
                    current_exp = SageBuilder::buildGreaterThanOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp; 
                  }
                | relational_expr LE_OP2 shift_expr {
                    current_exp = SageBuilder::buildLessOrEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp; 
                  }
                | relational_expr GE_OP2 shift_expr {
                    current_exp = SageBuilder::buildGreaterOrEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    );
                    $$ = current_exp; 
                  }
                ;

shift_expr : additive_expr
           | shift_expr SHRIGHT additive_expr {
               current_exp = SageBuilder::buildRshiftOp(
                 (SgExpression*)($1),
                 (SgExpression*)($3)
               ); 
               $$ = current_exp; 
             }
           | shift_expr SHLEFT additive_expr {
               current_exp = SageBuilder::buildLshiftOp(
                 (SgExpression*)($1),
                 (SgExpression*)($3)
               ); 
               $$ = current_exp; 
             }
           ;

additive_expr : multiplicative_expr
              | additive_expr '+' multiplicative_expr {
                  current_exp = SageBuilder::buildAddOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp; 
                }
              | additive_expr '-' multiplicative_expr {
                  current_exp = SageBuilder::buildSubtractOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp; 
                }
              ;

multiplicative_expr : primary_expr
                    | multiplicative_expr '*' primary_expr {
                        current_exp = SageBuilder::buildMultiplyOp(
                          (SgExpression*)($1),
                          (SgExpression*)($3)
                        ); 
                        $$ = current_exp; 
                      }
                    | multiplicative_expr '/' primary_expr {
                        current_exp = SageBuilder::buildDivideOp(
                          (SgExpression*)($1),
                          (SgExpression*)($3)
                        ); 
                        $$ = current_exp; 
                      }
                    | multiplicative_expr '%' primary_expr {
                        current_exp = SageBuilder::buildModOp(
                          (SgExpression*)($1),
                          (SgExpression*)($3)
                        ); 
                        $$ = current_exp; 
                      }
                    ;

primary_expr : ICONSTANT {
               current_exp = SageBuilder::buildIntVal($1);
               $$ = current_exp;
              }
             | ID_EXPRESSION {
               current_exp = SageBuilder::buildVarRefExp(
                 (const char*)($1),SageInterface::getScope(gNode)
               );
               $$ = current_exp;
              }
             | '(' expression ')' {
                 $$ = current_exp;
               } 
             ;

unary_expr : postfix_expr {
             current_exp = (SgExpression*)($1);
             $$ = current_exp;
            }  
           |PLUSPLUS unary_expr {
              current_exp = SageBuilder::buildPlusPlusOp(
                (SgExpression*)($2),
                SgUnaryOp::prefix
              );
              $$ = current_exp;
            }
          | MINUSMINUS unary_expr {
              current_exp = SageBuilder::buildMinusMinusOp(
                (SgExpression*)($2),
                SgUnaryOp::prefix
              );
              $$ = current_exp;
            }

           ;
/* Follow ANSI-C yacc grammar */                
postfix_expr:primary_expr {
               arraySection= false; 
                 current_exp = (SgExpression*)($1);
                 $$ = current_exp;
             }
            |postfix_expr '[' expression ']' {
               arraySection= false; 
               current_exp = SageBuilder::buildPntrArrRefExp((SgExpression*)($1), (SgExpression*)($3));
               $$ = current_exp;
             }
            | postfix_expr '[' expression ':' expression ']'
             {
               arraySection= true; // array section // TODO; BEST solution: still need a tree here!!
               // only add  symbol to the attribute for this first time 
               // postfix_expr should be ID_EXPRESSION
               if (!array_symbol)
               {  
                 SgVarRefExp* vref = isSgVarRefExp((SgExpression*)($1));
                 assert (vref);
                 array_symbol = ompattribute->addVariable(omptype, vref->unparseToString());
                 // if (!addVar((const char*) )) YYABORT;
                 //std::cout<<("!array_symbol, add variable for \n")<< vref->unparseToString()<<std::endl;
               }
               lower_exp= NULL; 
               length_exp= NULL; 
               lower_exp = (SgExpression*)($3);
               length_exp = (SgExpression*)($5);
               assert (array_symbol != NULL);
               SgType* t = array_symbol->get_type();
               bool isPointer= (isSgPointerType(t) != NULL );
               bool isArray= (isSgArrayType(t) != NULL);
               if (!isPointer && ! isArray )
               {
                 std::cerr<<"Error. ompparser.yy expects a pointer or array type."<<std::endl;
                 std::cerr<<"while seeing "<<t->class_name()<<std::endl;
               }
               assert (lower_exp && length_exp);
               ompattribute->array_dimensions[array_symbol].push_back( std::make_pair (lower_exp, length_exp));
             }  
            | postfix_expr PLUSPLUS {
                  current_exp = SageBuilder::buildPlusPlusOp(
                    (SgExpression*)($1),
                    SgUnaryOp::postfix
                  ); 
                  $$ = current_exp; 
                }
             | postfix_expr MINUSMINUS {
                  current_exp = SageBuilder::buildMinusMinusOp(
                    (SgExpression*)($1),
                    SgUnaryOp::postfix
                  ); 
                  $$ = current_exp; 
             }
            ;

/* ----------------------end for parsing expressions ------------------*/

/*  in C
variable-list : identifier
              | variable-list , identifier 
*/

/* in C++ (we use the C++ version) */ 
variable_list : ID_EXPRESSION {
              if (is_complex_clause) {
                addComplexVar((const char*)$1);
              }
              else if (is_ompparser_variable) {
                std::cout << "Got expression: " << $1 << "\n";
                addOmpVariable((const char*)$1);
              }
              else {
                if (!addVar((const char*)$1)) {
                    YYABORT;
                };
              }
            }
              | variable_list ',' ID_EXPRESSION {
              if (is_complex_clause) {
                addComplexVar((const char*)$3);
              }
              else if (is_ompparser_variable) {
                std::cout << "Got expression: " << $3 << "\n";
                addOmpVariable((const char*)$3);
              }
              else {
                if (!addVar((const char*)$3)) {
                    YYABORT;
                };
              }
            }

%%
int yyerror(const char *s) {
    SgLocatedNode* lnode = isSgLocatedNode(gNode);
    assert (lnode);
    printf("Error when parsing pragma:\n\t %s \n\t associated with node at line %d\n", orig_str, lnode->get_file_info()->get_line()); 
    printf(" %s!\n", s);
    assert(0);
    return 0; // we want to the program to stop on error
}


OmpAttribute* getParsedDirective() {
    return ompattribute;
}

void omp_parser_init(SgNode* aNode, const char* str) {
    orig_str = str;  
    omp_lexer_init(str);
    gNode = aNode;
}

static bool addVar(const char* var)  {
    array_symbol = ompattribute->addVariable(omptype,var);
    return true;
}

static bool addComplexVar(const char* var)  {
    array_symbol = ompattribute->addComplexClauseVariable(current_clause, var);
    return true;
}

static bool addOmpVariable(const char* var)  {
    SgInitializedName* sgvar = NULL;
    SgVariableSymbol* symbol = NULL;
    SgScopeStatement* scope = SageInterface::getScope(gNode);
    ROSE_ASSERT(scope != NULL);
    symbol = lookupVariableSymbolInParentScopes (var, scope);
    sgvar = symbol->get_declaration();
    if (sgvar != NULL) {
        symbol = isSgVariableSymbol(sgvar->get_symbol_from_symbol_table());
    };
    omp_variable_list.push_back(std::make_pair(var, sgvar));
    return true;
}


static bool addOmpExpression(const char* expr) {
    assert (current_exp != NULL);
    omp_expression = current_exp;
    omp_expression->set_parent(gNode);
    return true;
}


