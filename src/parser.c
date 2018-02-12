#include "parser.h"
#include <stdlib.h>

typedef struct good_MatchingParams {
    const good_TokenType *expected_types;
    size_t expected_types_len;
    int ast_making;
    good_ASTType ast_type;
    int with_value;
} good_MatchingParams;

struct good_Parser {
    good_Tokenizer *tknzr;
};

static good_AST *good_new_ast(good_ASTType type, const good_ASTValue *value);
static good_AST *good_append_child(good_AST *parent, good_AST *child);

good_Parser *good_new_parser(good_Tokenizer *tknzr)
{
    good_Parser *psr;

    psr = (good_Parser *) malloc(sizeof (good_Parser));
    if (psr == NULL) {
        return NULL;
    }
    psr->tknzr = tknzr;

    return psr;
}

void good_delete_parser(good_Parser *psr)
{
    if (psr == NULL) {
        return;
    }

    psr->tknzr = NULL;
    free(psr);
}

const good_AST *good_parse(good_Parser *psr)
{
    good_AST *root_ast;
    const good_Token *tkn;

    root_ast = good_new_ast(good_AST_ROOT, NULL);
    if (root_ast == NULL) {
        return NULL;
    }

    do {
        tkn = good_consume_token(psr->tknzr);
        if (tkn->type == good_TKN_NEW_LINE) {
            tkn = good_consume_token(psr->tknzr);
        }
        if (tkn->type == good_TKN_EOF) {
            break;
        }

        if (tkn->type == good_TKN_NAME) {
            good_AST *prule_ast;
            good_AST *prule_lhs_ast;
            good_AST *prule_rhs_ast;

            prule_ast = good_new_ast(good_AST_PRULE, NULL);
            if (prule_ast == NULL) {
                return NULL;
            }
            good_append_child(root_ast, prule_ast);
            
            prule_lhs_ast = good_new_ast(good_AST_PRULE_LHS, (good_ASTValue *) tkn);
            if (prule_lhs_ast == NULL) {
                return NULL;
            }
            good_append_child(prule_ast, prule_lhs_ast);

            tkn = good_consume_token(psr->tknzr);
            if (tkn->type == good_TKN_NEW_LINE) {
                tkn = good_consume_token(psr->tknzr);
            }
            if (tkn->type != good_TKN_PRULE_LEADER) {
                // TODO SYNTAX ERROR
                return NULL;
            }

            do {
                prule_rhs_ast = good_new_ast(good_AST_PRULE_RHS, (good_ASTValue *) tkn);
                if (prule_rhs_ast == NULL) {
                    return NULL;
                }
                good_append_child(prule_ast, prule_rhs_ast);

                do {
                    good_AST *prule_rhs_elem_ast;

                    tkn = good_consume_token(psr->tknzr);
                    if (tkn->type == good_TKN_NAME || tkn->type == good_TKN_STRING) {
                        prule_rhs_elem_ast = good_new_ast(good_AST_PRULE_RHS_ELEM, (good_ASTValue *) tkn);
                        if (prule_rhs_elem_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(prule_rhs_ast, prule_rhs_elem_ast);
                    }
                } while (tkn->type == good_TKN_NAME || tkn->type == good_TKN_STRING);
                
                if (tkn->type != good_TKN_PRULE_OR && tkn->type != good_TKN_NEW_LINE && tkn->type != good_TKN_PRULE_TERMINATOR) {
                    // TODO SYNTAX ERROR
                    return NULL;
                }

                if (tkn->type == good_TKN_NEW_LINE) {
                    tkn = good_consume_token(psr->tknzr);
                }
            } while (tkn->type == good_TKN_PRULE_OR);

            if (tkn->type != good_TKN_PRULE_TERMINATOR) {
                // TODO SYNTAX ERROR
                return NULL;
            }
        }
        else {
            // TODO ERROR
            return NULL;
        }
    } while (tkn->type != good_TKN_EOF);

    return root_ast;
}

static good_AST *good_new_ast(good_ASTType type, const good_ASTValue *value)
{
    good_AST *ast;

    ast = (good_AST *) malloc(sizeof (good_AST));
    if (ast == NULL) {
        return NULL;
    }

    ast->type = type;
    if (value != NULL) {
        ast->value = *value;
    }
    ast->brother = NULL;
    ast->child = NULL;

    return ast;
}

static good_AST *good_append_child(good_AST *parent, good_AST *child)
{
    good_AST *youngest_child;

    if (parent == NULL) {
        return child;
    }

    if (parent->child == NULL) {
        parent->child = child;

        return parent;
    }

    youngest_child = parent->child;
    while (youngest_child->brother != NULL) {
        youngest_child = youngest_child->brother;
    }
    youngest_child->brother = child;

    return parent;
}
