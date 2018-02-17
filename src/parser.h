#ifndef good_PARSER_H
#define good_PARSER_H

#include "ast.h"
#include "tokenizer.h"

typedef struct good_Parser good_Parser;

good_Parser *good_new_parser(good_Tokenizer *tknzr);
void good_delete_parser(good_Parser *psr);
const good_AST *good_parse(good_Parser *psr);

#endif
