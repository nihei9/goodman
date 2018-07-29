#ifndef good_GRAMMAR_H
#define good_GRAMMAR_H

#include "production_rules.h"
#include "collections.h"

typedef struct good_Grammar {
    syms_SymbolStore *syms;
    syms_SymbolID terminal_symbol_id_from;
    syms_SymbolID terminal_symbol_id_to;
    good_ProductionRules *prules;
    syms_SymbolID start_symbol;
} good_Grammar;

#endif
