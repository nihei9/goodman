#include "follow_set.h"
#include "collections.h"

typedef struct ffset_FollowSetTableElem {
    syms_SymbolID *head;
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
    syms_SymbolID sym;
    size_t arr_bottom_index;
    size_t arr_fill_index;
    int has_eof;
    int already_caluclated;
} ffset_FollowSetCalcFrame;


static void ffset_set_flws_calc_frame(ffset_FollowSetCalcFrame *frame, syms_SymbolID sym, size_t arr_bottom_index);
static int ffset_calc_flws_at(ffset_FollowSet *flws, ffset_FollowSetCalcFrame *frame, const good_Grammar *grammar, const ffset_FirstSet *fsts);


ffset_FollowSet *ffset_new_flws(void)
{
    ffset_FollowSet *flws = NULL;
    arr_Array *arr = NULL;
    hmap_HashMap *map = NULL;

    flws = (ffset_FollowSet *) malloc(sizeof (ffset_FollowSet));
    if (flws == NULL) {
        goto FAILURE;
    }

    arr = arr_new(sizeof (syms_SymbolID));
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

int ffset_calc_flws(ffset_FollowSet *flws, const good_Grammar *grammar, const ffset_FirstSet *fsts)
{
    const good_ProductionRule *prule;
    good_ProductionRuleFilter filter;

    good_set_prule_filter_match_all(&filter);
    prule = good_next_prule(&filter, grammar->prules);
    if (prule == NULL) {
        return 1;
    }
    while (prule != NULL) {
        ffset_FollowSetCalcFrame frame;
        int ret;

        if (prule->lhs >= grammar->terminal_symbol_id_from && prule->lhs <= grammar->terminal_symbol_id_to) {
            prule = good_next_prule(&filter, grammar->prules);
            continue;
        }

        ffset_set_flws_calc_frame(&frame, prule->lhs, 0);
        ret = ffset_calc_flws_at(flws, &frame, grammar, fsts);
        if (ret != 0) {
            return 1;
        }

        prule = good_next_prule(&filter, grammar->prules);
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


static void ffset_set_flws_calc_frame(ffset_FollowSetCalcFrame *frame, syms_SymbolID sym, size_t arr_bottom_index)
{
    frame->sym = sym;
    frame->arr_bottom_index = arr_bottom_index;
}


static int ffset_calc_flws_at(ffset_FollowSet *flws, ffset_FollowSetCalcFrame *frame, const good_Grammar *grammar, const ffset_FirstSet *fsts)
{
    frame->arr_fill_index = frame->arr_bottom_index;
    frame->has_eof = 0;
    frame->already_caluclated = 0;

    // 計算済みなら処理終了
    if (hmap_lookup(flws->set.map, &frame->sym) != NULL) {
        return 0;
    }

    // 記号が開始記号ならEOFをFOLLOW集合に追加する。
    if (frame->sym == grammar->start_symbol) {
        frame->has_eof = 1;
    }

    {
        const good_ProductionRule *prule;
        good_ProductionRuleFilter filter;

        good_set_prule_filter_match_all(&filter);
        prule = good_next_prule(&filter, grammar->prules);
        if (prule == NULL) {
            return 1;
        }
        while (prule != NULL) {
            size_t i;

            if (prule->lhs >= grammar->terminal_symbol_id_from && prule->lhs <= grammar->terminal_symbol_id_to) {
                prule = good_next_prule(&filter, grammar->prules);
                continue;
            }

            for (i = 0; i < prule->rhs_len; i++) {
                ffset_FirstSetItem fsts_item;
                ffset_FollowSetItem flws_item;
                int ret;
                ffset_FollowSetCalcFrame f;
                size_t j;

                // 生成規則の右辺に計算対象の記号が現れた場合のみFOLLOW集合を計算する。
                if (prule->rhs[i] != frame->sym) {
                    continue;
                }

                // 計算対象の記号に後続する記号列のFIRST集合をFOLLOW集合に追加する。
                fsts_item.input.fsts = (ffset_FirstSet *) fsts;
                fsts_item.input.prule_id = prule->id;
                fsts_item.input.offset = i + 1;
                ret = ffset_get_fsts(&fsts_item);
                if (ret != 0) {
                    return 1;
                }
                for (j = 0; j < fsts_item.output.len; j++) {
                    void *ret;
                    int already_exist = 0;
                    size_t k;

                    // 記号がすでにFOLLOW集合に含まれる場合は再度登録はしない。
                    for (k = 0; k < frame->arr_fill_index; k++) {
                        const syms_SymbolID *id;
                        
                        id = (syms_SymbolID *) arr_get(flws->work.arr, k);
                        if (id == NULL) {
                            return 1;
                        }
                        if (fsts_item.output.set[j] == *id) {
                            already_exist = 1;
                            break;
                        }
                    }
                    if (!already_exist) {
                        ret = arr_set(flws->work.arr, frame->arr_fill_index++, &fsts_item.output.set[j]);
                        if (ret == NULL) {
                            return 1;
                        }
                    }
                }

                if (fsts_item.output.has_empty == 0) {
                    continue;
                }

                // 上記で求めたFIRST集合に空集合が含まれる場合は、FIRST集合計算時に対象となった生成規則の左辺値のFOLLOW集合を求めてそれを追加する。

                ffset_set_flws_calc_frame(&f, prule->lhs, frame->arr_fill_index);
                ret = ffset_calc_flws_at(flws, &f, grammar, fsts);
                if (ret != 0) {
                    return 1;
                }
                flws_item.input.flws = flws;
                flws_item.input.symbol = f.sym;
                ret = ffset_get_flws(&flws_item);
                if (ret != 0) {
                    return 1;
                }
                if (flws_item.output.has_eof) {
                    frame->has_eof = 1;
                }
                for (j = 0; j < flws_item.output.len; j++) {
                    void *ret;
                    int already_exist = 0;
                    size_t k;

                    // 記号がすでにFOLLOW集合に含まれる場合は再度登録はしない。
                    for (k = 0; k < frame->arr_fill_index; k++) {
                        const syms_SymbolID *id;
                        
                        id = (syms_SymbolID *) arr_get(flws->work.arr, k);
                        if (id == NULL) {
                            return 1;
                        }
                        if (flws_item.output.set[j] == *id) {
                            already_exist = 1;
                            break;
                        }
                    }
                    if (!already_exist) {
                        ret = arr_set(flws->work.arr, frame->arr_fill_index++, &flws_item.output.set[j]);
                        if (ret == NULL) {
                            return 1;
                        }
                    }
                }
            }

            prule = good_next_prule(&filter, grammar->prules);
        }
    }

    {
        ffset_FollowSetTableElem *elem;
        syms_SymbolID *set;
        size_t i;
        void *ret;

        set = (syms_SymbolID *) calloc(frame->arr_fill_index - frame->arr_bottom_index, sizeof (syms_SymbolID));
        if (set == NULL) {
            return 1;
        }
        for (i = frame->arr_bottom_index; i < frame->arr_fill_index; i++) {
            const syms_SymbolID *sym;

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
