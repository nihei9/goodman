#ifndef good_PARSER_H
#define good_PARSER_H

#include "ast.h"
#include "tokenizer.h"
#include "error.h"

typedef struct good_Parser good_Parser;

good_Parser *good_new_parser(good_Tokenizer *tknzr, good_ASTNodeStore *nodes);
void good_delete_parser(good_Parser *psr);
const good_Error *good_get_parser_error(const good_Parser *psr);
good_ASTNode *good_parse(good_Parser *psr);

#endif
