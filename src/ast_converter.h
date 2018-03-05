#ifndef good_AST_CONVERTER_H
#define good_AST_CONVERTER_H

#include "ast.h"
#include "terminal_symbol_table.h"
#include "grammar.h"
#include <stdlib.h>

typedef struct good_Grammar {
    const good_TerminalSymbolTable *tsymtbl;
    const grm_Grammar *prtbl;
} good_Grammar;

const good_Grammar *good_new_grammar_from_ast(const good_AST *root_ast, const grm_SymbolTable *symtbl);
void good_delete_grammar(good_Grammar *grammar);

#endif
