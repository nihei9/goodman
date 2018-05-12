#ifndef good_TOKENIZER_H
#define good_TOKENIZER_H

#include "token.h"
#include "symbol_table.h"
#include <stdio.h>

typedef struct good_Tokenizer good_Tokenizer;

good_Tokenizer *good_new_tokenizer(FILE *target, good_SymbolTable *symtbl);
void good_delete_tokenizer(good_Tokenizer *tknzr);
const good_Token *good_consume_token(good_Tokenizer *tknzr);

#endif
