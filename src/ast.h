#ifndef good_AST_H
#define good_AST_H

#include "token.h"

typedef enum good_ASTType {
    good_AST_ROOT,
    good_AST_PRULE,
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

typedef struct good_ASTNode good_ASTNode;

typedef struct good_RootNodeBody {
    good_ASTNode *terminal_symbol;
    good_ASTNode *prule;
} good_RootNodeBody;

typedef struct good_PRuleNodeBody {
    syms_SymbolID lhs;
    good_ASTNode *rhs;
    good_ASTNode *prev;
    good_ASTNode *next;
} good_PRuleNodeBody;

typedef struct good_PRuleRHSNodeBody {
    good_ASTNode *elem;
    good_ASTNode *prev;
    good_ASTNode *next;
} good_PRuleRHSNodeBody;

typedef struct good_PRuleRHSElementNodeBody {
    syms_SymbolID symbol;
    good_QuantifierType quantifier;
    good_ASTNode *prev;
    good_ASTNode *next;
} good_PRuleRHSElementNodeBody;

struct good_ASTNode {
    good_ASTType type;

    union {
        good_RootNodeBody root;
        good_PRuleNodeBody prule;
        good_PRuleRHSNodeBody prule_rhs;
        good_PRuleRHSElementNodeBody prule_rhs_element;
    } body;

    good_ASTNode *prev;
    good_ASTNode *next;
};

typedef struct good_ASTNodeStore good_ASTNodeStore;

good_ASTNodeStore *good_new_ast_node_store();
void good_delete_ast_node_store(good_ASTNodeStore *node_store);
good_ASTNode *good_get_ast_node(good_ASTNodeStore *node_store, good_ASTType type);
void good_return_ast_node(good_ASTNodeStore *node_store, good_ASTNode *node);
int good_append_prule_node(good_ASTNode *root_node, good_ASTNode *prule_node);
int good_append_tsymbol_prule_node(good_ASTNode *root_node, good_ASTNode *prule_node);
int good_append_prule_rhs_node(good_ASTNode *prule_node, good_ASTNode *prule_rhs_node);
int good_append_prule_rhs_elem_node(good_ASTNode *prule_rhs_node, good_ASTNode *prule_rhs_elem_node);
void good_print_ast(const good_ASTNode *root_node, const syms_SymbolStore *syms);

#endif
