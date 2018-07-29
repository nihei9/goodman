#include "ast.h"
#include <stdlib.h>

struct good_ASTNodeStore {
    good_ASTNode *used_nodes;
    good_ASTNode *free_nodes;
};

good_ASTNodeStore *good_new_ast_node_store()
{
    good_ASTNodeStore *node_store;

    node_store = (good_ASTNodeStore *) malloc(sizeof (good_ASTNodeStore));
    if (node_store == NULL) {
        return NULL;
    }
    node_store->used_nodes = NULL;
    node_store->free_nodes = NULL;

    return node_store;
}

void good_delete_ast_node_store(good_ASTNodeStore *node_store)
{
    good_ASTNode *node;
    good_ASTNode *next_node;

    if (node_store == NULL) {
        return;
    }

    node = node_store->used_nodes;
    while (node != NULL) {
        next_node = node->next;
        free(node);
        node = next_node;
    }
    node_store->used_nodes = NULL;
    node = node_store->free_nodes;
    while (node != NULL) {
        next_node = node->next;
        free(node);
        node = next_node;
    }
    node_store->free_nodes = NULL;
    free(node_store);
}

static void good_initialize_ast_node_body(good_ASTNode *node)
{
    switch (node->type) {
    case good_AST_ROOT:
        {
            good_RootNodeBody *body = &node->body.root;
            body->prule = NULL;
            body->terminal_symbol = NULL;
        }
        break;
    case good_AST_PRULE:
        {
            good_PRuleNodeBody *body = &node->body.prule;
            body->rhs = NULL;
            body->prev = NULL;
            body->next = NULL;
        }
        break;
    case good_AST_PRULE_RHS:
        {
            good_PRuleRHSNodeBody *body = &node->body.prule_rhs;
            body->elem = NULL;
            body->prev = NULL;
            body->next = NULL;
        }
        break;
    case good_AST_PRULE_RHS_ELEM_SYMBOL:
    case good_AST_PRULE_RHS_ELEM_STRING:
        {
            good_PRuleRHSElementNodeBody *body = &node->body.prule_rhs_element;
            body->quantifier = good_Q_1;
            body->prev = NULL;
            body->next = NULL;
        }
        break;
    }
}

good_ASTNode *good_get_ast_node(good_ASTNodeStore *node_store, good_ASTType type)
{
    good_ASTNode *node;

    if (node_store->free_nodes == NULL) {
        node = (good_ASTNode *) malloc(sizeof (good_ASTNode));
        if (node == NULL) {
            return NULL;
        }
        node->prev = NULL;
        node->next = NULL;
        node_store->free_nodes = node;
    }

    node = node_store->free_nodes;
    node_store->free_nodes = node->next;
    if (node->next != NULL) {
        node->next->prev = NULL;
    }

    node->prev = NULL;
    if (node_store->used_nodes != NULL) {
        node_store->used_nodes->prev = node;
    }
    node->next = node_store->used_nodes;
    node_store->used_nodes = node;

    node->type = type;
    good_initialize_ast_node_body(node);

    return node;
}

void good_return_ast_node(good_ASTNodeStore *node_store, good_ASTNode *node)
{
    // used_nodesから外す
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    else {
        node_store->used_nodes = node;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->prev = NULL;

    // free_nodesにつなぐ
    if (node_store->free_nodes != NULL) {
        node_store->free_nodes->prev = node;
    }
    node->next = node_store->free_nodes;
    node_store->used_nodes = node;
}

int good_append_prule_node(good_ASTNode *root_node, good_ASTNode *prule_node)
{
    good_RootNodeBody *root_body;

    if (root_node->type != good_AST_ROOT || prule_node->type != good_AST_PRULE) {
        return 1;
    }

    root_body = &root_node->body.root;
    if (root_body->prule == NULL) {
        root_body->prule = prule_node;

        return 0;
    }
    else {
        good_ASTNode *node;

        for (node = root_body->prule; node != NULL; node = node->body.prule.next) {
            if (node->body.prule.next == NULL) {
                prule_node->body.prule.prev = node;
                node->body.prule.next = prule_node;
                
                return 0;
            }
        }
    }

    return 1;
}

int good_append_tsymbol_prule_node(good_ASTNode *root_node, good_ASTNode *prule_node)
{
    good_RootNodeBody *root_body;

    if (root_node->type != good_AST_ROOT || prule_node->type != good_AST_PRULE) {
        return 1;
    }

    root_body = &root_node->body.root;
    if (root_body->terminal_symbol == NULL) {
        root_body->terminal_symbol = prule_node;

        return 0;
    }
    else {
        good_ASTNode *node;

        for (node = root_body->terminal_symbol; node != NULL; node = node->body.prule.next) {
            if (node->body.prule.next == NULL) {
                prule_node->body.prule.prev = node;
                node->body.prule.next = prule_node;
                
                return 0;
            }
        }
    }

    return 1;
}

int good_append_prule_rhs_node(good_ASTNode *prule_node, good_ASTNode *prule_rhs_node)
{
    good_PRuleNodeBody *prule_body;

    if (prule_node->type != good_AST_PRULE || prule_rhs_node->type != good_AST_PRULE_RHS) {
        return 1;
    }

    prule_body = &prule_node->body.prule;
    if (prule_body->rhs == NULL) {
        prule_body->rhs = prule_rhs_node;

        return 0;
    }
    else {
        good_ASTNode *node;

        for (node = prule_body->rhs; node != NULL; node = node->body.prule_rhs.next) {
            if (node->body.prule_rhs.next == NULL) {
                prule_rhs_node->body.prule_rhs.prev = node;
                node->body.prule_rhs.next = prule_rhs_node;
                
                return 0;
            }
        }
    }

    return 1;
}

int good_append_prule_rhs_elem_node(good_ASTNode *prule_rhs_node, good_ASTNode *prule_rhs_elem_node)
{
    good_PRuleRHSNodeBody *prule_rhs_body;

    if (prule_rhs_node->type != good_AST_PRULE_RHS
        || (prule_rhs_elem_node->type != good_AST_PRULE_RHS_ELEM_SYMBOL && prule_rhs_elem_node->type != good_AST_PRULE_RHS_ELEM_STRING)) {
        return 1;
    }

    prule_rhs_body = &prule_rhs_node->body.prule_rhs;
    if (prule_rhs_body->elem == NULL) {
        prule_rhs_body->elem = prule_rhs_elem_node;

        return 0;
    }
    else {
        good_ASTNode *node;

        for (node = prule_rhs_body->elem; node != NULL; node = node->body.prule_rhs_element.next) {
            if (node->body.prule_rhs_element.next == NULL) {
                prule_rhs_elem_node->body.prule_rhs_element.prev = node;
                node->body.prule_rhs_element.next = prule_rhs_elem_node;
                
                return 0;
            }
        }
    }

    return 1;
}

void good_print_ast(const good_ASTNode *root_node, const syms_SymbolStore *syms)
{
    const good_RootNodeBody *root_body;
    const good_ASTNode *prule_node;

    root_body = &root_node->body.root;
    printf("terminal_symbol\n");
    for (prule_node = root_body->terminal_symbol; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const good_PRuleNodeBody *prule_body;
        const good_ASTNode *prule_rhs_node;

        prule_body = &prule_node->body.prule;
        for (prule_rhs_node = prule_body->rhs; prule_rhs_node != NULL; prule_rhs_node = prule_rhs_node->body.prule_rhs.next) {
            const good_PRuleRHSNodeBody *prule_rhs_body;
            const good_PRuleRHSElementNodeBody *prule_rhs_elem_body;

            prule_rhs_body = &prule_rhs_node->body.prule_rhs;
            prule_rhs_elem_body = &prule_rhs_body->elem->body.prule_rhs_element;
            printf("    %s: '%s'\n", syms_lookup(syms, prule_body->lhs), syms_lookup(syms, prule_rhs_elem_body->symbol));
        }
    }
    printf("production rule\n");
    for (prule_node = root_body->prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const good_PRuleNodeBody *prule_body;
        const good_ASTNode *prule_rhs_node;

        prule_body = &prule_node->body.prule;
        for (prule_rhs_node = prule_body->rhs; prule_rhs_node != NULL; prule_rhs_node = prule_rhs_node->body.prule_rhs.next) {
            const good_PRuleRHSNodeBody *prule_rhs_body;
            const good_ASTNode *prule_rhs_elem_node;

            printf("    %s:", syms_lookup(syms, prule_body->lhs));

            prule_rhs_body = &prule_rhs_node->body.prule_rhs;
            for (prule_rhs_elem_node = prule_rhs_body->elem; prule_rhs_elem_node != NULL; prule_rhs_elem_node = prule_rhs_elem_node->body.prule_rhs_element.next) {
                const good_PRuleRHSElementNodeBody *prule_rhs_elem_body;

                prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
                if (prule_rhs_elem_node->type == good_AST_PRULE_RHS_ELEM_STRING) {
                    printf(" '%s'", syms_lookup(syms, prule_rhs_elem_body->symbol));
                }
                else {
                    printf(" %s", syms_lookup(syms, prule_rhs_elem_body->symbol));
                }
                if (prule_rhs_elem_body->quantifier == good_Q_0_OR_1) {
                    printf("?");
                }
                else if (prule_rhs_elem_body->quantifier == good_Q_0_OR_MORE) {
                    printf("*");
                }
                else if (prule_rhs_elem_body->quantifier == good_Q_1_OR_MORE) {
                    printf("+");
                }
            }
            printf("\n");
        }
    }
}
