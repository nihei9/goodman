#ifndef good_AST_CONVERTER_H
#define good_AST_CONVERTER_H

#include "ast.h"
#include "grammar.h"
#include <stdlib.h>

const good_Grammar *good_new_grammar(good_AST *root_ast, const syms_SymbolStore *syms);
void good_delete_grammar(good_Grammar *grammar);

#endif
