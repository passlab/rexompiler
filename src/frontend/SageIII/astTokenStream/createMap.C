// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "createMap.h"
#include <algorithm>
using namespace std;

void
createMap::buildMaps()
   {
#ifndef USE_ROSE
#endif
   }

void
createMap::internalMatchBetweenASTandTokenStreamSeparator(separator*
                                                          ast, separator* token){

#ifndef USE_ROSE
#endif
}

//get the linearized AST provided in the constructor
std::vector<SgNode*>&               createMap::get_linearizedAST()
{
    return linearizedAST;
};

//the integer int refers to an index in the tokenStream vector
std::map<SgNode*, std::pair<int,int> > createMap::get_mapFromNodeToToken()
{
    return nodeToTokenMap;

};
std::map<std::pair<int,int>,SgNode*> createMap::get_mapFromTokenToNode()
{
    return tokenToNodeMap;
};

void 
createMap::printNodeToTokenMap()
   {
#ifndef USE_ROSE
#endif
   }

void 
createMap::printTokenToNodeMap()
   {
  // Nothing implemented here.
   }

