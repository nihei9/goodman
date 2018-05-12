#ifndef good_TERMINAL_SYMBOL_TABLE_H
#define good_TERMINAL_SYMBOL_TABLE_H

#include "symbol_table.h"
#include <stdlib.h>

typedef struct good_TerminalSymbolTable good_TerminalSymbolTable;

good_TerminalSymbolTable *good_new_tsymtbl(void);
void good_delete_tsymtbl(good_TerminalSymbolTable *tsymtbl);
int good_put_tsym(good_TerminalSymbolTable *tsymtbl, good_SymbolID tsym_id, const char *format);
const char *good_get_tsym_format(const good_TerminalSymbolTable *tsymtbl, good_SymbolID tsym_id);

#endif
