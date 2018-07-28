#ifndef good_AST_H
#define good_AST_H

#include "token.h"

typedef enum good_ASTType {
    good_AST_ROOT,
    good_AST_PRULE,
    good_AST_PRULE_LHS,
    good_AST_PRULE_RHS,
    good_AST_PRULE_RHS_ELEM_STRING,
    good_AST_PRULE_RHS_ELEM_SYMBOL,
} good_ASTType;

typedef enum good_QuantifierType {
    good_Q_1,
    good_Q_0_OR_1,
    good_Q_0_OR_MORE,
    good_Q_1_OR_MORE,
} good_QuantifierType;

typedef struct good_AST {
    good_ASTType type;
    good_Token token;

    good_QuantifierType quantifier;

    struct good_AST *parent;
    struct good_AST *older_brother;
    struct good_AST *brother;
    struct good_AST *child;

    size_t _num_child;
} good_AST;

good_AST *good_new_ast(good_ASTType type, const good_Token *token);
good_AST *good_new_ast_with_q(good_ASTType type, const good_Token *token, good_QuantifierType quantifier);
void good_delete_ast(good_AST *ast);
good_AST *good_append_child(good_AST *parent, good_AST *child);
int good_pop_ast(good_AST *ast);
good_AST *good_get_child(const good_AST *ast, size_t offset);
size_t good_count_child(const good_AST *ast);

#define PRULE_OFFSET 0
#define LHS_OFFSET 0
#define RHS_OFFSET 1
#define RHS_ELEM_OFFSET 0

#endif
