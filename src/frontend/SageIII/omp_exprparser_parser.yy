/* OpenMP C and C++ Grammar */
/* Author: Markus Schordan, 2003 */
/* Modified by Christian Biesinger 2006 for OpenMP 2.0 */
/* Modified by Chunhua Liao for OpenMP 3.0 and connect to OmpAttribute, 2008 */
/* Updated by Chunhua Liao for OpenMP 4.5,  2017 */

/*
To debug bison conflicts, use the following command line in the build tree

/bin/sh ../../../../sourcetree/config/ylwrap ../../../../sourcetree/src/frontend/Sab.h `echo expression_parser.cc | sed -e s/cc$/hh/ -e s/cpp$/hpp/ -e s/cxx$/hxx/ -e s/c++$/h++/ -e s/c$/h/` y.output expression_parser.output -- bison -y -d -r state
in the build tree
*/
%define api.prefix {omp_exprparser_}
%defines
%define parse.error verbose

%{
/* DQ (2/10/2014): IF is conflicting with Boost template IF. */
#undef IF

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "sage3basic.h" // Sage Interface and Builders
#include "sageBuilder.h"

#ifdef _MSC_VER
  #undef IN
  #undef OUT
  #undef DUPLICATE
#endif

using namespace OmpSupport;
using namespace SageInterface;

/* Parser - BISON */

/*the scanner function*/
extern int omp_exprparser_lex();

/*A customized initialization function for the scanner, str is the string to be scanned.*/
extern void omp_exprparser_lexer_init(const char* str);

//! Initialize the parser with the originating SgPragmaDeclaration and its pragma text
extern void omp_exprparser_parser_init(SgNode* aNode, const char* str);
extern SgExpression* parseExpression(SgNode*, const char*);
extern SgExpression* parseArraySectionExpression(SgNode*, const char*);

static int omp_exprparser_error(const char*);

// The context node with the pragma annotation being parsed
//
// We attach the attribute to the pragma declaration directly for now, 
// A few OpenMP directive does not affect the next structure block
// This variable is set by the prefix_parser_init() before prefix_parse() is called.
//Liao
static SgNode* omp_directive_node;

static const char* orig_str; 

// The current expression node being generated 
static SgExpression* current_exp = NULL;
// a flag to indicate if the program is looking forward in the symbol table
static bool omp_exprparser_look_forward = false;

// We now follow the OpenMP 4.0 standard's C-style array section syntax: [lower-bound:length] or just [length]
// the latest variable symbol being parsed, used to help parsing the array dimensions associated with array symbol
// such as a[0:n][0:m]
static SgVariableSymbol* array_symbol; 
static SgExpression* lower_exp = NULL;
static SgExpression* length_exp = NULL;
// check if the parsed a[][] is an array element access a[i][j] or array section a[lower:length][lower:length]
// 
static bool arraySection=true; 

// mark whether it is for ompparser
static bool is_ompparser_variable = false;
static bool is_ompparser_expression = false;
// add ompparser var
static bool addOmpVariable(const char*);
std::vector<std::pair<std::string, SgNode*> > omp_variable_list;
std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions;  
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
        VARLIST ARRAY_SECTION
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
                  | omp_array_section
                  ;

omp_varlist : VARLIST {
                    is_ompparser_variable = true;
                    } variable_list { is_ompparser_variable = false; }
               ;

omp_expression : EXPRESSION {
                is_ompparser_expression = true;
            } '(' expression ')' {
                is_ompparser_expression = false;
            }
            ;

omp_array_section : ARRAY_SECTION {
                      is_ompparser_expression = true;
                  } '(' array_section_list ')' {
                      is_ompparser_expression = false;
                  }
                  ;

array_section_list : id_expression_opt_dimension
                   | array_section_list ',' id_expression_opt_dimension
                   ;

/* mapped variables may have optional dimension information */
id_expression_opt_dimension : ID_EXPRESSION { if (!addOmpVariable((const char*)$1)) YYABORT; } dimension_field_optseq
                            | '*' ID_EXPRESSION { if (!addOmpVariable((const char*)$2)) YYABORT; } dimension_field_optseq
                            ;

/* Parse optional dimension information associated with map(a[0:n][0:m]) Liao 1/22/2013 */
dimension_field_optseq : /* empty */
                       | dimension_field_seq
                       ;
/* sequence of dimension fields */
dimension_field_seq : dimension_field
                    | dimension_field_seq dimension_field
                    ;

dimension_field : '[' expression {lower_exp = current_exp; } 
                  ':' expression { length_exp = current_exp;
                       assert (array_symbol != NULL);
                       SgType* t = array_symbol->get_type();
                       bool isPointer= (isSgPointerType(t) != NULL );
                       bool isArray= (isSgArrayType(t) != NULL);
                       if (!isPointer && ! isArray )
                       {
                         std::cerr<<"Error. ompparser.yy expects a pointer or array type."<<std::endl;
                         std::cerr<<"while seeing "<<t->class_name()<<std::endl;
                       }
                       array_dimensions[array_symbol].push_back( std::make_pair (lower_exp, length_exp));
                       } 
                   ']'
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
                 (const char*)($1),SageInterface::getScope(omp_directive_node)
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
                 //array_symbol = ompattribute->addVariable(omptype, vref->unparseToString());
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
               //ompattribute->array_dimensions[array_symbol].push_back( std::make_pair (lower_exp, length_exp));
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
                std::cout << "Got expression: " << $1 << "\n";
                addOmpVariable((const char*)$1);
              }
              | variable_list ',' ID_EXPRESSION {
                std::cout << "Got expression: " << $3 << "\n";
                addOmpVariable((const char*)$3);
              }

%%
int yyerror(const char *s) {
    SgLocatedNode* lnode = isSgLocatedNode(omp_directive_node);
    assert (lnode);
    printf("Error when parsing pragma:\n\t %s \n\t associated with node at line %d\n", orig_str, lnode->get_file_info()->get_line()); 
    printf(" %s!\n", s);
    assert(0);
    return 0; // we want to the program to stop on error
}

void omp_exprparser_parser_init(SgNode* directive, const char* str) {
    orig_str = str;
    omp_exprparser_lexer_init(str);
    omp_directive_node = directive;
}

// Grab all explicit? variables declared within a common block and add them into the omp variable list
static void ofs_add_block_variables (const char* block_name)
{
  std::vector<SgCommonBlock*> block_vec = SageInterface::getSgNodeListFromMemoryPool<SgCommonBlock>();
  SgCommonBlockObject* found_block_object = NULL;
  for (std::vector<SgCommonBlock*>::const_iterator i = block_vec.begin();
       i!= block_vec.end();i++)
  {
    bool innerbreak = false;
    SgCommonBlock* c_block = *i;
    SgCommonBlockObjectPtrList & blockList = c_block->get_block_list();
    SgCommonBlockObjectPtrList::iterator i2 = blockList.begin();
    while (i2 != blockList.end())
    {
      std::string name = (*i2)->get_block_name();
      if (strcmp(block_name, name.c_str())==0)
      {
        found_block_object = *i2;
        innerbreak = true;
        break;
      }
      i2++;
    }// end while block objects

    if (innerbreak)
      break;
  } // end for all blocks

  if (found_block_object == NULL)
  {
    printf("error: cannot find a common block with a name of %s\n",block_name);
    ROSE_ABORT();
  }

  // add each variable within the block into ompattribute
  SgExprListExp * explistexp = found_block_object->get_variable_reference_list ();
  assert(explistexp != NULL);
  SgExpressionPtrList& explist = explistexp->get_expressions();

  Rose_STL_Container<SgExpression*>::const_iterator exp_iter = explist.begin();
  assert (explist.size()>0); // must have variable defined
  while (exp_iter !=explist.end())
  {
    SgVarRefExp * var_exp = isSgVarRefExp(*exp_iter);
    assert (var_exp!=NULL);
    SgVariableSymbol * symbol = isSgVariableSymbol(var_exp->get_symbol());
    assert (symbol!=NULL);
    SgInitializedName* sgvar = symbol->get_declaration();
    const char* var = sgvar->get_name().getString().c_str();
    if (sgvar != NULL) {
        symbol = isSgVariableSymbol(sgvar->get_symbol_from_symbol_table());
    };
    omp_variable_list.push_back(std::make_pair(var, sgvar));
    exp_iter++;
  }
}

static bool addOmpVariable(const char* var)  {

    // if the leading symbol is '/', it is a block name in Fortran
    if (var[0] == '/') {
        std::string block_name = std::string(var);
        block_name.pop_back();
        ofs_add_block_variables(block_name.c_str()+1);
        return true;
    }

    SgInitializedName* sgvar = NULL;
    SgVariableSymbol* symbol = NULL;
    SgScopeStatement* scope = NULL;

    if (omp_exprparser_look_forward != true) {
        scope = SageInterface::getScope(omp_directive_node);
    }
    else {
        SgStatement* cur_stmt = getEnclosingStatement(omp_directive_node);
        ROSE_ASSERT (isSgPragmaDeclaration(cur_stmt));

        // omp declare simd may show up several times before the impacted function declaration.
        SgStatement* nstmt = getNextStatement(cur_stmt);
        ROSE_ASSERT (nstmt); // must have next statement followed.
        // skip possible multiple pragma declarations
        while (!isSgFunctionDeclaration(nstmt)) {
            nstmt = getNextStatement (nstmt);
            ROSE_ASSERT (nstmt);
        };
        // At this point, it must be a function declaration
        SgFunctionDeclaration* func = isSgFunctionDeclaration(nstmt);
        ROSE_ASSERT (func);
        SgFunctionDefinition* def = func->get_definition();
        scope = def->get_body();
    };

    ROSE_ASSERT(scope != NULL);
    symbol = lookupVariableSymbolInParentScopes(var, scope);
    sgvar = symbol->get_declaration();
    if (sgvar != NULL) {
        symbol = isSgVariableSymbol(sgvar->get_symbol_from_symbol_table());
    };
    omp_variable_list.push_back(std::make_pair(var, sgvar));
    array_symbol = symbol;
    return true;
}

SgExpression* parseArraySectionExpression(SgNode* directive, bool look_forward, const char* str) {
    orig_str = str;
    omp_exprparser_lexer_init(str);
    omp_directive_node = directive;
    omp_exprparser_look_forward = look_forward;
    omp_exprparser_parse();
    SgExpression* sg_expression = current_exp;

    return sg_expression;
}

SgExpression* parseExpression(SgNode* directive, bool look_forward, const char* str) {
    orig_str = str;
    omp_exprparser_lexer_init(str);
    omp_directive_node = directive;
    omp_exprparser_look_forward = look_forward;
    omp_exprparser_parse();
    SgExpression* sg_expression = current_exp;

    return sg_expression;
}
