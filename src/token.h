#ifndef good_TOKEN_H
#define good_TOKEN_H

#include "symbol_table.h"
#include <stdlib.h>

typedef enum good_TokenType {
    good_TKN_NAME,
    good_TKN_PRULE_LEADER,
    good_TKN_PRULE_OR,
    good_TKN_PRULE_TERMINATOR,
    good_TKN_STRING,
    good_TKN_NEW_LINE,
    good_TKN_EOF,
} good_TokenType;

typedef struct good_Position {
    size_t row;
    size_t col;
} good_Position;

typedef struct good_Token {
    good_TokenType type;
    union {
        good_SymbolID symbol_id;
    } value;

    good_Position pos;
} good_Token;

#endif
