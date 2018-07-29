#include "ast_converter.h"

#define MAX_RHS_LEN 1024

static int good_put_symbols(syms_SymbolStore *syms, syms_SymbolID *min_tsym_id, syms_SymbolID *max_tsym_id, const good_ASTNode *root_ast, const syms_SymbolStore *ast_syms);
static int good_put_prules(good_ProductionRules *prules, syms_SymbolStore *syms, const good_ASTNode *root_ast, const syms_SymbolStore *ast_syms);

good_Grammar *good_new_grammar(good_ASTNode *root_node, syms_SymbolStore *ast_syms)
{
    good_Grammar *grammar = NULL;
    syms_SymbolStore *syms = NULL;
    syms_SymbolID min_tsym_id;
    syms_SymbolID max_tsym_id;
    good_ProductionRules *prules = NULL;
    int ret;
    
    grammar = (good_Grammar *) malloc(sizeof (good_Grammar));
    if (grammar == NULL) {
        goto FAILURE;
    }

    syms = syms_new();
    if (syms == NULL) {
        goto FAILURE;
    }
    ret = good_put_symbols(syms, &min_tsym_id, &max_tsym_id, root_node, ast_syms);
    if (ret != 0) {
        goto FAILURE;
    }

    prules = good_new_prules();
    if (prules == NULL) {
        goto FAILURE;
    }
    ret = good_put_prules(prules, syms, root_node, ast_syms);
    if (ret != 0) {
        goto FAILURE;
    }

    grammar->syms = syms;
    grammar->terminal_symbol_id_from = min_tsym_id;
    grammar->terminal_symbol_id_to = max_tsym_id;
    grammar->prules = prules;
    // NOTE 暫定的に最初の生成規則の左辺値を開始記号とみなす。
    grammar->start_symbol = *syms_put(syms, syms_lookup(ast_syms, root_node->body.root.prule->body.prule.lhs));

    return grammar;

FAILURE:
    free(grammar);
    good_delete_prules(prules);
    syms_delete(syms);

    return NULL;
}

static int good_put_symbols(syms_SymbolStore *syms, syms_SymbolID *min_tsym_id, syms_SymbolID *max_tsym_id, const good_ASTNode *root_node, const syms_SymbolStore *ast_syms)
{
    const good_ASTNode *prule_node;
    int is_first = 1;

    // 終端記号の定義のみを登録
    for (prule_node = root_node->body.root.terminal_symbol; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const char *lhs_str;
        const syms_SymbolID *id;
        
        lhs_str = syms_lookup(ast_syms, prule_node->body.prule.lhs);
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
    for (prule_node = root_node->body.root.prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const char *lhs_str;
        const syms_SymbolID *id;
        
        lhs_str = syms_lookup(ast_syms, prule_node->body.prule.lhs);
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

static int good_put_prules(good_ProductionRules *prules, syms_SymbolStore *syms, const good_ASTNode *root_node, const syms_SymbolStore *ast_syms)
{
    const good_ASTNode *prule_node;

    for (prule_node = root_node->body.root.prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const good_ASTNode *rhs_node;
        const char *lhs_str;
        const syms_SymbolID *p_lhs_id;
        syms_SymbolID lhs_id;
        syms_SymbolID rhs_elem_id_arr[MAX_RHS_LEN];
        size_t rhs_len;

        lhs_str = syms_lookup(ast_syms, prule_node->body.prule.lhs);
        if (lhs_str == NULL) {
            return 1;
        }
        p_lhs_id = syms_put(syms, lhs_str);
        if (p_lhs_id == NULL) {
            return 1;
        }
        lhs_id = *p_lhs_id;
        
        for (rhs_node = prule_node->body.prule.rhs; rhs_node != NULL; rhs_node = rhs_node->body.prule_rhs.next) {
            const good_ASTNode *rhs_elem_node;
            int ret;

            rhs_len = 0;
            for (rhs_elem_node = rhs_node->body.prule_rhs.elem; rhs_elem_node != NULL; rhs_elem_node = rhs_elem_node->body.prule_rhs_element.next) {
                const char *rhs_elem_str;
                const syms_SymbolID *p_rhs_elem_id;

                if (rhs_len >= MAX_RHS_LEN) {
                    return 1;
                }

                rhs_elem_str = syms_lookup(ast_syms, rhs_elem_node->body.prule_rhs_element.symbol);
                if (rhs_elem_str == NULL) {
                    return 1;
                }
                p_rhs_elem_id = syms_put(syms, rhs_elem_str);
                if (p_rhs_elem_id == NULL) {
                    return 1;
                }

                rhs_elem_id_arr[rhs_len++] = *p_rhs_elem_id;
            }

            ret = good_append_prule(prules, lhs_id, rhs_elem_id_arr, rhs_len);
            if (ret != 0) {
                return 1;
            }
        }
    }

    return 0;
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
    free(grammar);
}
