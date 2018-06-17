#include "ast_converter.h"
#include "ast_normalizer.h"

#define MAX_RHS_LEN 1024

static int good_put_symbols(syms_SymbolStore *syms, syms_SymbolID *min_tsym_id, syms_SymbolID *max_tsym_id, const good_AST *root_ast, const syms_SymbolStore *ast_syms);
static int good_put_prules(good_ProductionRules *prules, syms_SymbolStore *syms, const good_AST *root_ast, const syms_SymbolStore *ast_syms);
static const grm_Grammar *good_new_prtbl_from_ast(const good_AST *root_ast, const syms_SymbolStore *syms);
static int good_is_terminal_symbol_def(const good_AST *prule_ast);

const good_Grammar *good_new_grammar(good_AST *root_ast, const syms_SymbolStore *ast_syms)
{
    good_Grammar *grammar = NULL;
    syms_SymbolStore *syms = NULL;
    syms_SymbolID min_tsym_id;
    syms_SymbolID max_tsym_id;
    good_ProductionRules *prules = NULL;
    int ret;
    const grm_Grammar *prtbl = NULL;
    const good_AST *ret_ast;
    
    ret_ast = good_normalize_ast(root_ast, ast_syms);
    if (ret_ast == NULL) {
        goto FAILURE;
    }

    grammar = (good_Grammar *) malloc(sizeof (good_Grammar));
    if (grammar == NULL) {
        goto FAILURE;
    }

    syms = syms_new();
    if (syms == NULL) {
        goto FAILURE;
    }
    ret = good_put_symbols(syms, &min_tsym_id, &max_tsym_id, root_ast, ast_syms);
    if (ret != 0) {
        goto FAILURE;
    }

    prules = good_new_prules();
    if (prules == NULL) {
        goto FAILURE;
    }
    ret = good_put_prules(prules, syms, root_ast, ast_syms);
    if (ret != 0) {
        return 1;
    }

    prtbl = good_new_prtbl_from_ast(root_ast, ast_syms);
    if (prtbl == NULL) {
        goto FAILURE;
    }

    grammar->syms = syms;
    grammar->prules = prules;
    grammar->prtbl = prtbl;

    return grammar;

FAILURE:
    free(grammar);
    good_delete_prules(prules);
    syms_delete(syms);
    grm_delete((grm_Grammar *) prtbl);

    return NULL;
}

static int good_put_symbols(syms_SymbolStore *syms, syms_SymbolID *min_tsym_id, syms_SymbolID *max_tsym_id, const good_AST *root_ast, const syms_SymbolStore *ast_syms)
{
    const good_AST *prule_ast;
    int is_first = 1;

    // 終端記号の定義のみを登録
    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const char *lhs_str;
        const syms_SymbolID *id;

        if (!good_is_terminal_symbol_def(prule_ast)) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            return 1;
        }

        lhs_str = syms_lookup(ast_syms, lhs_ast->token.value.symbol_id);
        if (lhs_str == NULL) {
            return 1;
        }

        id = syms_put(syms, lhs_str);
        if (id == NULL) {
            return 1;
        }

        if (is_first) {
            *min_tsym_id = *id;
            is_first = 0;
        }
        *max_tsym_id = *id;
    }

    // 非終端記号の定義のみを登録
    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const char *lhs_str;
        const syms_SymbolID *id;

        if (good_is_terminal_symbol_def(prule_ast)) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            return 1;
        }

        lhs_str = syms_lookup(ast_syms, lhs_ast->token.value.symbol_id);
        if (lhs_str == NULL) {
            return 1;
        }

        id = syms_put(syms, lhs_str);
        if (id == NULL) {
            return 1;
        }
    }

    return 0;
}

static int good_put_prules(good_ProductionRules *prules, syms_SymbolStore *syms, const good_AST *root_ast, const syms_SymbolStore *ast_syms)
{
    const good_AST *prule_ast;

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const good_AST *rhs_ast;
        const char *lhs_str;
        const syms_SymbolID *lhs_id;
        syms_SymbolID rhs_elem_id_arr[MAX_RHS_LEN];
        size_t rhs_len;

        if (prule_ast->type != good_AST_PRULE) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            return 1;
        }
        lhs_str = syms_lookup(ast_syms, lhs_ast->token.value.symbol_id);
        if (lhs_str == NULL) {
            return 1;
        }
        lhs_id = syms_put(syms, lhs_str);
        if (lhs_id == NULL) {
            return 1;
        }

        for (rhs_ast = good_get_child(prule_ast, RHS_OFFSET); rhs_ast != NULL; rhs_ast = rhs_ast->brother) {
            const good_AST *rhs_elem_ast;
            int ret;

            rhs_len = 0;
            for (rhs_elem_ast = good_get_child(rhs_ast, RHS_ELEM_OFFSET); rhs_elem_ast != NULL; rhs_elem_ast = rhs_elem_ast->brother) {
                const char *rhs_elem_str;
                const syms_SymbolID *rhs_elem_id;

                if (rhs_len >= MAX_RHS_LEN) {
                    return 1;
                }

                rhs_elem_str = syms_lookup(ast_syms, rhs_elem_ast->token.value.symbol_id);
                if (rhs_elem_str == NULL) {
                    return 1;
                }
                rhs_elem_id = syms_put(syms, rhs_elem_str);
                if (rhs_elem_id == NULL) {
                    return 1;
                }

                rhs_elem_id_arr[rhs_len++] = rhs_elem_id;
            }

            ret = good_append_prule(prules, *lhs_id, rhs_elem_id_arr, rhs_len);
            if (ret != 0) {
                return 1;
            }
        }
    }

    return 0;
}

static const grm_Grammar *good_new_prtbl_from_ast(const good_AST *root_ast, const syms_SymbolStore *syms)
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
        lhs_str = syms_lookup(syms, lhs_ast->token.value.symbol_id);
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
        lhs_str = syms_lookup(syms, lhs_ast->token.value.symbol_id);
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

                rhs_elem_str = syms_lookup(syms, rhs_elem_ast->token.value.symbol_id);
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

    good_delete_prules(grammar->prules);
    grammar->prules = NULL;
    syms_delete(grammar->syms);
    grammar->syms = NULL;
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
