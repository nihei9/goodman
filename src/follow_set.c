#include "follow_set.h"
#include "collections.h"

typedef struct ffset_FollowSetTableElem {
    grm_SymbolID *head;
    size_t len;
    int has_eof;
} ffset_FollowSetTableElem;

struct ffset_FollowSet {
    struct {
        hmap_HashMap *map;
    } set;

    struct {
        arr_Array *arr;
    } work;
};

typedef struct ffset_FollowSetCalcFrame {
    grm_SymbolID sym;
    size_t arr_bottom_index;
    size_t arr_fill_index;
    int has_eof;
    int already_caluclated;
} ffset_FollowSetCalcFrame;


static void ffset_set_flws_calc_frame(ffset_FollowSetCalcFrame *frame, grm_SymbolID sym, size_t arr_bottom_index);
static int ffset_calc_flws_at(ffset_FollowSet *flws, ffset_FollowSetCalcFrame *frame, const grm_Grammar *grm, const ffset_FirstSet *fsts);


ffset_FollowSet *ffset_new_flws(void)
{
    ffset_FollowSet *flws = NULL;
    arr_Array *arr = NULL;
    hmap_HashMap *map = NULL;

    flws = (ffset_FollowSet *) malloc(sizeof (ffset_FollowSet));
    if (flws == NULL) {
        goto FAILURE;
    }

    arr = arr_new(sizeof (grm_SymbolID));
    if (arr == NULL) {
        goto FAILURE;
    }
    arr_autoext_on(arr, 64);

    map = hmap_new(c_TYPE_ULONG, c_TYPE_ULONG);
    if (map == NULL) {
        goto FAILURE;
    }

    flws->work.arr = arr;
    flws->set.map = map;

    return flws;

FAILURE:
    free(flws);
    arr_delete(arr);
    hmap_delete(map);

    return NULL;
}

void ffset_delete_flws(ffset_FollowSet *flws)
{
    if (flws == NULL) {
        return;
    }

    arr_delete(flws->work.arr);
    flws->work.arr = NULL;
    hmap_delete(flws->set.map);
    flws->set.map = NULL;
    free(flws);

    return;
}

int ffset_calc_flws(ffset_FollowSet *flws, const grm_Grammar *grm, const ffset_FirstSet *fsts)
{
    const grm_ProductionRule *prule;
    grm_ProductionRuleFilter filter_body;
    grm_ProductionRuleFilter *filter;

    filter = grm_find_all_prule(grm, &filter_body);
    if (filter == NULL) {
        return 1;
    }
    prule = grm_next_prule(grm, filter);
    if (prule == NULL) {
        return 1;
    }

    while (prule != NULL) {
        grm_SymbolID sym = grm_get_pr_lhs(prule);
        ffset_FollowSetCalcFrame frame;
        int ret;

        // 計算対象は非終端記号のみ
        if (grm_get_symbol_type(sym) != grm_SYMTYPE_NON_TERMINAL) {
            prule = grm_next_prule(grm, filter);
            continue;
        }

        ffset_set_flws_calc_frame(&frame, sym, 0);
        ret = ffset_calc_flws_at(flws, &frame, grm, fsts);
        if (ret != 0) {
            return 1;
        }

        prule = grm_next_prule(grm, filter);
    }

    return 0;
}


int ffset_get_flws(grm_SymbolID **set, size_t *len, int *has_eof, const ffset_FollowSet *flws, grm_SymbolID symbol)
{
    const ffset_FollowSetTableElem *elem;

    if (set != NULL && len == NULL) {
        return 1;
    }

    elem = (const ffset_FollowSetTableElem *) hmap_lookup(flws->set.map, &symbol);
    if (elem == NULL) {
        return 1;
    }

    if (set != NULL) {
        *set = elem->head;
    }
    if (len != NULL) {
        *len = elem->len;
    }
    if (has_eof != NULL) {
        *has_eof = elem->has_eof;
    }

    return 0;
}


static void ffset_set_flws_calc_frame(ffset_FollowSetCalcFrame *frame, grm_SymbolID sym, size_t arr_bottom_index)
{
    frame->sym = sym;
    frame->arr_bottom_index = arr_bottom_index;
}


static int ffset_calc_flws_at(ffset_FollowSet *flws, ffset_FollowSetCalcFrame *frame, const grm_Grammar *grm, const ffset_FirstSet *fsts)
{
    ffset_FollowSetTableElem *elem;
    const grm_SymbolID ssym = grm_get_start_symbol(grm);

    frame->arr_fill_index = frame->arr_bottom_index;
    frame->has_eof = 0;
    frame->already_caluclated = 0;

    elem = (ffset_FollowSetTableElem *) hmap_lookup(flws->set.map, &frame->sym);
    if (elem == NULL) {
        void *ret;

        elem = (ffset_FollowSetTableElem *) malloc(sizeof (ffset_FollowSetTableElem));
        if (elem == NULL) {
            return 1;
        }
        elem->head = NULL;
        elem->len = 0;
        elem->has_eof = 0;

        ret = hmap_put(flws->set.map, &frame->sym, &elem);
        if (ret == NULL) {
            free(elem);
            return 1;
        }
    }

    // 記号が開始記号ならEOFをFOLLOW集合に追加する。
    if (frame->sym == ssym) {
        elem->has_eof = 1;
    }

    {
        const grm_ProductionRule *prule;
        grm_ProductionRuleFilter filter_body;
        grm_ProductionRuleFilter *filter;

        filter = grm_find_all_prule(grm, &filter_body);
        if (filter == NULL) {
            return 1;
        }
        prule = grm_next_prule(grm, filter);
        if (prule == NULL) {
            return 1;
        }

        while (prule != NULL) {
            const grm_SymbolID *rhs = grm_get_pr_rhs(prule);
            size_t rhs_len = grm_get_pr_rhs_len(prule);
            size_t i;

            for (i = 0; i < rhs_len; i++) {
                grm_SymbolID *fsts_set;
                size_t fsts_len;
                int has_empty;
                int ret;
                ffset_FollowSetCalcFrame f;

                if (rhs[i] != frame->sym) {
                    continue;
                }

                ret = ffset_get_fsts(&fsts_set, &fsts_len, &has_empty, fsts, grm_get_pr_id(prule), i + 1);
                if (ret != 0) {
                    return 1;
                }

                // TODO setをFOLLOW集合に追加する。

                if (has_empty == 0) {
                    continue;
                }

                ffset_set_flws_calc_frame(&f, grm_get_pr_lhs(prule), 0);
                ret = ffset_calc_flws_at(flws, &f, grm, fsts);
                if (ret != 0) {
                    return 1;
                }

                // TODO 上記で計算したをFOLLOW集合を追加する。
            }
        }

        prule = grm_next_prule(grm, filter);
    }

    return 0;
}
