#ifndef good_AST_NORMALIZER_H
#define good_AST_NORMALIZER_H

#include "ast.h"
#include "collections.h"
#include <stdlib.h>

good_ASTNode *good_normalize_ast(good_ASTNodeStore *nodes, good_ASTNode *root_node, syms_SymbolStore *syms);

#endif
