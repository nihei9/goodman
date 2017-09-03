#include "grammar.h"
#include <stdlib.h>
#include <string.h>

typedef struct grm_Grammar {
	grm_SymbolTable *symtbl;
	grm_ProductionRuleTable *prtbl;
	grm_SymbolType default_symbol_type;
	grm_SymbolID start_symbol_id;
} grm_Grammar;

/*
 * grm_Grammarを生成する。
 */
grm_Grammar *grm_new(void)
{
	grm_Grammar *grm = NULL;
	grm_SymbolTable *symtbl = NULL;
	grm_ProductionRuleTable *prtbl = NULL;
	
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
	
	grm->symtbl = symtbl;
	grm->prtbl = prtbl;
	grm->default_symbol_type = grm_SYMTYPE_TERMINAL;
	
	return grm;

FAILURE:
	free(grm);
	grm = NULL;
	grm_delete_symtbl(symtbl);
	symtbl = NULL;
	grm_delete_prtbl(prtbl);
	prtbl = NULL;

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
grm_SymbolID *grm_put_symbol(grm_Grammar *grm, const char *symbol)
{
	return grm_put_in_symtbl(grm->symtbl, symbol, grm->default_symbol_type);
}

/*
 * symbolをIDへ変換する。SymbolTypeはtypeで指定されたものとなる。
 */
grm_SymbolID *grm_put_symbol_as(grm_Grammar *grm, const char *symbol, grm_SymbolType type)
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
