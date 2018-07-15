/*
 * # 生成規則の検索パターン
 * 
 * 1. 左辺値をキーに検索する。 => 検索結果は複数の可能性あり
 * 2. 生成規則のIDをキーに検索する。 => 検索結果は常に１件
 * 3. 全件取得 => 検索結果は複数の可能性あり
 */

#include "production_rules.h"
#include <stdlib.h>
#include <string.h>

static int good_initialize_prule(unsigned int index, void *elem, void *user_data)
{
	good_ProductionRule *prule = (good_ProductionRule *) elem;

	prule->is_empty = 1;

	return 0;
}

static int good_finalize_prule(unsigned int index, void *elem, void *user_data)
{
	good_ProductionRule *prule = (good_ProductionRule *) elem;

	free(prule->rhs);
	prule->rhs = NULL;
	prule->is_empty = 1;

	return 0;
}

static int good_match_prule_by_id(unsigned int index, const void *elem, c_TypeUnion cond, void *user_data)
{
	const good_ProductionRule *prule = (const good_ProductionRule *) elem;
	unsigned int expected = cond.t_uint;

	if (prule->is_empty == 1) {
		return 0;
	}

	if (prule->id == expected) {
		return 1;
	}

	return 0;
}

static int good_match_prule_by_lhs(unsigned int index, const void *elem, c_TypeUnion cond, void *user_data)
{
	const good_ProductionRule *prule = (const good_ProductionRule *) elem;
	syms_SymbolID expected = cond.t_ulong;

	if (prule->is_empty == 1) {
		return 0;
	}

	if (prule->lhs == expected) {
		return 1;
	}

	return 0;
}

static int good_match_all_prule(unsigned int index, const void *elem, c_TypeUnion cond, void *user_data)
{
	const good_ProductionRule *prule = (const good_ProductionRule *) elem;

	if (prule->is_empty == 1) {
		return 0;
	}

	return 1;
}

struct good_ProductionRules {
	arr_Array *prs;
	size_t fill_index;
	unsigned int next_id;
};

/*
 * good_ProductionRulesオブジェクトを生成する。
 */
good_ProductionRules *good_new_prules(void)
{
	good_ProductionRules *prules;
	arr_Array *prs;
	
	prules = (good_ProductionRules *) malloc(sizeof (good_ProductionRules));
	if (prules == NULL) {
		return NULL;
	}
	
	prs = arr_new(sizeof (good_ProductionRule));
	if (prs == NULL) {
		free(prules);
		return NULL;
	}
	arr_set_initializer(prs, good_initialize_prule);
	arr_set_finalizer(prs, good_finalize_prule);
	arr_autoext_on(prs, 64);
	
	prules->prs = prs;
	prules->fill_index = 0;
	prules->next_id = 0;
	
	return prules;
}

/*
 * good_ProductionRulesを破棄する。
 */
void good_delete_prules(good_ProductionRules *prules)
{
	if (prules == NULL) {
		return;
	}
	
	arr_delete(prules->prs);
	prules->prs = NULL;
	free(prules);
}

int good_append_prule(good_ProductionRules *prules, syms_SymbolID lhs, const syms_SymbolID *rhs, size_t rhs_len)
{
	good_ProductionRule pr;
	syms_SymbolID *rhs_dup;
	
	rhs_dup = (syms_SymbolID *) malloc(sizeof (syms_SymbolID) * rhs_len);
	if (rhs_dup == NULL) {
		return 1;
	}
	memcpy(rhs_dup, rhs, sizeof (syms_SymbolID) * rhs_len);
	
	pr.id = prules->next_id++;
	pr.lhs = lhs;
	pr.rhs = rhs_dup;
	pr.rhs_len = rhs_len;
	pr.is_empty = 0;

	arr_set(prules->prs, prules->fill_index, &pr);
	prules->fill_index++;
	
	return 0;
}

size_t good_get_prules_len(const good_ProductionRules *prules)
{
	return prules->fill_index;
}

good_ProductionRuleFilter *good_set_prule_filter_by_id(good_ProductionRuleFilter *filter, unsigned int id)
{
	arr_Filter *f;

	f = arr_set_filter(&filter->prs_filter, good_match_prule_by_id, (c_TypeUnion) id);
	if (f == NULL) {
		return NULL;
	}

	return filter;
}

good_ProductionRuleFilter *good_set_prule_filter_by_lhs(good_ProductionRuleFilter *filter, syms_SymbolID lhs)
{
	arr_Filter *f;

	f = arr_set_filter(&filter->prs_filter, good_match_prule_by_lhs, (c_TypeUnion) (unsigned long) lhs);
	if (f == NULL) {
		return NULL;
	}

	return filter;
}

good_ProductionRuleFilter *good_set_prule_filter_match_all(good_ProductionRuleFilter *filter)
{
	arr_Filter *f;

	f = arr_set_filter(&filter->prs_filter, good_match_all_prule, (c_TypeUnion) NULL);
	if (f == NULL) {
		return NULL;
	}

	return filter;
}

const good_ProductionRule *good_next_prule(good_ProductionRuleFilter *filter, const good_ProductionRules *prules)
{
	return (const good_ProductionRule *) arr_next(&filter->prs_filter, prules->prs);
}
