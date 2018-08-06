#include "ast_normalizer.h"
#include "logger.h"
#include <string.h>
#include <stdio.h>

// prule_node が終端記号の定義を表すか否かを判定する。
// 全ての生成規則の右辺値が1つの要素のみで構成されており、かつその要素が文字列の場合は、
// その生成規則の左辺値の記号は終端記号と判定する。
static int good_is_tsymbol_node(const good_ASTNode *prule_node)
{
    const good_ASTNode *rhs_node;
    const good_PRuleNodeBody *prule_body;

    prule_body = &prule_node->body.prule;

    if (prule_body->rhs == NULL) {
        return 0;
    }

    for (rhs_node = prule_body->rhs; rhs_node != NULL; rhs_node = rhs_node->body.prule_rhs.next) {
        const good_ASTNode *elem_node;
        const good_PRuleRHSElementNodeBody *elem_body;

        // 空規則を含む場合は非終端記号の定義と判定する。
        if (rhs_node->body.prule_rhs.elem == NULL) {
            return 0;
        }

        elem_node = rhs_node->body.prule_rhs.elem;
        elem_body = &elem_node->body.prule_rhs_element;

        // 要素が２つ以上の右辺をもつ場合は非終端記号の定義と判定する。
        if (elem_body->next != NULL) {
            return 0;
        }

        // 文字列以外が右辺に含まれる場合は非終端記号の定義と判定する。
        if (elem_node->type != good_AST_PRULE_RHS_ELEM_STRING) {
            return 0;
        }
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

static good_ASTNode *good_new_tsymbol_prule_node(good_ASTNodeStore *nodes, syms_SymbolID lhs_id, syms_SymbolID rhs_elem_id)
{
    good_ASTNode *prule_node = NULL;
    good_ASTNode *rhs_node = NULL;
    good_ASTNode *rhs_elem_node = NULL;

    prule_node = good_get_ast_node(nodes, good_AST_PRULE);
    if (prule_node == NULL) {
        return NULL;
    }
    prule_node->body.prule.lhs = lhs_id;

    rhs_node = good_get_ast_node(nodes, good_AST_PRULE_RHS);
    if (rhs_node == NULL) {
        return NULL;
    }
    prule_node->body.prule.rhs = rhs_node;

    rhs_elem_node = good_get_ast_node(nodes, good_AST_PRULE_RHS_ELEM_STRING);
    if (rhs_elem_node == NULL) {
        return NULL;
    }
    rhs_elem_node->body.prule_rhs_element.symbol = rhs_elem_id;
    rhs_elem_node->body.prule_rhs_element.quantifier = good_Q_1;
    rhs_node->body.prule_rhs.elem = rhs_elem_node;

    return prule_node;
}

// 以下の条件をすべて満たすノードを書き換えルールのとおり書き換える。
// 
// 条件
// * 生成規則の右辺値のノードであること
// * token.type が good_TKN_STRING であること
// * token.value.symbol_id が target_id と一致すること
// 
// 書き換えルール
// * ast.type を good_ASTNode_PRULE_RHS_ELEM_SYMBOL にする
// * token.type を good_TKN_NAME にする
// * token.value.symbol_id を replacement_id にする
static good_ASTNode *good_rewrite_rhs_elem_node(good_ASTNode *root_node, syms_SymbolID target_id, syms_SymbolID replacement_id)
{
    const good_ASTNode *prule_node;

    for (prule_node = root_node->body.root.prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const good_ASTNode *rhs_node;

        // TODO 書き換え対象の判定は要確認
        // prule_nodeの左辺値がtaget_id自身の場合はどうする？（再帰定義も考慮する）

        for (rhs_node = prule_node->body.prule.rhs; rhs_node != NULL; rhs_node = rhs_node->body.prule_rhs.next) {
            good_ASTNode *rhs_elem_node;

            for (rhs_elem_node = rhs_node->body.prule_rhs.elem; rhs_elem_node != NULL; rhs_elem_node = rhs_elem_node->body.prule_rhs_element.next) {
                good_PRuleRHSElementNodeBody *rhs_elem_body;

                if (rhs_elem_node->type != good_AST_PRULE_RHS_ELEM_STRING) {
                    continue;
                }

                rhs_elem_body = &rhs_elem_node->body.prule_rhs_element;

                if (rhs_elem_body->symbol != target_id) {
                    continue;
                }

                rhs_elem_node->type = good_AST_PRULE_RHS_ELEM_SYMBOL;
                rhs_elem_body->symbol = replacement_id;
            }
        }
    }

    return root_node;
}

// グルーピングされた生成規則を独立した生成規則として切り出す。
static good_ASTNode *good_normalize_ast_1(good_ASTNodeStore *nodes, good_ASTNode *root_node, syms_SymbolStore *syms, int *num)
{
    good_ASTNode *prule_node;
    good_RootNodeBody *root_body;

    root_body = &root_node->body.root;

    for (prule_node = root_body->prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        good_PRuleNodeBody *prule_body;
        good_ASTNode *prule_rhs_node;

        if (good_is_tsymbol_node(prule_node)) {
            continue;
        }

        prule_body = &prule_node->body.prule;

        for (prule_rhs_node = prule_body->rhs; prule_rhs_node != NULL; prule_rhs_node = prule_rhs_node->body.prule_rhs.next) {
            good_PRuleRHSNodeBody *prule_rhs_body;
            good_ASTNode *prule_rhs_elem_node;

            prule_rhs_body = &prule_rhs_node->body.prule_rhs;

            for (prule_rhs_elem_node = prule_rhs_body->elem; prule_rhs_elem_node != NULL; prule_rhs_elem_node = prule_rhs_elem_node->body.prule_rhs_element.next) {
                good_PRuleRHSElementNodeBody *prule_rhs_elem_body;

                if (prule_rhs_elem_node->type != good_AST_PRULE_RHS_ELEM_GROUP) {
                    continue;
                }

                prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;

                // グループを生成規則として切り出す。
                {
                    char new_lhs_str_buffer[64];
                    const char *new_lhs_str;
                    const syms_SymbolID *new_lhs_id;
                    good_ASTNode *new_prule_node;

                    new_lhs_str = good_new_symbol_name(*num, new_lhs_str_buffer, sizeof(new_lhs_str_buffer));
                    if (new_lhs_str == NULL) {
                        return NULL;
                    }
                    new_lhs_id = syms_put(syms, new_lhs_str);
                    if (new_lhs_id == NULL) {
                        return NULL;
                    }

                    // 新たな生成規則のASTを生成
                    new_prule_node = good_get_ast_node(nodes, good_AST_PRULE);
                    if (new_prule_node == NULL) {
                        return NULL;
                    }
                    new_prule_node->body.prule.lhs = *new_lhs_id;

                    // グループの右辺を新たに作成した生成規則の要素として追加する。
                    good_append_prule_rhs_node(new_prule_node, prule_rhs_elem_body->rhs);
                    
                    // グループとして解析された要素を書き換える。
                    prule_rhs_elem_node->type = good_AST_PRULE_RHS_ELEM_SYMBOL;
                    prule_rhs_elem_body->symbol = *new_lhs_id;
                    prule_rhs_elem_body->rhs = NULL;

                    // 新たに作成した生成規則をroot配下に追加する。
                    good_append_prule_node(root_node, new_prule_node);

                    *num = *num + 1;
                }
            }
        }
    }

    return root_node;
}

// 生成規則と終端記号の定義を分離する。
// * 生成規則は good_RootNodeBody.prule へ残し、
// * 終端器号は good_RootNodeBody.terminal_symbolへ移動する。
static good_ASTNode *good_normalize_ast_2(good_ASTNodeStore *nodes, good_ASTNode *root_node, syms_SymbolStore *syms, int *num)
{
    good_ASTNode *prule_node;
    good_RootNodeBody *root_body;

    root_body = &root_node->body.root;

    prule_node = root_body->prule;
    while (prule_node != NULL) {
        good_PRuleNodeBody *prule_body;
        good_ASTNode *next_prule_node;

        next_prule_node = prule_node->body.prule.next;

        if (!good_is_tsymbol_node(prule_node)) {
            prule_node = next_prule_node;
            continue;
        }

        prule_body = &prule_node->body.prule;

        // prule_node を root_body->prule から外す。
        if (prule_body->prev == NULL) {
            root_body->prule = prule_body->next;
            if (prule_body->next != NULL) {
                prule_body->next->body.prule.prev = NULL;
            }
        }
        else {
            prule_body->prev->body.prule.next = prule_body->next;
            if (prule_body->next != NULL) {
                prule_body->next->body.prule.prev = prule_body->prev;
            }
        }
        prule_body->prev = NULL;
        prule_body->next = NULL;

        // prule_node を root_body->terminal_symbol へ接続する。
        good_append_tsymbol_prule_node(root_node, prule_node);

        prule_node = next_prule_node;
    }

    return root_node;
}

// 生成規則の右辺に現れる文字列を終端記号として定義する。
static good_ASTNode *good_normalize_ast_3(good_ASTNodeStore *nodes, good_ASTNode *root_node, syms_SymbolStore *syms, int *num)
{
    char new_lhs_str_buffer[64];
    const char *new_lhs_str;
    const good_ASTNode *prule_node;
    good_RootNodeBody *root_body;

    root_body = &root_node->body.root;

    for (prule_node = root_body->prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const good_PRuleNodeBody *prule_body;
        const good_ASTNode *rhs_node;

        prule_body = &prule_node->body.prule;

        for (rhs_node = prule_body->rhs; rhs_node != NULL; rhs_node = rhs_node->body.prule_rhs.next) {
            const good_PRuleRHSNodeBody *rhs_body;
            const good_ASTNode *rhs_elem_node;

            rhs_body = &rhs_node->body.prule_rhs;

            for (rhs_elem_node = rhs_body->elem; rhs_elem_node != NULL; rhs_elem_node = rhs_elem_node->body.prule_rhs_element.next) {
                const good_PRuleRHSElementNodeBody *rhs_elem_body;
                good_ASTNode *new_prule_node;
                const syms_SymbolID *new_lhs_id;
                const good_ASTNode *ret_ast;

                // 生成規則右辺に現れる文字列トークンは終端記号として定義する。

                if (rhs_elem_node->type != good_AST_PRULE_RHS_ELEM_STRING) {
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

                rhs_elem_body = &rhs_elem_node->body.prule_rhs_element;

                // 新たな生成規則のASTを生成
                new_prule_node = good_new_tsymbol_prule_node(nodes, *new_lhs_id, rhs_elem_body->symbol);
                if (new_prule_node == NULL) {
                    return NULL;
                }
                good_append_tsymbol_prule_node(root_node, new_prule_node);

                // 上記で定義を追加したことによるASTの書き換え
                ret_ast = good_rewrite_rhs_elem_node(root_node, rhs_elem_body->symbol, *new_lhs_id);
                if (ret_ast == NULL) {
                    return NULL;
                }

                *num = *num + 1;
            }
        }
    }

    return root_node;
}

// 量指定子をもつ生成規則を、量指定子をもたない形式に変換する。
static good_ASTNode *good_normalize_ast_4(good_ASTNodeStore *nodes, good_ASTNode *root_node, syms_SymbolStore *syms, int *num)
{
    const good_ASTNode *prule_node;

    for (prule_node = root_node->body.root.prule; prule_node != NULL; prule_node = prule_node->body.prule.next) {
        const good_ASTNode *rhs_node;

        for (rhs_node = prule_node->body.prule.rhs; rhs_node != NULL; rhs_node = rhs_node->body.prule_rhs.next) {
            good_ASTNode *rhs_elem_node;

            for (rhs_elem_node = rhs_node->body.prule_rhs.elem; rhs_elem_node != NULL; rhs_elem_node = rhs_elem_node->body.prule_rhs_element.next) {
                good_PRuleRHSElementNodeBody *rhs_elem_body;

                rhs_elem_body = &rhs_elem_node->body.prule_rhs_element;
                
                // 量指定子をもつ規則を展開する。
                if (rhs_elem_body->quantifier == good_Q_0_OR_1) {
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

                    const syms_SymbolID *new_lhs_id;

                    // オプションの記号を削除した生成規則を生成する。
                    {
                        good_ASTNode *new_prule_node;
                        good_ASTNode *new_rhs_node;
                        good_ASTNode *new_rhs_elem_node;
                        const char *new_lhs_str;
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

                        new_prule_node = good_get_ast_node(nodes, good_AST_PRULE);
                        if (new_prule_node == NULL) {
                            return NULL;
                        }
                        new_prule_node->body.prule.lhs = *new_lhs_id;
                        good_append_prule_node(root_node, new_prule_node);

                        /*
                         * 下記の1の生成規則を追加する。
                         * 
                         * ccc_or_empty
                         *     : ccc ... 1
                         *     | ε ..... 2
                         *     ;
                         */
                        new_rhs_node = good_get_ast_node(nodes, good_AST_PRULE_RHS);
                        if (new_rhs_node == NULL) {
                            return NULL;
                        }
                        good_append_prule_rhs_node(new_prule_node, new_rhs_node);

                        new_rhs_elem_node = good_get_ast_node(nodes, rhs_elem_node->type);
                        if (new_rhs_elem_node == NULL) {
                            return NULL;
                        }
                        new_rhs_elem_node->body.prule_rhs_element.symbol = rhs_elem_body->symbol;
                        good_append_prule_rhs_elem_node(new_rhs_node, new_rhs_elem_node);

                        /*
                         * 下記の2の生成規則を追加する。
                         * 
                         * ccc_or_empty
                         *     : ccc ... 1
                         *     | ε ..... 2
                         *     ;
                         */
                        new_rhs_node = good_get_ast_node(nodes, good_AST_PRULE_RHS);
                        if (new_rhs_node == NULL) {
                            return NULL;
                        }
                        good_append_prule_rhs_node(new_prule_node, new_rhs_node);
                    }

                    // オプションの記号を書き換える。
                    rhs_elem_body->symbol = *new_lhs_id;
                    rhs_elem_body->quantifier = good_Q_1;
                }
                else if (rhs_elem_body->quantifier == good_Q_0_OR_MORE || rhs_elem_body->quantifier == good_Q_1_OR_MORE) {
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

                    const syms_SymbolID *new_lhs_id;

                    // 繰り返し部分を抜き出した生成規則を生成する。
                    {
                        good_ASTNode *new_prule_node;
                        good_ASTNode *new_rhs_node;
                        good_ASTNode *new_rhs_elem_node;
                        const char *new_lhs_str;
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

                        new_prule_node = good_get_ast_node(nodes, good_AST_PRULE);
                        if (new_prule_node == NULL) {
                            return NULL;
                        }
                        new_prule_node->body.prule.lhs = *new_lhs_id;
                        good_append_prule_node(root_node, new_prule_node);

                        /*
                         * 下記の1の生成規則を追加する。
                         * 
                         * ccc_list
                         *     : ccc ............ 1
                         *     | ccc_list ccc ... 2
                         *     ;
                         */
                        new_rhs_node = good_get_ast_node(nodes, good_AST_PRULE_RHS);
                        if (new_rhs_node == NULL) {
                            return NULL;
                        }
                        good_append_prule_rhs_node(new_prule_node, new_rhs_node);

                        new_rhs_elem_node = good_get_ast_node(nodes, rhs_elem_node->type);
                        if (new_rhs_elem_node == NULL) {
                            return NULL;
                        }
                        new_rhs_elem_node->body.prule_rhs_element.symbol = rhs_elem_body->symbol;
                        good_append_prule_rhs_elem_node(new_rhs_node, new_rhs_elem_node);

                        /*
                         * 下記の2の生成規則を追加する。
                         * 
                         * ccc_list
                         *     : ccc ............ 1
                         *     | ccc_list ccc ... 2
                         *     ;
                         */
                        new_rhs_node = good_get_ast_node(nodes, good_AST_PRULE_RHS);
                        if (new_rhs_node == NULL) {
                            return NULL;
                        }
                        good_append_prule_rhs_node(new_prule_node, new_rhs_node);

                        new_rhs_elem_node = good_get_ast_node(nodes, good_AST_PRULE_RHS_ELEM_SYMBOL);
                        if (new_rhs_elem_node == NULL) {
                            return NULL;
                        }
                        new_rhs_elem_node->body.prule_rhs_element.symbol = *new_lhs_id;
                        good_append_prule_rhs_elem_node(new_rhs_node, new_rhs_elem_node);

                        new_rhs_elem_node = good_get_ast_node(nodes, rhs_elem_node->type);
                        if (new_rhs_elem_node == NULL) {
                            return NULL;
                        }
                        new_rhs_elem_node->body.prule_rhs_element.symbol = rhs_elem_body->symbol;
                        good_append_prule_rhs_elem_node(new_rhs_node, new_rhs_elem_node);

                        /*
                         * 下記の3の生成規則を追加する。
                         * 
                         * ccc_list
                         *     : ccc ............ 1
                         *     | ccc_list ccc ... 2
                         *     | ε .............. 3
                         *     ;
                         */
                        if (rhs_elem_body->quantifier == good_Q_0_OR_MORE) {
                            new_rhs_node = good_get_ast_node(nodes, good_AST_PRULE_RHS);
                            if (new_rhs_node == NULL) {
                                return NULL;
                            }
                            good_append_prule_rhs_node(new_prule_node, new_rhs_node);
                        }
                    }

                    // 繰り返し対象の記号を書き換える。
                    rhs_elem_body->symbol = *new_lhs_id;
                    rhs_elem_body->quantifier = good_Q_1;
                }
            }
        }
    }

    return root_node;
}

good_ASTNode *good_normalize_ast(good_ASTNodeStore *nodes, good_ASTNode *root_node, syms_SymbolStore *syms)
{
    good_ASTNode *ast;
    int num;

    good_log_info("--------------------------------");
    good_print_ast(root_node, syms);

    ast = good_normalize_ast_1(nodes, root_node, syms, &num);
    if (ast == NULL) {
        return NULL;
    }

    good_log_info("--------------------------------");
    good_print_ast(root_node, syms);

    ast = good_normalize_ast_2(nodes, root_node, syms, &num);
    if (ast == NULL) {
        return NULL;
    }

    good_log_info("--------------------------------");
    good_print_ast(root_node, syms);

    ast = good_normalize_ast_3(nodes, root_node, syms, &num);
    if (ast == NULL) {
        return NULL;
    }

    good_log_info("--------------------------------");
    good_print_ast(root_node, syms);

    ast = good_normalize_ast_4(nodes, root_node, syms, &num);
    if (ast == NULL) {
        return NULL;
    }

    good_log_info("--------------------------------");
    good_print_ast(root_node, syms);
    good_log_info("--------------------------------");

    return ast;
}
