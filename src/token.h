#ifndef good_TOKEN_H
#define good_TOKEN_H

#include "collections.h"
#include <stdlib.h>

typedef enum good_TokenType {
    good_TKN_NAME,
    good_TKN_PRULE_LEADER,      // :
    good_TKN_PRULE_OR,          // |
    good_TKN_PRULE_TERMINATOR,  // ;
    good_TKN_STRING,            // '...'
    good_TKN_OPTION,            // ?
    good_TKN_PLUS,              // +
    good_TKN_ASTERISK,          // *
    good_TKN_L_PAREN,           // (
    good_TKN_R_PAREN,           // )
    good_TKN_NEW_LINE,          // \n
    good_TKN_EOF,
} good_TokenType;

typedef struct good_Position {
    size_t row;
    size_t col;
} good_Position;

typedef struct good_Token {
    good_TokenType type;
    union {
        syms_SymbolID symbol_id;
    } value;

    good_Position pos;
} good_Token;

#endif
