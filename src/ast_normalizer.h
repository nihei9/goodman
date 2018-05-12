#ifndef good_AST_NORMALIZER_H
#define good_AST_NORMALIZER_H

#include "ast.h"
#include "symbol_table.h"
#include <stdlib.h>

good_AST *good_normalize_ast(good_AST *root_ast, good_SymbolTable *symtbl);

#endif
