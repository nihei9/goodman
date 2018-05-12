#include "grammar.h"
#include <stdlib.h>
#include <string.h>

typedef struct grm_Grammar {
	good_SymbolTable *symtbl;
	grm_ProductionRuleTable *prtbl;
	good_SymbolType default_symbol_type;
	good_SymbolID start_symbol_id;

	struct {
		good_SymbolID *rhs;
		size_t len;
		unsigned int fill_index;
	} pr_rhs_work;
} grm_Grammar;

#define RHS_WORK_INITIAL_SIZE 128
#define RHS_WORK_STEP_SIZE 128

/*
 * grm_Grammarを生成する。
 */
grm_Grammar *grm_new(void)
{
	grm_Grammar *grm = NULL;
	good_SymbolTable *symtbl = NULL;
	grm_ProductionRuleTable *prtbl = NULL;
	good_SymbolID *rhs = NULL;
	
	grm = (grm_Grammar *) malloc(sizeof (grm_Grammar));
	if (grm == NULL) {
		goto FAILURE;
	}
	symtbl = good_new_symtbl();
	if (symtbl == NULL) {
		goto FAILURE;
	}
	prtbl = grm_new_prtbl();
	if (prtbl == NULL) {
		goto FAILURE;
	}
	rhs = (good_SymbolID *) malloc(sizeof (good_SymbolID) * RHS_WORK_INITIAL_SIZE);
	if (rhs == NULL) {
		goto FAILURE;
	}
	
	grm->symtbl = symtbl;
	grm->prtbl = prtbl;
	grm->default_symbol_type = good_SYMTYPE_TERMINAL;
	grm->pr_rhs_work.rhs = rhs;
	grm->pr_rhs_work.len = RHS_WORK_INITIAL_SIZE;
	grm->pr_rhs_work.fill_index = 0;
	
	return grm;

FAILURE:
	free(grm);
	grm = NULL;
	good_delete_symtbl(symtbl);
	symtbl = NULL;
	grm_delete_prtbl(prtbl);
	prtbl = NULL;
	free(rhs);
	rhs = NULL;

	return NULL;
}

/*
 * grmを破棄する。
 */
void grm_delete(grm_Grammar *grm)
{
	if (grm == NULL) {
		return;
	}
	
	good_delete_symtbl(grm->symtbl);
	grm->symtbl = NULL;
	grm_delete_prtbl(grm->prtbl);
	grm->prtbl = NULL;
	free(grm->pr_rhs_work.rhs);
	grm->pr_rhs_work.rhs = NULL;

	free(grm);
}

/*
 * デフォルトのSymbolTypeを取得する。
 */
good_SymbolType grm_get_default_symbol_type(const grm_Grammar *grm)
{
	return grm->default_symbol_type;
}

/*
 * typeをデフォルトのSymbolTypeとして設定する。戻り値は設定変更前の値となる。
 */
good_SymbolType grm_set_default_symbol_type(grm_Grammar *grm, good_SymbolType type)
{
	good_SymbolType before_type = grm->default_symbol_type;

	grm->default_symbol_type = type;

	return before_type;
}

/*
 * symbolをIDへ変換する。SymbolTypeはデフォルト値が使用される。
 */
const good_SymbolID *grm_put_symbol(grm_Grammar *grm, const char *symbol)
{
	return good_put_in_symtbl(grm->symtbl, symbol, grm->default_symbol_type);
}

/*
 * symbolをIDへ変換する。SymbolTypeはtypeで指定されたものとなる。
 */
const good_SymbolID *grm_put_symbol_as(grm_Grammar *grm, const char *symbol, good_SymbolType type)
{
	return good_put_in_symtbl(grm->symtbl, symbol, type);
}

/*
 * idに対応する記号の文字列表現を取得する。
 */
const char *grm_lookup_symbol(const grm_Grammar *grm, good_SymbolID id)
{
	return good_lookup_in_symtbl(grm->symtbl, id);
}

/*
 * idを開始記号として登録する。
 */
good_SymbolID grm_set_start_symbol(grm_Grammar *grm, good_SymbolID id)
{
	grm->start_symbol_id = id;

	return id;
}

/*
 * 開始記号を取得する。
 * この関数は必ず grm_set_start_symbol() で開始記号を登録した後に呼ぶこと。
 */
good_SymbolID grm_get_start_symbol(const grm_Grammar *grm)
{
	return grm->start_symbol_id;
}

/*
 * 生成規則の数を返す。
 */
size_t grm_get_num_of_prule(const grm_Grammar *grm)
{
	return grm_get_prtbl_len(grm->prtbl);
}

/*
 * 生成規則を登録する。
 */
int grm_append_prule(grm_Grammar *grm, const char *lhs, const char *rhs[], size_t rhs_len)
{
	const good_SymbolID *tmp_lhs_id;
	good_SymbolID lhs_id;
	unsigned int i;
	int ret;

	if (grm->pr_rhs_work.len < rhs_len) {
		good_SymbolID *new_rhs;
		const size_t deficient_len = rhs_len - grm->pr_rhs_work.len;
		const size_t deficient_step = deficient_len / RHS_WORK_STEP_SIZE + (deficient_len % RHS_WORK_STEP_SIZE > 0)? 1 : 0;
		const size_t new_len = grm->pr_rhs_work.len + deficient_step * RHS_WORK_STEP_SIZE;

		new_rhs = (good_SymbolID *) realloc(grm->pr_rhs_work.rhs, sizeof (good_SymbolID) * new_len);
		if (new_rhs == NULL) {
			return 1;
		}
		grm->pr_rhs_work.rhs = new_rhs;
		grm->pr_rhs_work.len = new_len;
	}

	tmp_lhs_id = grm_put_symbol(grm, lhs);
	if (tmp_lhs_id == NULL) {
		return 1;
	}
	lhs_id = *tmp_lhs_id;

	for (i = 0; i < rhs_len; i++) {
		const good_SymbolID *id;

		id = grm_put_symbol(grm, rhs[i]);
		if (id == NULL) {
			return 1;
		}
		grm->pr_rhs_work.rhs[i] = *id;
	}

	ret = grm_append_to_prtbl(grm->prtbl, lhs_id, grm->pr_rhs_work.rhs, rhs_len);
	if (ret != 0) {
		return 1;
	}

	return 0;
}

/*
 * 指定のidに対応する生成規則を取得する。
 */
const grm_ProductionRule *grm_find_prule_by_id(const grm_Grammar *grm, unsigned int prule_id)
{
	grm_ProductionRuleFilter f;

	grm_set_pr_filter_by_id(&f, prule_id);
	
	return grm_next_pr(&f, grm->prtbl);
}

/*
 * 指定の左辺値を持つ生成規則を取得する。
 */
grm_ProductionRuleFilter *grm_find_prule_by_lhs(const grm_Grammar *grm, good_SymbolID lhs, grm_ProductionRuleFilter *filter)
{
	return grm_set_pr_filter_by_lhs(filter, lhs);
}

/*
 * 全ての生成規則を取得する。
 */
grm_ProductionRuleFilter *grm_find_all_prule(const grm_Grammar *grm, grm_ProductionRuleFilter *filter)
{
	return grm_set_pr_filter_match_all(filter);
}

/*
 * 生成規則のセットから、要素をひとつずつ取得する。
 */
const grm_ProductionRule *grm_next_prule(const grm_Grammar *grm, grm_ProductionRuleFilter *filter)
{
	return grm_next_pr(filter, grm->prtbl);
}
