#include "parser.h"
#include <stdlib.h>

struct good_Parser {
    good_Tokenizer *tknzr;

    good_Error error;
};

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

const good_Error *good_get_parser_error(const good_Parser *psr)
{
    return &psr->error;
}

const good_AST *good_parse(good_Parser *psr)
{
    good_AST *root_ast = NULL;
    const good_Token *tkn;

    root_ast = good_new_ast(good_AST_ROOT, NULL);
    if (root_ast == NULL) {
        goto ERROR;
    }

    do {
        tkn = good_consume_token(psr->tknzr);
        if (tkn == NULL) {
            psr->error = *good_get_tokenizer_error(psr->tknzr);
            goto ERROR;
        }
        while (tkn->type == good_TKN_NEW_LINE) {
            tkn = good_consume_token(psr->tknzr);
            if (tkn == NULL) {
                psr->error = *good_get_tokenizer_error(psr->tknzr);
                goto ERROR;
            }
        }
        if (tkn->type == good_TKN_EOF) {
            break;
        }

        if (tkn->type == good_TKN_NAME) {
            good_AST *prule_ast;
            good_AST *prule_lhs_ast;

            prule_ast = good_new_ast(good_AST_PRULE, NULL);
            if (prule_ast == NULL) {
                goto ERROR;
            }
            good_append_child(root_ast, prule_ast);
            
            prule_lhs_ast = good_new_ast(good_AST_PRULE_LHS, tkn);
            if (prule_lhs_ast == NULL) {
                goto ERROR;
            }
            good_append_child(prule_ast, prule_lhs_ast);

            tkn = good_consume_token(psr->tknzr);
            if (tkn == NULL) {
                psr->error = *good_get_tokenizer_error(psr->tknzr);
                goto ERROR;
            }
            while (tkn->type == good_TKN_NEW_LINE) {
                tkn = good_consume_token(psr->tknzr);
                if (tkn == NULL) {
                    psr->error = *good_get_tokenizer_error(psr->tknzr);
                    goto ERROR;
                }
            }
            if (tkn->type != good_TKN_PRULE_LEADER) {
                psr->error.code = good_ERR_MISSING_PRULE_LEADER;
                goto ERROR;
            }

            do {
                good_AST *prule_rhs_ast;

                prule_rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
                if (prule_rhs_ast == NULL) {
                    goto ERROR;
                }
                good_append_child(prule_ast, prule_rhs_ast);

                do {
                    good_AST *prule_rhs_elem_ast;

                    tkn = good_consume_token(psr->tknzr);
                    if (tkn == NULL) {
                        psr->error = *good_get_tokenizer_error(psr->tknzr);
                        goto ERROR;
                    }
                    if (tkn->type == good_TKN_NAME || tkn->type == good_TKN_STRING) {
                        good_ASTType type;

                        if (tkn->type == good_TKN_NAME) {
                            type = good_AST_PRULE_RHS_ELEM_SYMBOL;
                        }
                        else {
                            type = good_AST_PRULE_RHS_ELEM_STRING;
                        }

                        prule_rhs_elem_ast = good_new_ast(type, tkn);
                        if (prule_rhs_elem_ast == NULL) {
                            goto ERROR;
                        }
                        good_append_child(prule_rhs_ast, prule_rhs_elem_ast);
                    }
                } while (tkn->type == good_TKN_NAME || tkn->type == good_TKN_STRING);
                
                if (tkn->type != good_TKN_PRULE_OR && tkn->type != good_TKN_NEW_LINE && tkn->type != good_TKN_PRULE_TERMINATOR) {
                    if (tkn->type == good_TKN_EOF) {
                        psr->error.code = good_ERR_UNTERMINATED_PRULE;
                    }
                    else {
                        psr->error.code = good_ERR_UNEXPECTED_TOKEN_IN_PRULE;
                    }
                    goto ERROR;
                }

                while (tkn->type == good_TKN_NEW_LINE) {
                    tkn = good_consume_token(psr->tknzr);
                    if (tkn == NULL) {
                        psr->error = *good_get_tokenizer_error(psr->tknzr);
                        goto ERROR;
                    }
                }
            } while (tkn->type == good_TKN_PRULE_OR);

            if (tkn->type != good_TKN_PRULE_TERMINATOR) {
                psr->error.code = good_ERR_UNTERMINATED_PRULE;
                goto ERROR;
            }
        }
        else {
            // TODO ERROR
            goto ERROR;
        }
    } while (tkn->type != good_TKN_EOF);

    return root_ast;

ERROR:
    good_delete_ast(root_ast);

    return NULL;
}
