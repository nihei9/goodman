#ifndef grm_GRAMMAR_H
#define grm_GRAMMAR_H

#include "symbol_table.h"
#include "production_rule_table.h"

typedef struct grm_Grammar grm_Grammar;

grm_Grammar *grm_new(void);
void grm_delete(grm_Grammar *grm);
grm_SymbolType grm_get_default_symbol_type(const grm_Grammar *grm);
grm_SymbolType grm_set_default_symbol_type(grm_Grammar *grm, grm_SymbolType type);
const grm_SymbolID *grm_put_symbol(grm_Grammar *grm, const char *symbol);
const grm_SymbolID *grm_put_symbol_as(grm_Grammar *grm, const char *symbol, grm_SymbolType type);
const char *grm_lookup_symbol(const grm_Grammar *grm, grm_SymbolID id);
grm_SymbolID grm_set_start_symbol(grm_Grammar *grm, grm_SymbolID id);
int grm_append_prule(grm_Grammar *grm, const char *lhs, const char *rhs[], size_t rhs_len);

#endif
