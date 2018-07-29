#include "parser.h"
#include <stdlib.h>
#include <setjmp.h>

struct good_Parser {
    good_Tokenizer *tknzr;

    good_Error error;

    jmp_buf jmp_env;
    good_ASTNodeStore *ast_node_store;
};

good_Parser *good_new_parser(good_Tokenizer *tknzr, good_ASTNodeStore *ast_node_store)
{
    good_Parser *psr;

    psr = (good_Parser *) malloc(sizeof (good_Parser));
    if (psr == NULL) {
        return NULL;
    }
    psr->tknzr = tknzr;
    psr->ast_node_store = ast_node_store;

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

#define JMP_SYNTAX_ERROR 1
#define JMP_INTERNAL_ERROR 2

static const good_Token *good_psr_consume_token(good_Parser *psr)
{
    const good_Token *tkn;

    tkn = good_consume_token(psr->tknzr);
    if (tkn == NULL) {
        psr->error = *good_get_tokenizer_error(psr->tknzr);

        longjmp(psr->jmp_env, JMP_INTERNAL_ERROR);
    }

    if (tkn->type == good_TKN_L_PAREN || tkn->type == good_TKN_R_PAREN) {
        psr->error.code = good_ERR_UNIMPLEMENTED_FEATURE;

        longjmp(psr->jmp_env, JMP_SYNTAX_ERROR);
    }

    return tkn;
}

static const good_Token *good_psr_peek_token(good_Parser *psr)
{
    const good_Token *tkn;

    tkn = good_peek_token(psr->tknzr);
    if (tkn == NULL) {
        psr->error = *good_get_tokenizer_error(psr->tknzr);

        longjmp(psr->jmp_env, JMP_INTERNAL_ERROR);
    }

    if (tkn->type == good_TKN_L_PAREN || tkn->type == good_TKN_R_PAREN) {
        psr->error.code = good_ERR_UNIMPLEMENTED_FEATURE;

        longjmp(psr->jmp_env, JMP_SYNTAX_ERROR);
    }

    return tkn;
}

static const good_Token *good_psr_match_1(good_Parser *psr, good_TokenType token_type, good_ErrorCode error_code)
{
    const good_Token *tkn;

    tkn = good_psr_consume_token(psr);
    if (tkn->type != token_type) {
        psr->error.code = error_code;

        longjmp(psr->jmp_env, JMP_SYNTAX_ERROR);
    }

    return tkn;
}

static void good_psr_skip_newline(good_Parser *psr)
{
    const good_Token *tkn;

    tkn = good_psr_peek_token(psr);
    while (tkn->type == good_TKN_NEW_LINE) {
        good_psr_consume_token(psr);
        tkn = good_psr_peek_token(psr);
    }
}

static good_ASTNode *good_psr_new_ast_node(good_Parser *psr, good_ASTType type)
{
    good_ASTNode *node;

    node = good_get_ast_node(psr->ast_node_store, type);
    if (node == NULL) {
        longjmp(psr->jmp_env, JMP_INTERNAL_ERROR);
    }

    return node;
}

static good_ASTNode *good_parse_prule_rhs_elem(good_Parser *psr, good_ASTNode *prule_rhs_node)
{
    good_ASTNode *prule_rhs_elem_node;
    good_ASTType type;
    syms_SymbolID symbol;
    good_QuantifierType quantifier;
    const good_Token *tkn;

    tkn = good_psr_peek_token(psr);
    if (tkn->type != good_TKN_NAME && tkn->type != good_TKN_STRING) {
        return NULL;
    }

    tkn = good_psr_consume_token(psr);
    if (tkn->type == good_TKN_NAME) {
        type = good_AST_PRULE_RHS_ELEM_SYMBOL;
    }
    else {
        type = good_AST_PRULE_RHS_ELEM_STRING;
    }
    symbol = tkn->value.symbol_id;
    
    quantifier = good_Q_1;
    tkn = good_psr_peek_token(psr);
    if (tkn->type == good_TKN_OPTION || tkn->type == good_TKN_ASTERISK || tkn->type == good_TKN_PLUS) {
        tkn = good_psr_consume_token(psr);
        if (tkn->type == good_TKN_OPTION) {
            quantifier = good_Q_0_OR_1;
        }
        else if (tkn->type == good_TKN_ASTERISK) {
            quantifier = good_Q_0_OR_MORE;
        }
        else if (tkn->type == good_TKN_PLUS) {
            quantifier = good_Q_1_OR_MORE;
        }
    }

    prule_rhs_elem_node = good_psr_new_ast_node(psr, type);
    prule_rhs_elem_node->body.prule_rhs_element.symbol = symbol;
    prule_rhs_elem_node->body.prule_rhs_element.quantifier = quantifier;

    good_append_prule_rhs_elem_node(prule_rhs_node, prule_rhs_elem_node);

    return prule_rhs_elem_node;
}

good_ASTNode *good_parse(good_Parser *psr)
{
    good_ASTNode *root_node = NULL;
    const good_Token *tkn;

    if (setjmp(psr->jmp_env) != 0) {
        return NULL;
    }

    root_node = good_psr_new_ast_node(psr, good_AST_ROOT);
    root_node->body.root.prule = NULL;
    root_node->body.root.terminal_symbol = NULL;

    while (1) {
        good_ASTNode *prule_node;

        good_psr_skip_newline(psr);

        tkn = good_psr_consume_token(psr);
        if (tkn->type != good_TKN_NAME) {
            break;
        }

        prule_node = good_psr_new_ast_node(psr, good_AST_PRULE);
        prule_node->body.prule.lhs = tkn->value.symbol_id;

        good_psr_skip_newline(psr);

        good_psr_match_1(psr, good_TKN_PRULE_LEADER, good_ERR_MISSING_PRULE_LEADER);

        while (1) {
            good_ASTNode *prule_rhs_node;

            prule_rhs_node = good_psr_new_ast_node(psr, good_AST_PRULE_RHS);
            
            while (good_parse_prule_rhs_elem(psr, prule_rhs_node) != NULL);
            
            good_psr_skip_newline(psr);

            good_append_prule_rhs_node(prule_node, prule_rhs_node);

            tkn = good_psr_peek_token(psr);
            if (tkn->type != good_TKN_PRULE_OR) {
                break;
            }
            good_psr_consume_token(psr);
        }

        good_psr_match_1(psr, good_TKN_PRULE_TERMINATOR, good_ERR_UNTERMINATED_PRULE);

        // ひとまず全てpruleに接続する。
        // （後続の正規化のフェーズで終端記号の定義を分離する）
        good_append_prule_node(root_node, prule_node);
    }

    return root_node;
}
