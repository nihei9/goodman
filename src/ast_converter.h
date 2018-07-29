#ifndef good_AST_CONVERTER_H
#define good_AST_CONVERTER_H

#include "ast.h"
#include "grammar.h"
#include <stdlib.h>

good_Grammar *good_new_grammar(good_ASTNode *root_node, syms_SymbolStore *syms);
void good_delete_grammar(good_Grammar *grammar);

#endif
