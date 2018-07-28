#include "parser.h"
#include <stdlib.h>
#include <setjmp.h>

struct good_Parser {
    good_Tokenizer *tknzr;

    good_Error error;

    jmp_buf jmp_env;
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

static good_AST *good_psr_new_ast_node_with_q(good_Parser *psr, good_AST *parent, good_ASTType type, const good_Token *tkn, good_QuantifierType quantifier)
{
    good_AST *node;

    node = good_new_ast_with_q(type, tkn, quantifier);
    if (node == NULL) {
        longjmp(psr->jmp_env, JMP_INTERNAL_ERROR);
    }

    if (parent != NULL) {
        good_append_child(parent, node);
    }

    return node;
}

static good_AST *good_psr_new_ast_node(good_Parser *psr, good_AST *parent, good_ASTType type, const good_Token *tkn)
{
    return good_psr_new_ast_node_with_q(psr, parent, type, tkn, good_Q_1);
}

static good_AST *good_parse_prule_rhs_elem(good_Parser *psr, good_AST *parent)
{
    good_Token rhs_elem_tkn;
    good_ASTType type;
    good_QuantifierType q;
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
    rhs_elem_tkn = *tkn;
    
    q = good_Q_1;
    tkn = good_psr_peek_token(psr);
    if (tkn->type == good_TKN_OPTION || tkn->type == good_TKN_ASTERISK || tkn->type == good_TKN_PLUS) {
        tkn = good_psr_consume_token(psr);
        if (tkn->type == good_TKN_OPTION) {
            q = good_Q_0_OR_1;
        }
        else if (tkn->type == good_TKN_ASTERISK) {
            q = good_Q_0_OR_MORE;
        }
        else if (tkn->type == good_TKN_PLUS) {
            q = good_Q_1_OR_MORE;
        }
    }

    return good_psr_new_ast_node_with_q(psr, parent, type, &rhs_elem_tkn, q);
}

const good_AST *good_parse(good_Parser *psr)
{
    good_AST *root_ast = NULL;
    const good_Token *tkn;

    if (setjmp(psr->jmp_env) != 0) {
        goto ERROR;
    }

    root_ast = good_psr_new_ast_node(psr, NULL, good_AST_ROOT, NULL);

    while (1) {
        good_AST *prule_ast;

        good_psr_skip_newline(psr);

        tkn = good_psr_consume_token(psr);
        if (tkn->type != good_TKN_NAME) {
            break;
        }

        prule_ast = good_psr_new_ast_node(psr, root_ast, good_AST_PRULE, NULL);
        good_psr_new_ast_node(psr, prule_ast, good_AST_PRULE_LHS, tkn);

        good_psr_skip_newline(psr);

        good_psr_match_1(psr, good_TKN_PRULE_LEADER, good_ERR_MISSING_PRULE_LEADER);

        while (1) {
            good_AST *prule_rhs_ast;

            prule_rhs_ast = good_psr_new_ast_node(psr, prule_ast, good_AST_PRULE_RHS, NULL);
            
            while (good_parse_prule_rhs_elem(psr, prule_rhs_ast) != NULL);
            
            good_psr_skip_newline(psr);

            tkn = good_psr_peek_token(psr);
            if (tkn->type != good_TKN_PRULE_OR) {
                break;
            }
            good_psr_consume_token(psr);
        }

        good_psr_match_1(psr, good_TKN_PRULE_TERMINATOR, good_ERR_UNTERMINATED_PRULE);
    }

    return root_ast;

ERROR:
    good_delete_ast(root_ast);

    return NULL;
}
