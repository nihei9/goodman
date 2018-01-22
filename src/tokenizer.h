#ifndef good_TOKENIZER_H
#define good_TOKENIZER_H

#include "symbol_table.h"
#include <stdio.h>

typedef enum good_TokenType {
    good_TKN_NAME,
    good_TKN_STRING,
    good_TKN_INDENT,
    good_TKN_NEW_LINE,
} good_TokenType;

typedef struct good_Token {
    good_TokenType type;
    union {
        unsigned int uint_value;
        grm_SymbolID symbol_id;
    } value;

    struct {
        size_t row;
        size_t col;
    } pos;
} good_Token;

typedef struct good_Tokenizer good_Tokenizer;

good_Tokenizer *good_new_tokenizer(FILE *target);
void good_delete_tokenizer(good_Tokenizer *tknzr);
const good_Token *good_consume_token(good_Tokenizer *tknzr);

#endif
