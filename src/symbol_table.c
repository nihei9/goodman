#include "symbol_table.h"
#include <collections.h>
#include <stdlib.h>
#include <string.h>

struct good_SymbolTable {
	hmap_HashMap *sym2id_map;
	hmap_HashMap *id2sym_map;
	good_SymbolID id;

	good_SymbolID ret_id;
};

/*
 * good_SymbolTableオブジェクトを生成する。
 */
good_SymbolTable *good_new_symtbl(void)
{
	good_SymbolTable *symtbl = NULL;
	hmap_HashMap *sym2id_map = NULL;
	hmap_HashMap *id2sym_map = NULL;

	symtbl = (good_SymbolTable *) malloc(sizeof (good_SymbolTable));
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
 * 指定のgood_SymbolTableオブジェクトを破棄する。
 */
void good_delete_symtbl(good_SymbolTable *symtbl)
{
	if (symtbl == NULL) {
		return;
	}

	// TODO good_put_in_symtbl()内で生成したシンボルの複製の解放処理を実装する。

	hmap_delete(symtbl->sym2id_map);
	symtbl->sym2id_map = NULL;
	hmap_delete(symtbl->id2sym_map);
	symtbl->id2sym_map = NULL;
	free(symtbl);
}

/*
 * symbolをsymtblへ登録する。
 * 
 * 戻り値は内部的に保持するgood_SymbolIDへのポインタとなるため、次回呼び出し時には内容が書き換わる可能性がある。
 * よって、戻り値を受け取った呼び出し元はすぐにgood_SymbolID型の変数へと値を退避させること。
 */
const good_SymbolID *good_put_in_symtbl(good_SymbolTable *symtbl, const char *symbol, good_SymbolType type)
{
	const char *dup_sym;
	const good_SymbolID *id;
    good_SymbolID sym_id;

	if (symtbl == NULL || symbol == NULL) {
		return NULL;
	}

	id = (const good_SymbolID *) hmap_lookup(symtbl->sym2id_map, &symbol);
	if (id != NULL) {
		symtbl->ret_id = *id;

		return &symtbl->ret_id;
	}

	dup_sym = strdup(symbol);
	if (dup_sym == NULL) {
		return NULL;
	}

	switch (type) {
	case good_SYMTYPE_TERMINAL:		sym_id = symtbl->id << 1; break;
	case good_SYMTYPE_NON_TERMINAL:	sym_id = (symtbl->id << 1) | 0x1; break;
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
const char *good_lookup_in_symtbl(const good_SymbolTable *symtbl, good_SymbolID id)
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
good_SymbolType good_get_symbol_type(good_SymbolID id)
{
	if ((id & 0x1) == 0x1) {
		return good_SYMTYPE_NON_TERMINAL;
	}

	return good_SYMTYPE_TERMINAL;
}