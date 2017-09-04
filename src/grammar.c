#include "grammar.h"
#include <stdlib.h>
#include <string.h>

typedef struct grm_Grammar {
	grm_SymbolTable *symtbl;
	grm_ProductionRuleTable *prtbl;
	grm_SymbolType default_symbol_type;
	grm_SymbolID start_symbol_id;

	struct {
		grm_SymbolID *rhs;
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
	grm_SymbolTable *symtbl = NULL;
	grm_ProductionRuleTable *prtbl = NULL;
	grm_SymbolID *rhs = NULL;
	
	grm = (grm_Grammar *) malloc(sizeof (grm_Grammar));
	if (grm == NULL) {
		goto FAILURE;
	}
	symtbl = grm_new_symtbl();
	if (symtbl == NULL) {
		goto FAILURE;
	}
	prtbl = grm_new_prtbl();
	if (prtbl == NULL) {
		goto FAILURE;
	}
	rhs = (grm_SymbolID *) malloc(sizeof (grm_SymbolID) * RHS_WORK_INITIAL_SIZE);
	if (rhs == NULL) {
		goto FAILURE;
	}
	
	grm->symtbl = symtbl;
	grm->prtbl = prtbl;
	grm->default_symbol_type = grm_SYMTYPE_TERMINAL;
	grm->pr_rhs_work.rhs = rhs;
	grm->pr_rhs_work.len = RHS_WORK_INITIAL_SIZE;
	grm->pr_rhs_work.fill_index = 0;
	
	return grm;

FAILURE:
	free(grm);
	grm = NULL;
	grm_delete_symtbl(symtbl);
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
	
	grm_delete_symtbl(grm->symtbl);
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
grm_SymbolType grm_get_default_symbol_type(const grm_Grammar *grm)
{
	return grm->default_symbol_type;
}

/*
 * typeをデフォルトのSymbolTypeとして設定する。戻り値は設定変更前の値となる。
 */
grm_SymbolType grm_set_default_symbol_type(grm_Grammar *grm, grm_SymbolType type)
{
	grm_SymbolType before_type = grm->default_symbol_type;

	grm->default_symbol_type = type;

	return before_type;
}

/*
 * symbolをIDへ変換する。SymbolTypeはデフォルト値が使用される。
 */
const grm_SymbolID *grm_put_symbol(grm_Grammar *grm, const char *symbol)
{
	return grm_put_in_symtbl(grm->symtbl, symbol, grm->default_symbol_type);
}

/*
 * symbolをIDへ変換する。SymbolTypeはtypeで指定されたものとなる。
 */
const grm_SymbolID *grm_put_symbol_as(grm_Grammar *grm, const char *symbol, grm_SymbolType type)
{
	return grm_put_in_symtbl(grm->symtbl, symbol, type);
}

/*
 * idに対応する記号の文字列表現を取得する。
 */
const char *grm_lookup_symbol(const grm_Grammar *grm, grm_SymbolID id)
{
	return grm_lookup_in_symtbl(grm->symtbl, id);
}

/*
 * idを開始記号として登録する。
 */
grm_SymbolID grm_set_start_symbol(grm_Grammar *grm, grm_SymbolID id)
{
	grm->start_symbol_id = id;

	return id;
}


/*
 * 生成規則を登録する。
 */
int grm_append_prule(grm_Grammar *grm, const char *lhs, const char *rhs[], size_t rhs_len)
{
	const grm_SymbolID *lhs_id;
	unsigned int i;

	if (grm->pr_rhs_work.len < rhs_len) {
		grm_SymbolID *new_rhs;
		const size_t deficient_len = rhs_len - grm->pr_rhs_work.len;
		const size_t deficient_step = deficient_len / RHS_WORK_STEP_SIZE + (deficient_len % RHS_WORK_STEP_SIZE > 0)? 1 : 0;
		const size_t new_len = grm->pr_rhs_work.len + deficient_step * RHS_WORK_STEP_SIZE;

		new_rhs = (grm_SymbolID *) realloc(grm->pr_rhs_work.rhs, sizeof (grm_SymbolID) * new_len);
		if (new_rhs == NULL) {
			return 1;
		}
		grm->pr_rhs_work.rhs = new_rhs;
		grm->pr_rhs_work.len = new_len;
	}

	lhs_id = grm_put_symbol(grm, lhs);
	if (lhs_id == NULL) {
		return 1;
	}

	for (i = 0; i < rhs_len; i++) {
		const grm_SymbolID *id;

		id = grm_put_symbol(grm, rhs[i]);
		if (id == NULL) {
			return 1;
		}
		grm->pr_rhs_work.rhs[i] = *id;
	}

	grm_append_to_prtbl(grm->prtbl, *lhs_id, grm->pr_rhs_work.rhs, rhs_len);

	return 0;
}
