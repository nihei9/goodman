#include "tokenizer.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_1(connie_Connie *c);
void test_2(connie_Connie *c);

int main (void)
{
    connie_Connie *c = connie_new(__FILE__);

    TEST(c, test_1);
    TEST(c, test_2);

    connie_print(c);
    connie_delete(c);
    
    return 0;
}

void test_1(connie_Connie *c)
{
    good_Tokenizer *tknzr;
    syms_SymbolStore *syms;
    const good_Token *tkn;
    FILE *target;

    target = fopen("test/data/tokenizer_test/tokens.txt", "r");
    A_NOT_NULL(c, target);

    syms = syms_new();
    A_NOT_NULL(c, syms);

    tknzr = good_new_tokenizer(target, syms);
    A_NOT_NULL(c, tknzr);

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_LEADER, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_OR, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_OR, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_TERMINATOR, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "xxxx", syms_lookup(syms, tkn->value.symbol_id));
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "xxxx_yyyy", syms_lookup(syms, tkn->value.symbol_id));
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "__XXXX__", syms_lookup(syms, tkn->value.symbol_id));
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "Xxxx", syms_lookup(syms, tkn->value.symbol_id));
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_STRING(c, "xxxx", syms_lookup(syms, tkn->value.symbol_id));
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_STRING(c, "xxxx :|;", syms_lookup(syms, tkn->value.symbol_id));
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
    }

    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_EOF, tkn->type);
    }

    good_delete_tokenizer(tknzr);
    syms_delete(syms);
}

void test_2(connie_Connie *c)
{
    good_Tokenizer *tknzr;
    syms_SymbolStore *syms;
    const good_Token *tkn;
    FILE *target;

    target = fopen("test/data/test.goodman", "r");
    A_NOT_NULL(c, target);

    syms = syms_new();
    A_NOT_NULL(c, syms);

    tknzr = good_new_tokenizer(target, syms);
    A_NOT_NULL(c, tknzr);

    // E
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "E", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 0, tkn->pos.row);
        A_EQL_ULONG(c, 0, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 0, tkn->pos.row);
        A_EQL_ULONG(c, 1, tkn->pos.col);
    }

    // :
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_LEADER, tkn->type);
        A_EQL_ULONG(c, 1, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // E
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "E", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 1, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // '+'
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_ULONG(c, 1, tkn->pos.row);
        A_EQL_ULONG(c, 8, tkn->pos.col);
    }

    // T
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "T", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 1, tkn->pos.row);
        A_EQL_ULONG(c, 12, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 1, tkn->pos.row);
        A_EQL_ULONG(c, 13, tkn->pos.col);
    }

    // |
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_OR, tkn->type);
        A_EQL_ULONG(c, 2, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // T
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "T", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 2, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 2, tkn->pos.row);
        A_EQL_ULONG(c, 7, tkn->pos.col);
    }

    // ;
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_TERMINATOR, tkn->type);
        A_EQL_ULONG(c, 3, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 3, tkn->pos.row);
        A_EQL_ULONG(c, 5, tkn->pos.col);
    }

    // T
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "T", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 4, tkn->pos.row);
        A_EQL_ULONG(c, 0, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 4, tkn->pos.row);
        A_EQL_ULONG(c, 1, tkn->pos.col);
    }

    // :
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_LEADER, tkn->type);
        A_EQL_ULONG(c, 5, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // T
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "T", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 5, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // '*'
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_STRING(c, "*", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 5, tkn->pos.row);
        A_EQL_ULONG(c, 8, tkn->pos.col);
    }

    // F
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "F", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 5, tkn->pos.row);
        A_EQL_ULONG(c, 12, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 5, tkn->pos.row);
        A_EQL_ULONG(c, 13, tkn->pos.col);
    }

    // |
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_OR, tkn->type);
        A_EQL_ULONG(c, 6, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // F
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "F", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 6, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 6, tkn->pos.row);
        A_EQL_ULONG(c, 7, tkn->pos.col);
    }

    // ;
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_TERMINATOR, tkn->type);
        A_EQL_ULONG(c, 7, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 7, tkn->pos.row);
        A_EQL_ULONG(c, 5, tkn->pos.col);
    }

    // F
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "F", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 8, tkn->pos.row);
        A_EQL_ULONG(c, 0, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 8, tkn->pos.row);
        A_EQL_ULONG(c, 1, tkn->pos.col);
    }

    // :
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_LEADER, tkn->type);
        A_EQL_ULONG(c, 9, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // '('
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_STRING(c, "(", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 9, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // E
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "E", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 9, tkn->pos.row);
        A_EQL_ULONG(c, 10, tkn->pos.col);
    }

    // ')'
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_STRING(c, ")", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 9, tkn->pos.row);
        A_EQL_ULONG(c, 12, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 9, tkn->pos.row);
        A_EQL_ULONG(c, 15, tkn->pos.col);
    }

    // |
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_OR, tkn->type);
        A_EQL_ULONG(c, 10, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // id
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "id", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 10, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 10, tkn->pos.row);
        A_EQL_ULONG(c, 8, tkn->pos.col);
    }

    // ;
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_TERMINATOR, tkn->type);
        A_EQL_ULONG(c, 11, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 11, tkn->pos.row);
        A_EQL_ULONG(c, 5, tkn->pos.col);
    }

    // id
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NAME, tkn->type);
        A_EQL_STRING(c, "id", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 13, tkn->pos.row);
        A_EQL_ULONG(c, 0, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 13, tkn->pos.row);
        A_EQL_ULONG(c, 2, tkn->pos.col);
    }

    // :
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_LEADER, tkn->type);
        A_EQL_ULONG(c, 14, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // '[a-zA-Z_][a-zA-Z0-9_]*'
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_STRING, tkn->type);
        A_EQL_STRING(c, "[a-zA-Z_][a-zA-Z0-9_]*", syms_lookup(syms, tkn->value.symbol_id));
        A_EQL_ULONG(c, 14, tkn->pos.row);
        A_EQL_ULONG(c, 6, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 14, tkn->pos.row);
        A_EQL_ULONG(c, 30, tkn->pos.col);
    }

    // ;
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_PRULE_TERMINATOR, tkn->type);
        A_EQL_ULONG(c, 15, tkn->pos.row);
        A_EQL_ULONG(c, 4, tkn->pos.col);
    }

    // \n
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_NEW_LINE, tkn->type);
        A_EQL_ULONG(c, 15, tkn->pos.row);
        A_EQL_ULONG(c, 5, tkn->pos.col);
    }

    // EOF
    tkn = good_consume_token(tknzr);
    A_NOT_NULL(c, tkn);
    if (tkn != NULL) {
        A_EQL_INT(c, good_TKN_EOF, tkn->type);
        A_EQL_ULONG(c, 16, tkn->pos.row);
        A_EQL_ULONG(c, 0, tkn->pos.col);
    }

    good_delete_tokenizer(tknzr);
    syms_delete(syms);
}
