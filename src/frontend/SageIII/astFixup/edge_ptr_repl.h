#ifndef EDGE_PTR_REPL_H
#define EDGE_PTR_REPL_H

#include <map>

class SgNode;

/**
 * \brief utility functions to operate on the AST.
 */
using replacement_map_t = std::map<SgNode *, SgNode *>;

//! For all nodes in the memory pool, it looks for the edges in the replacement map. If a match is found the edge is updated.
void edgePointerReplacement(replacement_map_t const &);

//! Traverse the AST `root` looking for the edges in the replacement map. If a match is found the edge is updated.
void edgePointerReplacement(SgNode * root, replacement_map_t const &);

#endif
