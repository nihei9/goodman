#include "ast_normalizer.h"
#include <string.h>
#include <stdio.h>

static good_AST *good_normalize_ast_1(good_AST *root_ast, syms_SymbolStore *syms, int *num);
static good_AST *good_normalize_ast_2(good_AST *root_ast, syms_SymbolStore *syms, int *num);
static good_AST *good_rewrite_rhs_elem_ast(good_AST *root_ast, syms_SymbolID target_id, syms_SymbolID replacement_id);
static good_AST *good_new_prule_ast(syms_SymbolID lhs_id, syms_SymbolID rhs_elem_id);
static int good_is_terminal_symbol_def(const good_AST *prule_ast);
static const char *good_new_symbol_name(int num, char *buffer, size_t size);

good_AST *good_normalize_ast(good_AST *root_ast, syms_SymbolStore *syms)
{
    good_AST *ast;
    int num;

    ast = good_normalize_ast_1(root_ast, syms, &num);
    if (ast == NULL) {
        return NULL;
    }
    ast = good_normalize_ast_2(root_ast, syms, &num);
    if (ast == NULL) {
        return NULL;
    }

    return ast;
}

// 生成規則の右辺に現れる文字列を終端記号として定義する。
static good_AST *good_normalize_ast_1(good_AST *root_ast, syms_SymbolStore *syms, int *num)
{
    const good_AST *prule_ast;
    char new_lhs_str_buffer[64];
    const char *new_lhs_str;

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const good_AST *rhs_ast;

        if (good_is_terminal_symbol_def(prule_ast) != 0) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            return NULL;
        }

        for (rhs_ast = good_get_child(prule_ast, RHS_OFFSET); rhs_ast != NULL; rhs_ast = rhs_ast->brother) {
            const good_AST *rhs_elem_ast;

            for (rhs_elem_ast = good_get_child(rhs_ast, RHS_ELEM_OFFSET); rhs_elem_ast != NULL; rhs_elem_ast = rhs_elem_ast->brother) {
                good_AST *new_prule_ast;
                const syms_SymbolID *new_lhs_id;
                const good_AST *ret_ast;

                // 生成規則右辺に現れる文字列トークンは終端記号として定義する。

                if (rhs_elem_ast->token.type != good_TKN_STRING) {
                    continue;
                }

                // 定義する終端記号の名前を定義
                // 名前は $ + num の形式
                new_lhs_str = good_new_symbol_name(*num, new_lhs_str_buffer, sizeof(new_lhs_str_buffer));
                if (new_lhs_str == NULL) {
                    return NULL;
                }
                // 終端記号として名前を登録
                new_lhs_id = syms_put(syms, new_lhs_str);
                if (new_lhs_id == NULL) {
                    return NULL;
                }

                // 新たな生成規則のASTを生成
                new_prule_ast = good_new_prule_ast(*new_lhs_id, rhs_elem_ast->token.value.symbol_id);
                if (new_prule_ast == NULL) {
                    return NULL;
                }
                good_append_child(root_ast, new_prule_ast);

                // 上記で定義を追加したことによるASTの書き換え
                ret_ast = good_rewrite_rhs_elem_ast(root_ast, rhs_elem_ast->token.value.symbol_id, *new_lhs_id);
                if (ret_ast == NULL) {
                    return NULL;
                }

                *num = *num + 1;
            }
        }
    }

    return root_ast;
}

// 量指定子をもつ生成規則を、量指定子をもたない形式に変換する。
static good_AST *good_normalize_ast_2(good_AST *root_ast, syms_SymbolStore *syms, int *num)
{
    const good_AST *prule_ast;

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *lhs_ast;
        const good_AST *rhs_ast;

        if (good_is_terminal_symbol_def(prule_ast) != 0) {
            continue;
        }

        lhs_ast = good_get_child(prule_ast, LHS_OFFSET);
        if (lhs_ast == NULL) {
            return NULL;
        }

        for (rhs_ast = good_get_child(prule_ast, RHS_OFFSET); rhs_ast != NULL; rhs_ast = rhs_ast->brother) {
            good_AST *rhs_elem_ast;

            for (rhs_elem_ast = good_get_child(rhs_ast, RHS_ELEM_OFFSET); rhs_elem_ast != NULL; rhs_elem_ast = rhs_elem_ast->brother) {
                // 量指定子をもつ規則を展開する。
                if (rhs_elem_ast->quantifier == good_Q_0_OR_1) {
                    /*
                     * before
                     * aaa
                     *     : bbb ccc? ddd
                     *     ;
                     * 
                     * after
                     * aaa
                     *     : bbb ccc_or_empty ddd # cccをccc_or_emptyに置き換えて量指定子を削除
                     *     ;
                     * ccc_or_empty
                     *     : ccc
                     *     | ε
                     *     ;
                     */

                    good_Token new_lhs_tkn;

                    // オプションの記号を削除した生成規則を生成する。
                    {
                        good_AST *new_prule_ast;
                        good_AST *new_lhs_ast;
                        good_AST *new_rhs_ast;
                        good_AST *new_rhs_elem_ast;
                        const char *new_lhs_str;
                        const syms_SymbolID *new_lhs_id;
                        char new_lhs_str_buffer[64];

                        // 定義する終端記号の名前を定義
                        // 名前は $ + num の形式
                        new_lhs_str = good_new_symbol_name(*num, new_lhs_str_buffer, sizeof(new_lhs_str_buffer));
                        if (new_lhs_str == NULL) {
                            return NULL;
                        }
                        // 終端記号として名前を登録
                        new_lhs_id = syms_put(syms, new_lhs_str);
                        if (new_lhs_id == NULL) {
                            return NULL;
                        }

                        new_lhs_tkn.pos.row = 0;
                        new_lhs_tkn.pos.col = 0;
                        new_lhs_tkn.type = good_TKN_NAME;
                        new_lhs_tkn.value.symbol_id = *new_lhs_id;

                        new_prule_ast = good_new_ast(good_AST_PRULE, NULL);
                        if (new_prule_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(root_ast, new_prule_ast);

                        new_lhs_ast = good_new_ast(good_AST_PRULE_LHS, &new_lhs_tkn);
                        if (new_lhs_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_prule_ast, new_lhs_ast);

                        /*
                         * 下記の1の生成規則を追加する。
                         * 
                         * ccc_or_empty
                         *     : ccc ... 1
                         *     | ε ..... 2
                         *     ;
                         */
                        new_rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
                        if (new_rhs_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_prule_ast, new_rhs_ast);

                        new_rhs_elem_ast = good_new_ast(rhs_elem_ast->type, &rhs_elem_ast->token);
                        if (new_rhs_elem_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_rhs_ast, new_rhs_elem_ast);

                        /*
                         * 下記の2の生成規則を追加する。
                         * 
                         * ccc_or_empty
                         *     : ccc ... 1
                         *     | ε ..... 2
                         *     ;
                         */
                        new_rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
                        if (new_rhs_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_prule_ast, new_rhs_ast);
                    }

                    // オプションの記号を書き換える。
                    rhs_elem_ast->token = new_lhs_tkn;
                    rhs_elem_ast->quantifier = good_Q_1;
                }
                else if (rhs_elem_ast->quantifier == good_Q_0_OR_MORE || rhs_elem_ast->quantifier == good_Q_1_OR_MORE) {
                    /*
                     * before
                     * aaa
                     *     : bbb ccc* ddd
                     *     ;
                     * 
                     * after
                     * aaa
                     *     : bbb ccc_list ddd # cccをccc_listに置き換えて量指定子を削除
                     *     ;
                     * ccc_list               # cccの１回以上の繰り返しを生成規則として追加
                     *     : ccc
                     *     | ccc_list ccc
                     *     | ε
                     *     ;
                     * 
                     * before
                     * aaa
                     *     : bbb ccc+ ddd
                     *     ;
                     * 
                     * after
                     * aaa
                     *     : bbb ccc_list ddd # cccをccc_listに置き換えて量指定子を削除
                     *     ;
                     * ccc_list               # cccの１回以上の繰り返しを生成規則として追加
                     *     : ccc
                     *     | ccc_list ccc
                     *     ;
                     */

                    good_Token new_lhs_tkn;

                    // 繰り返し部分を抜き出した生成規則を生成する。
                    {
                        good_AST *new_prule_ast;
                        good_AST *new_lhs_ast;
                        good_AST *new_rhs_ast;
                        good_AST *new_rhs_elem_ast;
                        const char *new_lhs_str;
                        const syms_SymbolID *new_lhs_id;
                        char new_lhs_str_buffer[64];

                        // 定義する終端記号の名前を定義
                        // 名前は $ + num の形式
                        new_lhs_str = good_new_symbol_name(*num, new_lhs_str_buffer, sizeof(new_lhs_str_buffer));
                        if (new_lhs_str == NULL) {
                            return NULL;
                        }
                        // 終端記号として名前を登録
                        new_lhs_id = syms_put(syms, new_lhs_str);
                        if (new_lhs_id == NULL) {
                            return NULL;
                        }

                        new_lhs_tkn.pos.row = 0;
                        new_lhs_tkn.pos.col = 0;
                        new_lhs_tkn.type = good_TKN_NAME;
                        new_lhs_tkn.value.symbol_id = *new_lhs_id;

                        new_prule_ast = good_new_ast(good_AST_PRULE, NULL);
                        if (new_prule_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(root_ast, new_prule_ast);

                        new_lhs_ast = good_new_ast(good_AST_PRULE_LHS, &new_lhs_tkn);
                        if (new_lhs_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_prule_ast, new_lhs_ast);

                        /*
                         * 下記の1の生成規則を追加する。
                         * 
                         * ccc_list
                         *     : ccc ............ 1
                         *     | ccc_list ccc ... 2
                         *     ;
                         */
                        new_rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
                        if (new_rhs_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_prule_ast, new_rhs_ast);

                        new_rhs_elem_ast = good_new_ast(rhs_elem_ast->type, &rhs_elem_ast->token);
                        if (new_rhs_elem_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_rhs_ast, new_rhs_elem_ast);

                        /*
                         * 下記の2の生成規則を追加する。
                         * 
                         * ccc_list
                         *     : ccc ............ 1
                         *     | ccc_list ccc ... 2
                         *     ;
                         */
                        new_rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
                        if (new_rhs_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_prule_ast, new_rhs_ast);

                        new_rhs_elem_ast = good_new_ast(good_AST_PRULE_RHS_ELEM_SYMBOL, &new_lhs_tkn);
                        if (new_rhs_elem_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_rhs_ast, new_rhs_elem_ast);

                        new_rhs_elem_ast = good_new_ast(rhs_elem_ast->type, &rhs_elem_ast->token);
                        if (new_rhs_elem_ast == NULL) {
                            return NULL;
                        }
                        good_append_child(new_rhs_ast, new_rhs_elem_ast);

                        /*
                         * 下記の3の生成規則を追加する。
                         * 
                         * ccc_list
                         *     : ccc ............ 1
                         *     | ccc_list ccc ... 2
                         *     | ε .............. 3
                         *     ;
                         */
                        if (rhs_elem_ast->quantifier == good_Q_0_OR_MORE) {
                            new_rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
                            if (new_rhs_ast == NULL) {
                                return NULL;
                            }
                            good_append_child(new_prule_ast, new_rhs_ast);
                        }
                    }

                    // 繰り返し対象の記号を書き換える。
                    rhs_elem_ast->token = new_lhs_tkn;
                    rhs_elem_ast->quantifier = good_Q_1;
                }
            }
        }
    }

    return root_ast;
}

// 以下の条件をすべて満たすノードを書き換えルールのとおり書き換える。
// 
// 条件
// * 生成規則の右辺値のノードであること
// * token.type が good_TKN_STRING であること
// * token.value.symbol_id が target_id と一致すること
// 
// 書き換えルール
// * ast.type を good_AST_PRULE_RHS_ELEM_SYMBOL にする
// * token.type を good_TKN_NAME にする
// * token.value.symbol_id を replacement_id にする
static good_AST *good_rewrite_rhs_elem_ast(good_AST *root_ast, syms_SymbolID target_id, syms_SymbolID replacement_id)
{
    const good_AST *prule_ast;

    for (prule_ast = good_get_child(root_ast, PRULE_OFFSET); prule_ast != NULL; prule_ast = prule_ast->brother) {
        const good_AST *rhs_ast;

        // TODO 書き換え対象の判定は要確認
        // prule_astの左辺値がtaget_id自身の場合はどうする？（再帰定義も考慮する）
        if (good_is_terminal_symbol_def(prule_ast)) {
            continue;
        }

        for (rhs_ast = good_get_child(prule_ast, RHS_OFFSET); rhs_ast != NULL; rhs_ast = rhs_ast->brother) {
            good_AST *rhs_elem_ast;

            for (rhs_elem_ast = good_get_child(rhs_ast, RHS_ELEM_OFFSET); rhs_elem_ast != NULL; rhs_elem_ast = rhs_elem_ast->brother) {
                if (rhs_elem_ast->token.type != good_TKN_STRING) {
                    continue;
                }

                if (rhs_elem_ast->token.value.symbol_id != target_id) {
                    continue;
                }

                rhs_elem_ast->type = good_AST_PRULE_RHS_ELEM_SYMBOL;
                rhs_elem_ast->token.type = good_TKN_NAME;
                rhs_elem_ast->token.value.symbol_id = replacement_id;
            }
        }
    }

    return root_ast;
}

static good_AST *good_new_prule_ast(syms_SymbolID lhs_id, syms_SymbolID rhs_elem_id)
{
    good_AST *prule_ast = NULL;
    good_AST *lhs_ast = NULL;
    good_AST *rhs_ast = NULL;
    good_AST *rhs_elem_ast = NULL;
    good_Token lhs_token;
    good_Token rhs_elem_token;

    prule_ast = good_new_ast(good_AST_PRULE, NULL);
    if (prule_ast == NULL) {
        goto FAILURE;
    }

    lhs_token.type = good_TKN_NAME;
    lhs_token.value.symbol_id = lhs_id;
    lhs_ast = good_new_ast(good_AST_PRULE_LHS, &lhs_token);
    if (lhs_ast == NULL) {
        goto FAILURE;
    }
    good_append_child(prule_ast, lhs_ast);

    rhs_ast = good_new_ast(good_AST_PRULE_RHS, NULL);
    if (rhs_ast == NULL) {
        goto FAILURE;
    }
    good_append_child(prule_ast, rhs_ast);

    rhs_elem_token.type = good_TKN_STRING;
    rhs_elem_token.value.symbol_id = rhs_elem_id;
    rhs_elem_ast = good_new_ast(good_AST_PRULE_RHS_ELEM_STRING, &rhs_elem_token);
    if (rhs_elem_ast == NULL) {
        goto FAILURE;
    }
    good_append_child(rhs_ast, rhs_elem_ast);

    return prule_ast;

FAILURE:
    good_delete_ast(prule_ast);

    return NULL;
}

static int good_is_terminal_symbol_def(const good_AST *prule_ast)
{
    const good_AST *rhs_ast;

    if (prule_ast->type != good_AST_PRULE) {
        return 0;
    }

    // 全ての生成規則の右辺値が1つの要素のみで構成されており、かつその要素が文字列の場合は、
    // その生成規則の左辺値の記号は終端記号と判定する。
    
    rhs_ast = good_get_child(prule_ast, RHS_OFFSET);
    if (rhs_ast == NULL) {
        return 0;
    }

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

static const char *good_new_symbol_name(int num, char *buffer, size_t size)
{
    int p = 0;
    int i;
    int work_num = num;
    size_t num_str_len = 0;
    char num_str[32];

    if (buffer == NULL || size <= 0) {
        return NULL;
    }

    for (i = 0; i < 32 - 1; i++) {
        int n = work_num % 10;

        num_str[i] = n;

        work_num /= 10;
        if (work_num <= 0) {
            num_str_len = i + 1;

            break;
        }
    }
    if (num_str_len <= 0) {
        return NULL;
    }

    p = 0;
    buffer[p++] = '$';
    for (i = num_str_len - 1; i >= 0; i--) {
        buffer[p++] = '0' + num_str[i];
    }
    buffer[p] = '\0';

    return buffer;
}
