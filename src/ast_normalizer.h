#ifndef good_AST_NORMALIZER_H
#define good_AST_NORMALIZER_H

#include "ast.h"
#include "collections.h"
#include <stdlib.h>

good_AST *good_normalize_ast(good_AST *root_ast, syms_SymbolStore *syms);

#endif
