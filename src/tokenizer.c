#include "tokenizer.h"
#include <stdlib.h>

static const good_Token *good_tokenize(good_Tokenizer *tknzr);
inline static int good_get_char(good_Tokenizer *tknzr);
inline static void good_unget_char(good_Tokenizer *tknzr);
inline static good_Position good_get_position(const good_Tokenizer *tknzr);
static int good_is_name_letter(int c);

#define TOKENIZER_STR_LEN 1024

struct good_Tokenizer {
    FILE *target;

    int next_char;
    int latest_char;
    int pre_char;

    good_Position next_pos;
    good_Position latest_pos;
    good_Position pre_pos;

    struct {
        char *str;
        size_t str_len;
    } work;

    good_Token tkn;

    grm_SymbolTable *symtbl;
};

good_Tokenizer *good_new_tokenizer(FILE *target)
{
    good_Tokenizer *tknzr = NULL;
    char *str = NULL;
    grm_SymbolTable *symtbl = NULL;

    tknzr = (good_Tokenizer *) malloc(sizeof (good_Tokenizer));
    if (tknzr == NULL) {
        goto FAILURE;
    }
    str = (char *) calloc(TOKENIZER_STR_LEN, sizeof (char));
    if (str == NULL) {
        goto FAILURE;
    }
    symtbl = grm_new_symtbl();
    if (symtbl == NULL) {
        goto FAILURE;
    }
    tknzr->target = target;
    tknzr->next_char = 'g'; // EOF以外なら何でもいい
    tknzr->latest_char = 'g';
    tknzr->pre_char = 'g';
    tknzr->next_pos.row = 0;
    tknzr->next_pos.col = 0;
    tknzr->latest_pos.row = 0;
    tknzr->latest_pos.col = 0;
    tknzr->pre_pos.row = 0;
    tknzr->pre_pos.col = 0;
    tknzr->work.str = str;
    tknzr->work.str_len = TOKENIZER_STR_LEN;
    tknzr->symtbl = symtbl;

    return tknzr;

FAILURE:
    free(tknzr);
    free(str);
    grm_delete_symtbl(symtbl);

    return NULL;
}

void good_delete_tokenizer(good_Tokenizer *tknzr)
{
    if (tknzr == NULL) {
        return;
    }

    tknzr->target = NULL;
    free(tknzr->work.str);
    tknzr->work.str = NULL;
    grm_delete_symtbl(tknzr->symtbl);
    tknzr->symtbl = NULL;
    free(tknzr);
}

const good_Token *good_consume_token(good_Tokenizer *tknzr)
{
    return good_tokenize(tknzr);
}

static const good_Token *good_tokenize(good_Tokenizer *tknzr)
{
    good_Token tkn;
    int c;

    if (tknzr->tkn.type == good_TKN_EOF) {
        return &tknzr->tkn;
    }

    c = good_get_char(tknzr);

    if (c == ' ' || c == '\t') {
        c = good_get_char(tknzr);
    }

    tkn.pos = good_get_position(tknzr);

    if (good_is_name_letter(c)) {
        const grm_SymbolID *id;
        size_t i = 0;

        do {
            tknzr->work.str[i++] = c;
            c = good_get_char(tknzr);
        } while (good_is_name_letter(c));
        good_unget_char(tknzr);
        tknzr->work.str[i] = '\0';

        // 第３引数はtokenizerにおいては使用しないので、何を設定しても無関係。
        id = grm_put_in_symtbl(tknzr->symtbl, tknzr->work.str, grm_SYMTYPE_TERMINAL);
        if (id == NULL) {
            return NULL;
        }

        tkn.type = good_TKN_NAME;
        tkn.value.symbol_id = *id;

        goto RETURN;
    }
    if (c == ':') {
        tkn.type = good_TKN_PRULE_LEADER;

        goto RETURN;
    }
    if (c == '|') {
        tkn.type = good_TKN_PRULE_OR;

        goto RETURN;
    }
    if (c == ';') {
        tkn.type = good_TKN_PRULE_TERMINATOR;

        goto RETURN;
    }
    if (c == '\'') {
        const grm_SymbolID *id;
        size_t i = 0;

        do {
            tknzr->work.str[i++] = c;
            c = good_get_char(tknzr);
        } while (c != '\'');
        good_unget_char(tknzr);
        tknzr->work.str[i] = '\0';

        // 第３引数はtokenizerにおいては使用しないので、何を設定しても無関係。
        id = grm_put_in_symtbl(tknzr->symtbl, tknzr->work.str, grm_SYMTYPE_TERMINAL);
        if (id == NULL) {
            return NULL;
        }

        tkn.type = good_TKN_STRING;
        tkn.value.symbol_id = *id;

        goto RETURN;
    }
    if (c == '#') {
        do {
            c = good_get_char(tknzr);
        } while (c != '\n' || c != EOF);
        good_unget_char(tknzr);
    }
    if (c == '\n') {
        tkn.type = good_TKN_NEW_LINE;

        goto RETURN;
    }
    if (c == EOF) {
        tkn.type = good_TKN_EOF;

        goto RETURN;
    }

RETURN:

    tknzr->tkn = tkn;

    return &tknzr->tkn;
}

inline static int good_get_char(good_Tokenizer *tknzr)
{
    if (tknzr->latest_char == EOF) {
        return EOF;
    }

    tknzr->pre_char = tknzr->latest_char;
    tknzr->pre_pos = tknzr->latest_pos;

    tknzr->latest_char = tknzr->next_char;
    tknzr->latest_pos = tknzr->next_pos;

    if (tknzr->pre_char == '\n') {
        tknzr->latest_pos.row++;
        tknzr->latest_pos.col = 0;
    }

    tknzr->next_pos = tknzr->latest_pos;

    tknzr->next_char = fgetc(tknzr->target);
    tknzr->next_pos.row++;

    if (tknzr->next_char == ' ' || tknzr->next_char == '\t') {
        int c;

        do {
            c = fgetc(tknzr->target);
            tknzr->next_pos.col++;
        } while (tknzr->next_char == ' ' || tknzr->next_char == '\t');
        ungetc(c, tknzr->target);
        tknzr->next_pos.row--;
    }

    return tknzr->latest_char;
}

// good_unget_char()を呼ぶ前に必ずgood_get_char()を1回以上呼んでいること。
// good_unget_char()を2回以上連続して呼ばないこと。
inline static void good_unget_char(good_Tokenizer *tknzr)
{
    tknzr->next_char = tknzr->latest_char;
    tknzr->next_pos = tknzr->latest_pos;

    tknzr->latest_char = tknzr->pre_char;
    tknzr->latest_pos = tknzr->pre_pos;
}

inline static good_Position good_get_position(const good_Tokenizer *tknzr)
{
    return tknzr->latest_pos;
}

static int good_is_name_letter(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        return 1;
    }

    return 0;
}
