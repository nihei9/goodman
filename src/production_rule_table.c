/*
 * # 生成規則の検索パターン
 * 
 * 1. 左辺値をキーに検索する。 => 検索結果は複数の可能性あり
 * 2. 生成規則のIDをキーに検索する。 => 検索結果は常に１件
 * 3. 全件取得 => 検索結果は複数の可能性あり
 */

#include "production_rule_table.h"
#include <stdlib.h>
#include <string.h>

struct grm_ProductionRule {
	unsigned int id;
	grm_SymbolID lhs;
	grm_SymbolID *rhs;
	size_t rhs_len;

	int is_empty;
};

unsigned int grm_get_pr_id(const grm_ProductionRule *pr)
{
	return pr->id;
}

grm_SymbolID grm_get_pr_lhs(const grm_ProductionRule *pr)
{
	return pr->lhs;
}

const grm_SymbolID *grm_get_pr_rhs(const grm_ProductionRule *pr)
{
	return pr->rhs;
}

size_t grm_get_pr_rhs_len(const grm_ProductionRule *pr)
{
	return pr->rhs_len;
}

static int grm_initialize_pr(unsigned int index, void *elem, void *user_data)
{
	grm_ProductionRule *pr = (grm_ProductionRule *) elem;

	pr->is_empty = 1;

	return 0;
}

static int grm_finalize_pr(unsigned int index, void *elem, void *user_data)
{
	grm_ProductionRule *pr = (grm_ProductionRule *) elem;

	free(pr->rhs);
	pr->rhs = NULL;
	pr->is_empty = 1;

	return 0;
}

static int grm_match_pr_by_id(unsigned int index, const void *elem, c_TypeUnion cond, void *user_data)
{
	const grm_ProductionRule *pr = (const grm_ProductionRule *) elem;
	unsigned int expected = cond.t_uint;

	if (pr->is_empty == 1) {
		return 0;
	}

	if (pr->id == expected) {
		return 1;
	}

	return 0;
}

static int grm_match_pr_by_lhs(unsigned int index, const void *elem, c_TypeUnion cond, void *user_data)
{
	const grm_ProductionRule *pr = (const grm_ProductionRule *) elem;
	grm_SymbolID expected = cond.t_ulong;

	if (pr->is_empty == 1) {
		return 0;
	}

	if (pr->lhs == expected) {
		return 1;
	}

	return 0;
}

static int grm_match_all_pr(unsigned int index, const void *elem, c_TypeUnion cond, void *user_data)
{
	const grm_ProductionRule *pr = (const grm_ProductionRule *) elem;

	if (pr->is_empty == 1) {
		return 0;
	}

	return 1;
}

struct grm_ProductionRuleTable {
	arr_Array *prs;
	size_t fill_index;
	unsigned int next_id;
};

/*
 * grm_ProductionRuleTableオブジェクトを生成する。
 */
grm_ProductionRuleTable *grm_new_prtbl(void)
{
	grm_ProductionRuleTable *prtbl;
	arr_Array *prs;
	
	prtbl = (grm_ProductionRuleTable *) malloc(sizeof (grm_ProductionRuleTable));
	if (prtbl == NULL) {
		return NULL;
	}
	
	prs = arr_new(sizeof (grm_ProductionRule));
	if (prs == NULL) {
		free(prtbl);
		return NULL;
	}
	arr_set_initializer(prs, grm_initialize_pr);
	arr_set_finalizer(prs, grm_finalize_pr);
	arr_autoext_on(prs, 64);
	
	prtbl->prs = prs;
	prtbl->fill_index = 0;
	prtbl->next_id = 0;
	
	return prtbl;
}

/*
 * grm_ProductionRuleTableを破棄する。
 */
void grm_delete_prtbl(grm_ProductionRuleTable *prtbl)
{
	if (prtbl == NULL) {
		return;
	}
	
	arr_delete(prtbl->prs);
	prtbl->prs = NULL;
	free(prtbl);
}

int grm_append_to_prtbl(grm_ProductionRuleTable *prtbl, grm_SymbolID lhs, const grm_SymbolID *rhs, size_t rhs_len)
{
	grm_ProductionRule pr;
	grm_SymbolID *rhs_dup;
	
	rhs_dup = (grm_SymbolID *) malloc(sizeof (grm_SymbolID) * rhs_len);
	if (rhs_dup == NULL) {
		return 1;
	}
	memcpy(rhs_dup, rhs, sizeof (grm_SymbolID) * rhs_len);
	
	pr.id = prtbl->next_id++;
	pr.lhs = lhs;
	pr.rhs = rhs_dup;
	pr.rhs_len = rhs_len;
	pr.is_empty = 0;

	arr_set(prtbl->prs, prtbl->fill_index, &pr);
	prtbl->fill_index++;
	
	return 0;
}

size_t grm_get_prtbl_len(const grm_ProductionRuleTable *prtbl)
{
	return prtbl->fill_index;
}

grm_ProductionRuleFilter *grm_set_pr_filter_by_id(grm_ProductionRuleFilter *filter, unsigned int id)
{
	arr_Filter *f;

	f = arr_set_filter(&filter->prs_filter, grm_match_pr_by_id, (c_TypeUnion) id);
	if (f == NULL) {
		return NULL;
	}

	return filter;
}

grm_ProductionRuleFilter *grm_set_pr_filter_by_lhs(grm_ProductionRuleFilter *filter, grm_SymbolID lhs)
{
	arr_Filter *f;

	f = arr_set_filter(&filter->prs_filter, grm_match_pr_by_lhs, (c_TypeUnion) (unsigned long) lhs);
	if (f == NULL) {
		return NULL;
	}

	return filter;
}

grm_ProductionRuleFilter *grm_set_pr_filter_match_all(grm_ProductionRuleFilter *filter)
{
	arr_Filter *f;

	f = arr_set_filter(&filter->prs_filter, grm_match_all_pr, (c_TypeUnion) NULL);
	if (f == NULL) {
		return NULL;
	}

	return filter;
}

const grm_ProductionRule *grm_next_pr(grm_ProductionRuleFilter *filter, const grm_ProductionRuleTable *prtbl)
{
	return (const grm_ProductionRule *) arr_next(&filter->prs_filter, prtbl->prs);
}
