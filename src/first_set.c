#include "first_set.h"

typedef struct ffset_FirstSetBranchElem {
    syms_SymbolID *set;
    size_t len;

    int has_calcurated;
} ffset_FirstSetBranchElem;

typedef struct ffset_FirstSetTableElem {
    ffset_FirstSetBranchElem *head;
    size_t len;
} ffset_FirstSetTableElem;

struct ffset_FirstSet {
    struct {
        ffset_FirstSetTableElem *table;
        size_t table_len;
        ffset_FirstSetBranchElem *branch;
        size_t branch_len;
    } set;

    struct {
        arr_Array *arr;
    } work;
};

typedef struct ffset_FirstSetCalcFrame {
    unsigned int prule_id;
    size_t offset;
    size_t arr_bottom_index;
    size_t arr_fill_index;
    int has_empty;
    int already_caluclated;
} ffset_FirstSetCalcFrame;

static int ffset_setup_fsts_table(ffset_FirstSet *fsts, const good_Grammar *grm);
static void ffset_set_fsts_calc_frame(ffset_FirstSetCalcFrame *frame, unsigned int prule_id, size_t offset, size_t arr_bottom_index);
static int ffset_calc_fsts_at(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, const good_Grammar *grm);
static int ffset_add_fsts(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, unsigned int prule_id, size_t offset);
static int ffset_push_fsts_symbol(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, syms_SymbolID symbol);

ffset_FirstSet *ffset_new_fsts(void)
{
    ffset_FirstSet *fsts = NULL;
    arr_Array *arr = NULL;

    fsts = (ffset_FirstSet *) malloc(sizeof (ffset_FirstSet));
    if (fsts == NULL) {
        goto FAILURE;
    }

    arr = arr_new(sizeof (syms_SymbolID));
    if (arr == NULL) {
        goto FAILURE;
    }
    arr_autoext_on(arr, 64);

    fsts->set.table = NULL;
    fsts->set.table_len = 0;
    fsts->set.branch = NULL;
    fsts->set.branch_len = 0;
    fsts->work.arr = arr;

    return fsts;

FAILURE:
    arr_delete(arr);
    free(fsts);

    return NULL;
}

void ffset_delete_fsts(ffset_FirstSet *fsts)
{
    if (fsts == NULL) {
        return;
    }

    free(fsts->set.table);
    fsts->set.table = NULL;
    free(fsts->set.branch);
    fsts->set.branch = NULL;
    arr_delete(fsts->work.arr);
    fsts->work.arr = NULL;
    free(fsts);
}

int ffset_calc_fsts(ffset_FirstSet *fsts, const good_Grammar *grammar)
{
    int ret;

    // FIRST集合を保持するテーブル領域を確保する。
    ret = ffset_setup_fsts_table(fsts, grammar);
    if (ret != 0) {
        return 1;
    }

    // FIRST集合を計算する。
    {
        good_ProductionRuleFilter filter;
        const good_ProductionRule *prule;

        good_set_prule_filter_match_all(&filter);

        prule = good_next_prule(&filter, grammar->prules);
        if (prule == NULL) {
            return 1;
        }
        while (prule != NULL) {
            size_t i;

            for (i = 0; i < prule->rhs_len; i++) {
                ffset_FirstSetCalcFrame frame;
                int ret;

                ffset_set_fsts_calc_frame(&frame, prule->id, i, 0);
                ret = ffset_calc_fsts_at(fsts, &frame, grammar);
                if (ret != 0) {
                    return 1;
                }
            }

            prule = good_next_prule(&filter, grammar->prules);
        }
    }

    return 0;
}

int ffset_get_fsts(ffset_FirstSetItem *item)
{
    ffset_FirstSetTableElem *table_elem;
    ffset_FirstSetBranchElem *branch_elem;
    syms_SymbolID *tmp_set = NULL;
    size_t tmp_len = 0;
    int tmp_has_empty = 0;

    if (item->input.prule_id >= item->input.fsts->set.table_len) {
        return 1;
    }
    table_elem = &item->input.fsts->set.table[item->input.prule_id];

    if (table_elem->head == NULL || item->input.offset >= table_elem->len) {
        tmp_has_empty = 1;
    }
    else {
        branch_elem = &table_elem->head[item->input.offset];

        tmp_set = branch_elem->set;
        tmp_len = branch_elem->len;
    }

    item->output.set = tmp_set;
    item->output.len = tmp_len;
    item->output.has_empty = tmp_has_empty;

    return 0;
}

static int ffset_setup_fsts_table(ffset_FirstSet *fsts, const good_Grammar *grammar)
{
    ffset_FirstSetTableElem *table = NULL;
    const size_t table_len = good_get_prules_len(grammar->prules);
    ffset_FirstSetBranchElem *branch = NULL;
    size_t branch_len;

    /*
     * tableを生成する。
     */

    table = (ffset_FirstSetTableElem *) calloc(table_len, sizeof (ffset_FirstSetTableElem));
    if (table == NULL) {
        goto FAILURE;
    }

    /*
     * 生成規則の右辺の長さの合計を取得する。
     */

    branch_len = 0;
    {
        good_ProductionRuleFilter filter;
        const good_ProductionRule *prule;

        good_set_prule_filter_match_all(&filter);
        prule = good_next_prule(&filter, grammar->prules);
        if (prule == NULL) {
            goto FAILURE;
        }
        while (prule != NULL) {
            branch_len += prule->rhs_len;

            prule = good_next_prule(&filter, grammar->prules);
        }
    }

    /*
     * branchを生成する。
     */

    {
        size_t i;

        branch = (ffset_FirstSetBranchElem *) calloc(branch_len, sizeof (ffset_FirstSetBranchElem));
        if (branch == NULL) {
            goto FAILURE;
        }
        for (i = 0; i < branch_len; i++) {
            ffset_FirstSetBranchElem *e = &branch[i];

            e->set = NULL;
            e->len = 0;
            e->has_calcurated = 0;
        }
    }

    /*
     * tableとbranchを接続する。
     */

    {
        good_ProductionRuleFilter filter;
        const good_ProductionRule *prule;
        size_t table_index;
        size_t branch_index;

        good_set_prule_filter_match_all(&filter);
        prule = good_next_prule(&filter, grammar->prules);
        if (prule == NULL) {
            goto FAILURE;
        }
        table_index = 0;
        branch_index = 0;
        while (prule != NULL) {
            ffset_FirstSetTableElem *tbl_elem = &table[table_index++];

            if (prule->rhs_len > 0) {
                tbl_elem->head = &branch[branch_index];
                tbl_elem->len = prule->rhs_len;

                branch_index += prule->rhs_len;
            }

            prule = good_next_prule(&filter, grammar->prules);
        }
    }

    fsts->set.table = table;
    fsts->set.table_len = table_len;
    fsts->set.branch = branch;
    fsts->set.branch_len = branch_len;

    return 0;

FAILURE:
    free(table);
    free(branch);

    return 1;
}

static void ffset_set_fsts_calc_frame(ffset_FirstSetCalcFrame *frame, unsigned int prule_id, size_t offset, size_t arr_bottom_index)
{
    frame->prule_id = prule_id;
    frame->offset = offset;
    frame->arr_bottom_index = arr_bottom_index;
}

static int ffset_calc_fsts_at(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, const good_Grammar *grammar)
{
    ffset_FirstSetTableElem *table_elem;
    ffset_FirstSetBranchElem *branch_elem;
    const good_ProductionRule *prule;
    const syms_SymbolID *rhs;

    frame->arr_fill_index = frame->arr_bottom_index;
    frame->has_empty = 0;
    frame->already_caluclated = 0;
    
    table_elem = &fsts->set.table[frame->prule_id];
    branch_elem = (table_elem->head != NULL)? &table_elem->head[frame->offset] : NULL;
    
    // 空規則の場合はbranch_elemがNULLになるので、その場合を考慮して事前にNULLチェックをする。
    if (branch_elem != NULL && branch_elem->has_calcurated != 0) {
        return 0;
    }

    prule = good_get_prule(grammar->prules, frame->prule_id);
    if (prule == NULL) {
        return 1;
    }

    if (prule->lhs >= grammar->terminal_symbol_id_from && prule->lhs <= grammar->terminal_symbol_id_to) {
        goto RETURN;
    }

    /*
     * 計算対象の記号列が空規則の場合
     */
    if (prule->rhs_len <= 0 || prule->rhs_len <= frame->offset) {
        frame->has_empty = 1;

        goto RETURN;
    }

    rhs = &prule->rhs[frame->offset];

    /*
     * 計算対象の記号列の先頭が終端記号の場合
     */
    if (rhs[0] >= grammar->terminal_symbol_id_from && rhs[0] <= grammar->terminal_symbol_id_to) {
        int ret;

        ret = ffset_push_fsts_symbol(fsts, frame, rhs[0]);
        if (ret != 0) {
            return 1;
        }

        goto RETURN;
    }

    /*
     * 計算対象の記号列の先頭が非終端記号の場合
     */
    {
        good_ProductionRuleFilter filter;
        const good_ProductionRule *prule;

        good_set_prule_filter_by_lhs(&filter, rhs[0]);
        prule = good_next_prule(&filter, grammar->prules);
        if (prule == NULL) {
            return 1;
        }
        while (prule != NULL) {
            ffset_FirstSetCalcFrame f;
            const good_ProductionRule *pr;
            int ret;

            // ffset_calc_fsts_at()の無限再帰を回避する。
            if (prule->id == frame->prule_id && frame->offset == 0) {
                prule = good_next_prule(&filter, grammar->prules);
                continue;
            }
            pr = good_get_prule(grammar->prules, frame->prule_id);
            if (pr == NULL) {
                return 1;
            }
            if (prule->rhs_len > 0 && prule->rhs != NULL) {
                if (prule->rhs[0] == pr->rhs[frame->offset]) {
                    prule = good_next_prule(&filter, grammar->prules);
                    continue;
                }
            }

            ffset_set_fsts_calc_frame(&f, prule->id, 0, frame->arr_fill_index);
            ret = ffset_calc_fsts_at(fsts, &f, grammar);
            if (ret != 0) {
                return 1;
            }

            ret = ffset_add_fsts(fsts, frame, f.prule_id, f.offset);
            if (ret != 0) {
                return 1;
            }

            if (f.has_empty != 0) {
                ffset_set_fsts_calc_frame(&f, frame->prule_id, frame->offset + 1, frame->arr_fill_index);
                ret = ffset_calc_fsts_at(fsts, &f, grammar);
                if (ret != 0) {
                    return 1;
                }
                ret = ffset_add_fsts(fsts, frame, f.prule_id, f.offset);
                if (ret != 0) {
                    return 1;
                }
            }

            prule = good_next_prule(&filter, grammar->prules);
        }
    }

RETURN:
    if (frame->has_empty) {
        return 0;
    }

    if ((frame->arr_fill_index - frame->arr_bottom_index) > 0) {
        syms_SymbolID *set;
        size_t i;

        set = (syms_SymbolID *) calloc(frame->arr_fill_index - frame->arr_bottom_index, sizeof (syms_SymbolID));
        if (set == NULL) {
            return 1;
        }
        for (i = frame->arr_bottom_index; i < frame->arr_fill_index; i++) {
            const syms_SymbolID *sym;

            sym = arr_get(fsts->work.arr, i);
            if (sym == NULL) {
                return 1;
            }

            set[i] = *sym;
        }

        // 空規則でない限りはbranch_elemはNULLでない想定
        if (branch_elem == NULL) {
            return 1;
        }

        branch_elem->set = set;
        branch_elem->len = frame->arr_fill_index - frame->arr_bottom_index;
    }
    branch_elem->has_calcurated = 1;

    return 0;
}

static int ffset_add_fsts(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, unsigned int prule_id, size_t offset)
{
    ffset_FirstSetTableElem *table_elem;
    ffset_FirstSetBranchElem *branch_elem;
    size_t i;

    table_elem = &fsts->set.table[prule_id];
    branch_elem = (table_elem->head != NULL)? &table_elem->head[offset] : NULL;

    if (branch_elem == NULL) {
        return 0;
    }

    if (branch_elem->has_calcurated == 0) {
        return 1;
    }

    for (i = 0; i < branch_elem->len; i++) {
        int ret;
        
        ret = ffset_push_fsts_symbol(fsts, frame, branch_elem->set[i]);
        if (ret != 0) {
            return 1;
        }
    }

    return 0;
}

static int ffset_push_fsts_symbol(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, syms_SymbolID symbol)
{
    int already_exists = 0;

    /*
     * frame->work.arrに前回計算時のゴミデータが残っている可能性を考慮して、
     * frame->arr_fill_indexとframe->arr_bottom_indexが同じ場合はarr_contain_in_range()を呼び出さないようにする。
     */
    if (frame->arr_fill_index > frame->arr_bottom_index) {
        if (arr_contain_in_range(fsts->work.arr, &symbol, frame->arr_bottom_index, frame->arr_fill_index) == c_BOOL_TRUE) {
            already_exists = 1;
        }
    }

    if (already_exists == 0) {
        void *ret;

        ret = arr_set(fsts->work.arr, frame->arr_fill_index, &symbol);
        if (ret == NULL) {
            return 1;
        }
        frame->arr_fill_index++;
    }

    return 0;
}
