#include "symbol_table.h"
#include <collections.h>
#include <stdlib.h>
#include <string.h>

struct grm_SymbolTable {
	hmap_HashMap *sym2id_map;
	hmap_HashMap *id2sym_map;
	grm_SymbolID id;

	grm_SymbolID ret_id;
};

/*
 * grm_SymbolTableオブジェクトを生成する。
 */
grm_SymbolTable *grm_new_symtbl(void)
{
	grm_SymbolTable *symtbl = NULL;
	hmap_HashMap *sym2id_map = NULL;
	hmap_HashMap *id2sym_map = NULL;

	symtbl = (grm_SymbolTable *) malloc(sizeof (grm_SymbolTable));
	if (symtbl == NULL) {
		goto FAILURE;
	}

	sym2id_map = hmap_new(c_TYPE_STRING, c_TYPE_ULONG);
	if (sym2id_map == NULL) {
		goto FAILURE;
	}
	id2sym_map = hmap_new(c_TYPE_ULONG, c_TYPE_STRING);
	if (id2sym_map == NULL) {
		goto FAILURE;
	}
	symtbl->sym2id_map = sym2id_map;
	symtbl->id2sym_map = id2sym_map;
	symtbl->id = 0;
	symtbl->ret_id = 0;

	return symtbl;

FAILURE:
	free(symtbl);
	symtbl = NULL;
	hmap_delete(sym2id_map);
	sym2id_map = NULL;
	hmap_delete(id2sym_map);
	id2sym_map = NULL;

	return NULL;
}

/*
 * 指定のgrm_SymbolTableオブジェクトを破棄する。
 */
void grm_delete_symtbl(grm_SymbolTable *symtbl)
{
	if (symtbl == NULL) {
		return;
	}

	// TODO grm_put_in_symtbl()内で生成したシンボルの複製の解放処理を実装する。

	hmap_delete(symtbl->sym2id_map);
	symtbl->sym2id_map = NULL;
	hmap_delete(symtbl->id2sym_map);
	symtbl->id2sym_map = NULL;
	free(symtbl);
}

/*
 * symbolをsymtblへ登録する。
 * 
 * 戻り値は内部的に保持するgrm_SymbolIDへのポインタとなるため、次回呼び出し時には内容が書き換わる可能性がある。
 * よって、戻り値を受け取った呼び出し元はすぐにgrm_SymbolID型の変数へと値を退避させること。
 */
const grm_SymbolID *grm_put_in_symtbl(grm_SymbolTable *symtbl, const char *symbol, grm_SymbolType type)
{
	const char *dup_sym;
	const grm_SymbolID *id;
    grm_SymbolID sym_id;

	if (symtbl == NULL || symbol == NULL) {
		return NULL;
	}

	id = (const grm_SymbolID *) hmap_lookup(symtbl->sym2id_map, &symbol);
	if (id != NULL) {
		symtbl->ret_id = *id;

		return &symtbl->ret_id;
	}

	dup_sym = strdup(symbol);
	if (dup_sym == NULL) {
		return NULL;
	}

	switch (type) {
	case grm_SYMTYPE_TERMINAL:		sym_id = symtbl->id << 1; break;
	case grm_SYMTYPE_NON_TERMINAL:	sym_id = (symtbl->id << 1) | 0x1; break;
	default:						return NULL;
	}
	if (hmap_put(symtbl->sym2id_map, &dup_sym, &sym_id) == NULL) {
		return NULL;
	}
	if (hmap_put(symtbl->id2sym_map, &sym_id, &dup_sym) == NULL) {
		return NULL;
	}
	
	symtbl->ret_id = sym_id;
    symtbl->id++;

	return &symtbl->ret_id;
}

/*
 * idに対応する記号の文字列表現を返す。
 */
const char *grm_lookup_in_symtbl(const grm_SymbolTable *symtbl, grm_SymbolID id)
{
	void *sym;

	if (symtbl == NULL) {
		return NULL;
	}

	sym = hmap_lookup(symtbl->id2sym_map, &id);
	if (sym == NULL) {
		return NULL;
	}

	return *((const char **) sym);
}

/*
 * idの記号種別を取得する。
 */
grm_SymbolType grm_get_symbol_type(grm_SymbolID id)
{
	if ((id & 0x1) == 0x1) {
		return grm_SYMTYPE_NON_TERMINAL;
	}

	return grm_SYMTYPE_TERMINAL;
}