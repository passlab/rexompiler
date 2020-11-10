%option prefix="omp_"
%option outfile="lex.yy.c"
%option stack
%option nounput
%option noyy_top_state
%option noyy_pop_state
%option noyy_push_state


%{

/* DQ (12/10/2016): This is a technique to suppress warnings in generated code that we want to be an error elsewhere in ROSE. 
   See https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html for more detail.
 */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

/* lex requires me to use extern "C" here */
extern "C" int omp_wrap() { return 1; }

extern int omp_lex();

#include <stdio.h>
#include <string>
#include <string.h>
#include "expression_parser.hh"

static const char* ompparserinput = NULL;
static std::string gExpressionString;

/* pass user specified string to buf, indicate the size using 'result', 
   and shift the current position pointer of user input afterwards 
   to prepare next round of token recognition!!
*/
#define YY_INPUT(buf, result, max_size) { \
                if (*ompparserinput == '\0') result = 0; \
                else { strncpy(buf, ompparserinput, max_size); \
                        buf[max_size] = 0; \
                        result = strlen(buf); \
                        ompparserinput += result; \
                } \
                }

%}

blank           [ ]
newline         [\n]
digit           [0-9]

id              [a-zA-Z_][a-zA-Z0-9_]*

%%
{digit}{digit}* { omp_lval.itype = atoi(strdup(yytext)); return (ICONSTANT); }

"="             { return ('='); }
"("             { return ('('); }
")"             { return (')'); }
"["             { return ('['); }
"]"             { return (']'); }
","             { return (','); }
":"             { return (':'); }
"+"             { return ('+'); }
"*"             { return ('*'); }
"-"             { return ('-'); }
"&"             { return ('&'); }
"^"             { return ('^'); }
"|"             { return ('|'); }
"&&"            { return (LOGAND); }
"||"            { return (LOGOR); }
"<<"            { return (SHLEFT); }
">>"            { return (SHRIGHT); }
"++"            { return (PLUSPLUS); }
"--"            { return (MINUSMINUS); }

">>="           { return (RIGHT_ASSIGN2); }
"<<="           { return (LEFT_ASSIGN2); }
"+="            { return (ADD_ASSIGN2); }
"-="            { return (SUB_ASSIGN2); }
"*="            { return (MUL_ASSIGN2); }
"/="            { return (DIV_ASSIGN2); }
"%="            { return (MOD_ASSIGN2); }
"&="            { return (AND_ASSIGN2); }
"^="            { return (XOR_ASSIGN2); }
"|="            { return (OR_ASSIGN2); }

"<"             { return ('<'); }
">"             { return ('>'); }
"<="            { return (LE_OP2);}
">="            { return (GE_OP2);}
"=="            { return (EQ_OP2);}
"!="            { return (NE_OP2);}
"\\"            { /*printf("found a backslash\n"); This does not work properly but can be ignored*/}

"->"            { return (PTR_TO); }
"."             { return ('.'); }

{newline}       { /* printf("found a new line\n"); */ /* return (NEWLINE); We ignore NEWLINE since we only care about the pragma string , We relax the syntax check by allowing it as part of line continuation */ }

expr            { return (EXPRESSION); }
varlist         { return (VARLIST); }
identifier      { return (IDENTIFIER); /*not in use for now*/ }
array_section   { return (ARRAY_SECTION); }
{id}            { omp_lval.stype = strdup(yytext); 
                  return (ID_EXPRESSION); }

{blank}*        ;
.               { return (LEXICALERROR);}

%%


/* entry point invoked by callers to start scanning for a string */
extern void omp_lexer_init(const char* str) {
  ompparserinput = str;
  /* We have omp_ suffix for all flex functions */
  omp_restart(omp_in);
}
/**
 * @file
 * Lexer for OpenMP-pragmas.
 */

