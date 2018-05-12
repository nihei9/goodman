#ifndef ffset_FOLLOW_SET_H
#define ffset_FOLLOW_SET_H

#include "grammar.h"
#include "first_set.h"

typedef struct ffset_FollowSet ffset_FollowSet;

typedef struct ffset_FollowSetItem {
    struct {
        ffset_FollowSet *flws;
        good_SymbolID symbol;
    } input;

    struct {
        good_SymbolID *set;
        size_t len;
        int has_eof;
    } output;
} ffset_FollowSetItem;

ffset_FollowSet *ffset_new_flws(void);
void ffset_delete_flws(ffset_FollowSet *flws);
int ffset_calc_flws(ffset_FollowSet *flws, const grm_Grammar *grm, const ffset_FirstSet *fsts);
int ffset_get_flws(ffset_FollowSetItem *item);

#endif
