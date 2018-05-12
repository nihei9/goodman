#include "ast_converter.h"
#include "ast_normalizer.h"

#define MAX_RHS_LEN 1024

static const good_TerminalSymbolTable *good_new_tsymtbl_from_ast(const good_AST *root_ast, const good_SymbolTable *symtbl);
static const grm_Grammar *good_new_prtbl_from_ast(const good_AST *root_ast, const good_SymbolTable *symtbl);
static int good_is_terminal_symbol_def(const good_AST *prule_ast);

const good_Grammar *good_new_grammar_from_ast(const good_AST *root_ast, const good_SymbolTable *symtbl)
{
    good_Grammar *grammar = NULL;
    const good_TerminalSymbolTable *tsymtbl = NULL;
    const grm_Grammar *prtbl = NULL;
    const good_AST *ret_ast;
    
    ret_ast = good_normalize_ast(root_ast, symtbl);
    if (ret_ast == NULL) {
        goto FAILURE;
    }

    grammar = (good_Grammar *) malloc(sizeof (good_Grammar));
    if (grammar == NULL) {
        goto FAILURE;
    }

    tsymtbl = good_new_tsymtbl_from_ast(root_ast, symtbl);
    if (tsymtbl == NULL) {
        goto FAILURE;
    }

    prtbl = good_new_prtbl_from_ast(root_ast, symtbl);
    if (prtbl == NULL) {
        goto FAILURE;
    }

    grammar->tsymtbl = tsymtbl;
    grammar->prtbl = prtbl;

    return grammar;

FAILURE:
    free(grammar);
    good_delete_tsymtbl((good_TerminalSymbolTable *) tsymtbl);
    grm_delete((grm_Grammar *) prtbl);

    return NULL;
}

static const good_TerminalSymbolTable *good_new_tsymtbl_from_ast(const good_AST *root_ast, const good_SymbolTable *symtbl)
{
    good_TerminalSymbolTable *tsymtbl = NULL;
    const good_AST *prule_ast;

    tsymtbl = good_new_tsymtbl();
    if (tsymtbl == NULL) {
        goto FAILURE;
    }

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const good_AST *rhs_ast;

        if (!good_is_terminal_symbol_def(prule_ast)) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            goto FAILURE;
        }

        for (rhs_ast = good_get_child(prule_ast, RHS_OFFSET); rhs_ast != NULL; rhs_ast = rhs_ast->brother) {
            const good_AST *rhs_elem_ast;
            const char *rhs_elem_str;
            int ret;

            rhs_elem_ast = good_get_child(rhs_ast, RHS_ELEM_OFFSET);
            if (rhs_elem_ast == NULL) {
                goto FAILURE;
            }

            rhs_elem_str = good_lookup_in_symtbl(symtbl, rhs_elem_ast->token.value.symbol_id);
            if (rhs_elem_str == NULL) {
                goto FAILURE;
            }

            ret = good_put_tsym(tsymtbl, lhs_ast->token.value.symbol_id, rhs_elem_str);
            if (ret != 0) {
                goto FAILURE;
            }
        }
    }

    return tsymtbl;

FAILURE:
    good_delete_tsymtbl(tsymtbl);

    return NULL;
}

static const grm_Grammar *good_new_prtbl_from_ast(const good_AST *root_ast, const good_SymbolTable *symtbl)
{
    grm_Grammar *prtbl = NULL;
    const good_AST *prule_ast;

    prtbl = grm_new();
    if (prtbl == NULL) {
        goto FAILURE;
    }
    
    grm_set_default_symbol_type(prtbl, good_SYMTYPE_TERMINAL);

    /*
     * 生成規則の左辺値を非終端記号として登録する。
     * それ以外（終端記号）は後続の生成規則の登録時に登録する。
     */

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const char *lhs_str;

        if (good_is_terminal_symbol_def(prule_ast)) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            goto FAILURE;
        }
        lhs_str = good_lookup_in_symtbl(symtbl, lhs_ast->token.value.symbol_id);
        if (lhs_str == NULL) {
            goto FAILURE;
        }

        grm_put_symbol_as(prtbl, lhs_str, good_SYMTYPE_NON_TERMINAL);
    }

    /*
     * 生成規則を登録する。
     * 合わせて終端記号の登録も行う。
     */

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const good_AST *rhs_ast;
        const char *lhs_str;
        const char *rhs_elem_str_arr[MAX_RHS_LEN];
        size_t rhs_len;

        if (prule_ast->type != good_AST_PRULE) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            goto FAILURE;
        }
        lhs_str = good_lookup_in_symtbl(symtbl, lhs_ast->token.value.symbol_id);
        if (lhs_str == NULL) {
            goto FAILURE;
        }

        for (rhs_ast = good_get_child(prule_ast, RHS_OFFSET); rhs_ast != NULL; rhs_ast = rhs_ast->brother) {
            const good_AST *rhs_elem_ast;
            int ret;

            rhs_len = 0;
            for (rhs_elem_ast = good_get_child(rhs_ast, RHS_ELEM_OFFSET); rhs_elem_ast != NULL; rhs_elem_ast = rhs_elem_ast->brother) {
                const char *rhs_elem_str;

                if (rhs_len >= MAX_RHS_LEN) {
                    goto FAILURE;
                }

                rhs_elem_str = good_lookup_in_symtbl(symtbl, rhs_elem_ast->token.value.symbol_id);
                if (rhs_elem_str == NULL) {
                    goto FAILURE;
                }

                grm_put_symbol(prtbl, rhs_elem_str);

                rhs_elem_str_arr[rhs_len++] = rhs_elem_str;
            }

            ret = grm_append_prule(prtbl, lhs_str, rhs_elem_str_arr, rhs_len);
            if (ret != 0) {
                goto FAILURE;
            }
        }
    }

    return prtbl;

FAILURE:
    grm_delete(prtbl);
    
    return NULL;
}

void good_delete_grammar(good_Grammar *grammar)
{
    if (grammar == NULL) {
        return;
    }

    good_delete_tsymtbl((good_TerminalSymbolTable *) grammar->tsymtbl);
    grammar->tsymtbl = NULL;
    grm_delete((grm_Grammar *) grammar->prtbl);
    grammar->prtbl = NULL;
    free(grammar);
}

static int good_is_terminal_symbol_def(const good_AST *prule_ast)
{
    const good_AST *rhs_ast;

    if (prule_ast->type != good_AST_PRULE) {
        return 0;
    }

    rhs_ast = good_get_child(prule_ast, RHS_OFFSET);
    if (rhs_ast == NULL) {
        return 0;
    }

    // 全ての生成規則の右辺値が1つの要素のみで構成されており、かつその要素が文字列の場合は、
    // その生成規則の左辺値の記号は終端記号と判定する。
    while (rhs_ast != NULL) {
        const good_AST *elem;

        if (good_count_child(rhs_ast) > 1) {
            return 0;
        }

        elem = good_get_child(rhs_ast, RHS_ELEM_OFFSET);
        if (elem->type != good_AST_PRULE_RHS_ELEM_STRING) {
            return 0;
        }

        rhs_ast = rhs_ast->brother;
    }
    
    return 1;
}
