#include "ast_converter.h"

#define PRULE_OFFSET 0
#define LHS_OFFSET 0
#define RHS_OFFSET 1
#define RHS_ELEM_OFFSET 0

#define MAX_RHS_LEN 1024

const grm_Grammar *good_convert_ast_to_grammar(const good_AST *root_ast, const grm_SymbolTable *symtbl)
{
    grm_Grammar *grm = NULL;
    const good_AST *prule_ast;

    grm = grm_new();
    if (grm == NULL) {
        goto FAILURE;
    }
    
    grm_set_default_symbol_type(grm, grm_SYMTYPE_TERMINAL);

    /*
     * 生成規則の左辺値を非終端記号として登録する。
     * それ以外（終端記号）は後続の生成規則の登録時に登録する。
     */

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const char *lhs_str;

        if (prule_ast->type != good_AST_PRULE) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            goto FAILURE;
        }
        lhs_str = grm_lookup_in_symtbl(symtbl, lhs_ast->value.token.value.symbol_id);
        if (lhs_str == NULL) {
            goto FAILURE;
        }

        grm_put_symbol_as(grm, lhs_str, grm_SYMTYPE_NON_TERMINAL);
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
        lhs_str = grm_lookup_in_symtbl(symtbl, lhs_ast->value.token.value.symbol_id);
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

                rhs_elem_str = grm_lookup_in_symtbl(symtbl, rhs_elem_ast->value.token.value.symbol_id);
                if (rhs_elem_str == NULL) {
                    goto FAILURE;
                }

                grm_put_symbol(grm, rhs_elem_str);

                rhs_elem_str_arr[rhs_len++] = rhs_elem_str;
            }

            ret = grm_append_prule(grm, lhs_str, rhs_elem_str_arr, rhs_len);
            if (ret != 0) {
                goto FAILURE;
            }
        }
    }

    return grm;

FAILURE:
    grm_delete(grm);

    return NULL;
}
