#ifndef MAP_SCOPES_IN_TOKEN_STREAM_AND_AST
#define MAP_SCOPES_IN_TOKEN_STREAM_AND_AST

#include <functional>
#include "linearizeAST.h"
#include <vector>

struct separator
   {
     int begin_pos;
     int end_pos;

  // DQ (11/1/2016): Fixup to allow compilation with the --enable-advanced-warnings option.
#ifndef _GLIBCXX_CONCEPT_CHECKS
     std::vector<separator> sub_separators;
#endif

     separator(int bp, int ep, std::vector<separator> ss );
     separator(int bp, int ep);

     void outputValues(std::vector<SgNode*>& linearizedAST, int counter = 0 );
   };

separator* mapSeparatorsAST(std::vector<SgNode*>& linearizedAST);

#endif
