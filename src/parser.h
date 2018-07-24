#ifndef good_PARSER_H
#define good_PARSER_H

#include "ast.h"
#include "tokenizer.h"
#include "error.h"

/*
 * AST構造
 * 
 * <good_AST_ROOT>
 *     +-- <good_AST_PRULE>
 *     |       +-- <good_AST_LHS>
 *     |       +-- <good_AST_RHS>
 *     |       |       +-- <good_AST_RHS_ELEM_STRING | good_AST_RHS_ELEM_SYMBOL>
 *     |       |       `-- ...
 *     |       `-- ...
 *     `-- ...
 */

typedef struct good_Parser good_Parser;

good_Parser *good_new_parser(good_Tokenizer *tknzr);
void good_delete_parser(good_Parser *psr);
const good_Error *good_get_parser_error(const good_Parser *psr);
const good_AST *good_parse(good_Parser *psr);

#endif
