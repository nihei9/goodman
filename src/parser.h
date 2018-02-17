#ifndef good_PARSER_H
#define good_PARSER_H

#include "tokenizer.h"

typedef enum good_ASTType {
    good_AST_ROOT,
    good_AST_PRULE,
    good_AST_PRULE_LHS,
    good_AST_PRULE_RHS,
    good_AST_PRULE_RHS_ELEM,
} good_ASTType;

typedef union good_ASTValue {
    good_Token token;
} good_ASTValue;

typedef struct good_AST {
    good_ASTType type;
    good_ASTValue value;

    struct good_AST *brother;
    struct good_AST *child;
} good_AST;

typedef struct good_Parser good_Parser;

good_Parser *good_new_parser(good_Tokenizer *tknzr);
void good_delete_parser(good_Parser *psr);
const good_AST *good_parse(good_Parser *psr);

#endif
