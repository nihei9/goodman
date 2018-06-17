#ifndef good_AST_CONVERTER_H
#define good_AST_CONVERTER_H

#include "ast.h"
#include "terminal_symbol_table.h"
#include "grammar.h"
#include "collections.h"
#include <stdlib.h>

typedef struct good_Grammar {
    const good_TerminalSymbolTable *tsymtbl;
    const grm_Grammar *prtbl;
} good_Grammar;

const good_Grammar *good_new_grammar(good_AST *root_ast, const syms_SymbolStore *syms);
void good_delete_grammar(good_Grammar *grammar);

#endif
