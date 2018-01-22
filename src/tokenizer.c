#include "tokenizer.h"
#include <stdlib.h>

static const good_Token *good_tokenize(good_Tokenizer *tknzr);
static int good_is_name_letter(int c);

#define TOKENIZER_STR_LEN 1024

struct good_Tokenizer {
    FILE *target;

    struct {
        size_t row;
        size_t col;
    } pos;

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
    tknzr->pos.row = 0;
    tknzr->pos.col = 0;
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

    c = fgetc(tknzr->target);
    if (c == EOF) {
        return NULL;
    }
    tknzr->pos.col++;

    if (c == ' ') {
        do {
            tknzr->pos.col++;
        } while ((c = fgetc(tknzr->target)) == ' ');
        
        if (c == EOF) {
            return NULL;
        }
    }

    tkn.pos.row = tknzr->pos.row;
    tkn.pos.col = tknzr->pos.col;

    if (c == '\t') {
        unsigned int level = 1;

        while ((c = fgetc(tknzr->target)) == '\t') {
            tknzr->pos.row++;
            level++;
        }
        ungetc(c, tknzr->target);

        tkn.type = good_TKN_INDENT;
        tkn.value.uint_value = level;

        goto RETURN;
    }
    if (c == '\n') {
        tknzr->pos.row++;
        tknzr->pos.col = 0;

        while ((c = fgetc(tknzr->target)) == '\n') {
            tknzr->pos.row++;
        }
        ungetc(c, tknzr->target);

        tkn.type = good_TKN_NEW_LINE;

        goto RETURN;
    }
    if (good_is_name_letter(c)) {
        const grm_SymbolID *id;
        size_t i = 0;

        do {
            tknzr->work.str[i++] = c;
            tknzr->pos.col++;
        } while (good_is_name_letter((c = fgetc(tknzr->target))));
        ungetc(c, tknzr->target);
        tknzr->pos.col--;
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
    if (c == '\'') {
        const grm_SymbolID *id;
        size_t i = 0;

        do {
            tknzr->work.str[i++] = c;
            tknzr->pos.col++;
        } while ((c = fgetc(tknzr->target)) != '\'');
        ungetc(c, tknzr->target);
        tknzr->pos.col--;
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

RETURN:

    tknzr->tkn = tkn;

    return &tknzr->tkn;
}

static int good_is_name_letter(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        return 1;
    }

    return 0;
}