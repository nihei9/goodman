#ifndef good_AST_CONVERTER_H
#define good_AST_CONVERTER_H

#include "ast.h"
#include "grammar.h"
#include <stdlib.h>

const grm_Grammar *good_convert_ast_to_grammar(const good_AST *root_ast, const grm_SymbolTable *symtbl);

#endif
