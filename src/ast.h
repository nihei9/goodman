#ifndef good_AST_H
#define good_AST_H

#include "token.h"

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

#endif
