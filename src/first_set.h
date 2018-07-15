#ifndef ffset_FIRST_SET_H
#define ffset_FIRST_SET_H

#include "grammar.h"

typedef struct ffset_FirstSet ffset_FirstSet;

typedef struct ffset_FirstSetItem {
    struct {
        ffset_FirstSet *fsts;
        unsigned int prule_id;
        size_t offset;
    } input;

    struct {
        syms_SymbolID *set;
        size_t len;
        int has_empty;
    } output;
} ffset_FirstSetItem;

ffset_FirstSet *ffset_new_fsts(void);
void ffset_delete_fsts(ffset_FirstSet *fsts);
int ffset_calc_fsts(ffset_FirstSet *fsts, const good_Grammar *grammar);
int ffset_get_fsts(ffset_FirstSetItem *item);

#endif
