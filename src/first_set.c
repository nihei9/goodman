#include "first_set.h"

typedef struct ffset_FirstSetBranchElem {
    grm_SymbolID *set;
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

static int ffset_setup_fsts_table(ffset_FirstSet *fsts, const grm_Grammar *grm);
static void ffset_set_fsts_calc_frame(ffset_FirstSetCalcFrame *frame, unsigned int prule_id, size_t offset, size_t arr_bottom_index);
static int ffset_calc_fsts_at(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, const grm_Grammar *grm);
static int ffset_add_fsts(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, unsigned int prule_id, size_t offset);
static int ffset_push_fsts_symbol(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, grm_SymbolID symbol);

ffset_FirstSet *ffset_new_fsts(void)
{
    ffset_FirstSet *fsts = NULL;
    arr_Array *arr = NULL;

    fsts = (ffset_FirstSet *) malloc(sizeof (ffset_FirstSet));
    if (fsts == NULL) {
        goto FAILURE;
    }

    arr = arr_new(sizeof (grm_SymbolID));
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

int ffset_calc_fsts(ffset_FirstSet *fsts, const grm_Grammar *grm)
{
    int ret;

    // FIRST集合を保持するテーブル領域を確保する。
    ret = ffset_setup_fsts_table(fsts, grm);
    if (ret != 0) {
        return 1;
    }

    // FIRST集合を計算する。
    {
        grm_ProductionRuleFilter pr_filter_entity;
        grm_ProductionRuleFilter *pr_filter;
        const grm_ProductionRule *pr;

        pr_filter = grm_set_pr_filter_match_all(&pr_filter_entity);
        if (pr_filter == NULL) {
            return 1;
        }

        pr = grm_next_prule(grm, pr_filter);
        if (pr == NULL) {
            return 1;
        }
        while (pr != NULL) {
            const size_t rhs_len = grm_get_pr_rhs_len(pr);
            size_t i;

            for (i = 0; i < rhs_len; i++) {
                ffset_FirstSetCalcFrame frame;
                int ret;

                ffset_set_fsts_calc_frame(&frame, grm_get_pr_id(pr), i, 0);
                ret = ffset_calc_fsts_at(fsts, &frame, grm);
                if (ret != 0) {
                    return 1;
                }
            }

            pr = grm_next_prule(grm, pr_filter);
        }
    }

    return 0;
}

int ffset_get_fsts(ffset_FirstSetItem *item)
{
    ffset_FirstSetTableElem *table_elem;
    ffset_FirstSetBranchElem *branch_elem;
    grm_SymbolID *tmp_set = NULL;
    size_t tmp_len = 0;
    int tmp_has_empty = 0;

    table_elem = &item->input.fsts->set.table[item->input.prule_id];
    branch_elem = (table_elem->head != NULL)? &table_elem->head[item->input.offset] : NULL;

    if (table_elem->len <= 0) {
        tmp_has_empty = 1;
    }
    else {
        tmp_set = branch_elem->set;
        tmp_len = branch_elem->len;
    }

    item->output.set = tmp_set;
    item->output.len = tmp_len;
    item->output.has_empty = tmp_has_empty;

    return 0;
}

static int ffset_setup_fsts_table(ffset_FirstSet *fsts, const grm_Grammar *grm)
{
    ffset_FirstSetTableElem *table = NULL;
    const size_t table_len = grm_get_num_of_prule(grm);
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
        grm_ProductionRuleFilter pr_filter_entity;
        grm_ProductionRuleFilter *pr_filter;
        const grm_ProductionRule *pr;

        pr_filter = grm_set_pr_filter_match_all(&pr_filter_entity);
        if (pr_filter == NULL) {
            goto FAILURE;
        }
        pr = grm_next_prule(grm, pr_filter);
        if (pr == NULL) {
            goto FAILURE;
        }
        while (pr != NULL) {
            branch_len += grm_get_pr_rhs_len(pr);

            pr = grm_next_prule(grm, pr_filter);
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
        grm_ProductionRuleFilter pr_filter_entity;
        grm_ProductionRuleFilter *pr_filter;
        const grm_ProductionRule *pr;
        size_t table_index;
        size_t branch_index;

        pr_filter = grm_set_pr_filter_match_all(&pr_filter_entity);
        if (pr_filter == NULL) {
            goto FAILURE;
        }
        pr = grm_next_prule(grm, pr_filter);
        if (pr == NULL) {
            goto FAILURE;
        }
        table_index = 0;
        branch_index = 0;
        while (pr != NULL) {
            ffset_FirstSetTableElem *tbl_elem = &table[table_index++];
            const size_t rhs_len = grm_get_pr_rhs_len(pr);

            if (rhs_len > 0) {
                tbl_elem->head = &branch[branch_index];
                tbl_elem->len = rhs_len;

                branch_index += rhs_len;
            }

            pr = grm_next_prule(grm, pr_filter);
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

static int ffset_calc_fsts_at(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, const grm_Grammar *grm)
{
    ffset_FirstSetTableElem *table_elem;
    ffset_FirstSetBranchElem *branch_elem;
    const grm_ProductionRule *pr;
    const grm_SymbolID *rhs;

    frame->arr_fill_index = frame->arr_bottom_index;
    frame->has_empty = 0;
    frame->already_caluclated = 0;
    
    table_elem = &fsts->set.table[frame->prule_id];
    branch_elem = (table_elem->head != NULL)? &table_elem->head[frame->offset] : NULL;
    
    if (branch_elem != NULL && branch_elem->has_calcurated != 0) {
        return 0;
    }

    pr = grm_find_prule_by_id(grm, frame->prule_id);
    if (pr == NULL) {
        return 1;
    }

    /*
     * 計算対象の記号列が空規則の場合
     */
    if (grm_get_pr_rhs_len(pr) <= 0 || grm_get_pr_rhs_len(pr) <= frame->offset) {
        frame->has_empty = 1;

        return 0;
    }

    /*
     * 計算対象の記号列の先頭が終端記号の場合
     */
    rhs = grm_get_pr_rhs(pr);
    rhs = &rhs[frame->offset];
    if (grm_get_symbol_type(rhs[0]) == grm_SYMTYPE_TERMINAL) {
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
        grm_ProductionRuleFilter pr_filter_entity;
        grm_ProductionRuleFilter *pr_filter;
        const grm_ProductionRule *pr;

        pr_filter = grm_find_prule_by_lhs(grm, rhs[0], &pr_filter_entity);
        if (pr_filter == NULL) {
            return 1;
        }
        pr = grm_next_prule(grm, pr_filter);
        if (pr == NULL) {
            return 1;
        }
        while (pr != NULL) {
            unsigned int prule_id;
            ffset_FirstSetCalcFrame f;
            int ret;

            // ffset_calc_fsts_at()の無限再帰を避けるため、計算中の記号列と同じものは除外する。
            prule_id = grm_get_pr_id(pr);
            if (prule_id == frame->prule_id && frame->offset == 0) {
                pr = grm_next_prule(grm, pr_filter);
                continue;
            }

            ffset_set_fsts_calc_frame(&f, grm_get_pr_id(pr), 0, frame->arr_fill_index);
            ret = ffset_calc_fsts_at(fsts, &f, grm);
            if (ret != 0) {
                return 1;
            }

            ret = ffset_add_fsts(fsts, frame, f.prule_id, f.offset);
            if (ret != 0) {
                return 1;
            }

            if (f.has_empty != 0) {
                ffset_set_fsts_calc_frame(&f, frame->prule_id, frame->offset + 1, frame->arr_fill_index);
                ret = ffset_calc_fsts_at(fsts, &f, grm);
                if (ret != 0) {
                    return 1;
                }

                ret = ffset_add_fsts(fsts, frame, f.prule_id, f.offset);
                if (ret != 0) {
                    return 1;
                }
            }

            pr = grm_next_prule(grm, pr_filter);
        }
    }

RETURN:
    if ((frame->arr_fill_index - frame->arr_bottom_index) > 0) {
        grm_SymbolID *set;
        size_t i;

        set = (grm_SymbolID *) calloc(frame->arr_fill_index - frame->arr_bottom_index, sizeof (grm_SymbolID));
        if (set == NULL) {
            return 1;
        }
        for (i = frame->arr_bottom_index; i < frame->arr_fill_index; i++) {
            const grm_SymbolID *sym;

            sym = arr_get(fsts->work.arr, i);
            if (sym == NULL) {
                return 1;
            }

            set[i] = *sym;
        }

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

static int ffset_push_fsts_symbol(ffset_FirstSet *fsts, ffset_FirstSetCalcFrame *frame, grm_SymbolID symbol)
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
