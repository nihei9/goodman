#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>

typedef struct good_Char {
    int c;
    good_Position pos;
} good_Char;

static const good_Token *good_tokenize(good_Tokenizer *tknzr);
inline static good_Char good_get_char(good_Tokenizer *tknzr);
inline static void good_unget_char(good_Tokenizer *tknzr, good_Char c);
static int good_is_name_letter(int c);

#define TOKENIZER_STR_LEN 1024

struct good_Tokenizer {
    FILE *target;
    good_Position pos;

    good_Char last_c;

    struct {
        good_Char c;
        int has_c;
    } c_buf;

    struct {
        char *str;
        size_t str_len;
    } work;

    good_Token tkn;

    syms_SymbolStore *syms;
};

good_Tokenizer *good_new_tokenizer(FILE *target, syms_SymbolStore *syms)
{
    good_Tokenizer *tknzr = NULL;
    char *str = NULL;

    tknzr = (good_Tokenizer *) malloc(sizeof (good_Tokenizer));
    if (tknzr == NULL) {
        goto FAILURE;
    }
    str = (char *) calloc(TOKENIZER_STR_LEN, sizeof (char));
    if (str == NULL) {
        goto FAILURE;
    }
    tknzr->target = target;
    tknzr->pos.row = 0;
    tknzr->pos.col = 0;
    tknzr->last_c.c = 'g';
    tknzr->last_c.pos.row = 0;
    tknzr->last_c.pos.col = 0;
    tknzr->c_buf.c.c = 'g';
    tknzr->c_buf.c.pos.row = 0;
    tknzr->c_buf.c.pos.col = 0;
    tknzr->c_buf.has_c = 0;
    tknzr->work.str = str;
    tknzr->work.str_len = TOKENIZER_STR_LEN;
    tknzr->syms = syms;

    return tknzr;

FAILURE:
    free(tknzr);
    free(str);

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
    tknzr->syms = NULL;
    free(tknzr);
}

const good_Token *good_consume_token(good_Tokenizer *tknzr)
{
    return good_tokenize(tknzr);
}

static const good_Token *good_tokenize(good_Tokenizer *tknzr)
{
    good_Token tkn;
    good_Char c;

    if (tknzr->tkn.type == good_TKN_EOF) {
        return &tknzr->tkn;
    }

    c = good_get_char(tknzr);

    if (c.c == ' ') {
        c = good_get_char(tknzr);
    }
    if (c.c == '#') {
        do {
            c = good_get_char(tknzr);
        } while (c.c != '\n' && c.c != EOF);
    }

    tkn.pos = c.pos;

    if (good_is_name_letter(c.c)) {
        const syms_SymbolID *id;
        size_t i = 0;

        do {
            tknzr->work.str[i++] = c.c;
            c = good_get_char(tknzr);
        } while (good_is_name_letter(c.c));
        good_unget_char(tknzr, c);
        tknzr->work.str[i] = '\0';

        id = syms_put(tknzr->syms, tknzr->work.str);
        if (id == NULL) {
            return NULL;
        }

        tkn.type = good_TKN_NAME;
        tkn.value.symbol_id = *id;

        goto RETURN;
    }
    if (c.c == ':') {
        tkn.type = good_TKN_PRULE_LEADER;

        goto RETURN;
    }
    if (c.c == '|') {
        tkn.type = good_TKN_PRULE_OR;

        goto RETURN;
    }
    if (c.c == ';') {
        tkn.type = good_TKN_PRULE_TERMINATOR;

        goto RETURN;
    }
    if (c.c == '\'') {
        const syms_SymbolID *id;
        size_t i = 0;

        do {
            c = good_get_char(tknzr);
            tknzr->work.str[i++] = c.c;
        } while (c.c != EOF && c.c != '\'');
        tknzr->work.str[--i] = '\0';

        if (c.c == EOF) {
            // TODO ERROR
            return NULL;
        }

        if (strlen(tknzr->work.str) <= 0) {
            // TODO ERROR
            return NULL;
        }

        id = syms_put(tknzr->syms, tknzr->work.str);
        if (id == NULL) {
            return NULL;
        }

        tkn.type = good_TKN_STRING;
        tkn.value.symbol_id = *id;

        goto RETURN;
    }
    if (c.c == '\n') {
        tkn.type = good_TKN_NEW_LINE;

        goto RETURN;
    }
    if (c.c == EOF) {
        tkn.type = good_TKN_EOF;

        goto RETURN;
    }

RETURN:

    tknzr->tkn = tkn;

    return &tknzr->tkn;
}

inline static good_Char good_get_char(good_Tokenizer *tknzr)
{
    int c;
    const good_Position pos = tknzr->pos;

    if (tknzr->last_c.c == EOF) {
        return tknzr->last_c;
    }

    if (tknzr->c_buf.has_c != 0) {
        tknzr->last_c = tknzr->c_buf.c;
        tknzr->c_buf.has_c = 0;

        return tknzr->last_c;
    }

    c = fgetc(tknzr->target);
    tknzr->pos.col++;

    // 1つ以上の連続する空白文字は' 'として扱う。
    if (c == ' ' || c == '\t') {
        do {
            c = fgetc(tknzr->target);
            tknzr->pos.col++;
        } while (c == ' ' || c == '\t');
        ungetc(c, tknzr->target);
        tknzr->pos.col--;

        c = ' ';
    }

    // 1つ以上の連続する改行は\nとして扱う。
    if (c == '\n') {
        good_Position tmp_pos;

        tknzr->pos.row++;
        tknzr->pos.col = 0;

        do {
            tmp_pos = tknzr->pos;

            c = fgetc(tknzr->target);
            tknzr->pos.row++;
            tknzr->pos.col = 0;
        } while (c == '\n');
        ungetc(c, tknzr->target);
        tknzr->pos = tmp_pos;

        c = '\n';
    }

    tknzr->last_c.c = c;
    tknzr->last_c.pos = pos;

    return tknzr->last_c;
}

inline static void good_unget_char(good_Tokenizer *tknzr, good_Char c)
{
    tknzr->c_buf.c = c;
    tknzr->c_buf.has_c = 1;
}

static int good_is_name_letter(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        return 1;
    }

    return 0;
}
