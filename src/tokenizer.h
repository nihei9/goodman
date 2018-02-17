#ifndef good_TOKENIZER_H
#define good_TOKENIZER_H

#include "token.h"
#include <stdio.h>

typedef struct good_Tokenizer good_Tokenizer;

good_Tokenizer *good_new_tokenizer(FILE *target);
void good_delete_tokenizer(good_Tokenizer *tknzr);
const good_Token *good_consume_token(good_Tokenizer *tknzr);

#endif
