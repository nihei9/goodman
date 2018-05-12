#include "follow_set.h"
#include "collections.h"

typedef struct ffset_FollowSetTableElem {
    good_SymbolID *head;
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
    good_SymbolID sym;
    size_t arr_bottom_index;
    size_t arr_fill_index;
    int has_eof;
    int already_caluclated;
} ffset_FollowSetCalcFrame;


static void ffset_set_flws_calc_frame(ffset_FollowSetCalcFrame *frame, good_SymbolID sym, size_t arr_bottom_index);
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

    arr = arr_new(sizeof (good_SymbolID));
    if (arr == NULL) {
        goto FAILURE;
    }
    arr_autoext_on(arr, 64);

    map = hmap_new(c_TYPE_ULONG, c_TYPE_PTR);
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
        good_SymbolID sym = grm_get_pr_lhs(prule);
        ffset_FollowSetCalcFrame frame;
        int ret;

        ffset_set_flws_calc_frame(&frame, sym, 0);
        ret = ffset_calc_flws_at(flws, &frame, grm, fsts);
        if (ret != 0) {
            return 1;
        }

        prule = grm_next_prule(grm, filter);
    }

    return 0;
}


int ffset_get_flws(ffset_FollowSetItem *item)
{
    const ffset_FollowSetTableElem **elem;

    elem = (const ffset_FollowSetTableElem **) hmap_lookup(item->input.flws->set.map, &item->input.symbol);
    if (elem == NULL) {
        return 1;
    }

    item->output.set = (*elem)->head;
    item->output.len = (*elem)->len;
    item->output.has_eof = (*elem)->has_eof;

    return 0;
}


static void ffset_set_flws_calc_frame(ffset_FollowSetCalcFrame *frame, good_SymbolID sym, size_t arr_bottom_index)
{
    frame->sym = sym;
    frame->arr_bottom_index = arr_bottom_index;
}


static int ffset_calc_flws_at(ffset_FollowSet *flws, ffset_FollowSetCalcFrame *frame, const grm_Grammar *grm, const ffset_FirstSet *fsts)
{
    const good_SymbolID ssym = grm_get_start_symbol(grm);

    frame->arr_fill_index = frame->arr_bottom_index;
    frame->has_eof = 0;
    frame->already_caluclated = 0;

    // 計算済みなら処理終了
    if (hmap_lookup(flws->set.map, &frame->sym) != NULL) {
        return 0;
    }

    // 記号が開始記号ならEOFをFOLLOW集合に追加する。
    if (frame->sym == ssym) {
        frame->has_eof = 1;
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
            const good_SymbolID *rhs = grm_get_pr_rhs(prule);
            size_t rhs_len = grm_get_pr_rhs_len(prule);
            size_t i;

            for (i = 0; i < rhs_len; i++) {
                ffset_FirstSetItem fsts_item;
                int ret;
                ffset_FollowSetCalcFrame f;
                ffset_FollowSetTableElem *elem;
                size_t j;

                if (rhs[i] != frame->sym) {
                    continue;
                }

                fsts_item.input.fsts = (ffset_FirstSet *) fsts;
                fsts_item.input.prule_id = grm_get_pr_id(prule);
                fsts_item.input.offset = i + 1;
                ret = ffset_get_fsts(&fsts_item);
                if (ret != 0) {
                    return 1;
                }

                for (i = 0; i < fsts_item.output.len; i++) {
                    void *ret;

                    ret = arr_set(flws->work.arr, frame->arr_fill_index++, &fsts_item.output.set[i]);
                    if (ret == NULL) {
                        return 1;
                    }
                }

                if (fsts_item.output.has_empty == 0) {
                    continue;
                }

                ffset_set_flws_calc_frame(&f, grm_get_pr_lhs(prule), frame->arr_fill_index);
                ret = ffset_calc_flws_at(flws, &f, grm, fsts);
                if (ret != 0) {
                    return 1;
                }

                elem = (ffset_FollowSetTableElem *) hmap_lookup(flws->set.map, &frame->sym);
                if (elem == NULL) {
                    return 1;
                }
                for (j = 0; j < elem->len; i++) {
                    void *ret;

                    ret = arr_set(flws->work.arr, frame->arr_fill_index++, &elem->head[i]);
                    if (ret == NULL) {
                        return 1;
                    }
                }
            }

            prule = grm_next_prule(grm, filter);
        }
    }

    {
        ffset_FollowSetTableElem *elem;
        good_SymbolID *set;
        size_t i;
        void *ret;

        elem = (ffset_FollowSetTableElem *) hmap_lookup(flws->set.map, &frame->sym);
        if (elem != NULL) {
            return 0;
        }

        set = (good_SymbolID *) calloc(frame->arr_fill_index - frame->arr_bottom_index, sizeof (good_SymbolID));
        if (set == NULL) {
            return 1;
        }
        for (i = frame->arr_bottom_index; i < frame->arr_fill_index; i++) {
            const good_SymbolID *sym;

            sym = arr_get(flws->work.arr, i);
            if (sym == NULL) {
                return 1;
            }

            set[i] = *sym;
        }

        elem = (ffset_FollowSetTableElem *) malloc(sizeof (ffset_FollowSetTableElem));
        if (elem == NULL) {
            free(set);
            return 1;
        }
        elem->head = set;
        elem->len = frame->arr_fill_index - frame->arr_bottom_index;
        elem->has_eof = frame->has_eof;

        ret = hmap_put(flws->set.map, &frame->sym, &elem);
        if (ret == NULL) {
            free(elem);
            return 1;
        }
    }

    return 0;
}
