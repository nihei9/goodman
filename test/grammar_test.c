#include "grammar.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_symbols(connie_Connie *c);

void put_symbol_as(connie_Connie *c, grm_Grammar *grm, const char *symbol, grm_SymbolType type);
void put_symbol(connie_Connie *c, grm_Grammar *grm, const char *symbol, grm_SymbolType expected_type);

int main(void)
{
  connie_Connie *c = connie_new(__FILE__);

  TEST(c, test_symbols);

  connie_print(c);

  connie_delete(c);

  return 0;
}

/*
 * 記号登録のテスト
 */
void test_symbols(connie_Connie *c)
{
	grm_Grammar *grm;
	
	grm = grm_new();
	A_NOT_NULL(c, grm);

	{
		grm_SymbolType type;

		type = grm_set_default_symbol_type(grm, grm_SYMTYPE_NON_TERMINAL);
		// grm_new()直後のデフォルト値はgrm_SYMTYPE_TERMINALとなること。
		A_EQL_INT(c, grm_SYMTYPE_TERMINAL, type);

		type = grm_set_default_symbol_type(grm, grm_SYMTYPE_TERMINAL);
		// 前段の処理で設定したgrm_SYMTYPE_NON_TERMINALが返されること。
		A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, type);

		type = grm_get_default_symbol_type(grm);
		// 前段の処理で設定したgrm_SYMTYPE_TERMINALが返されること。
		A_EQL_INT(c, grm_SYMTYPE_TERMINAL, type);
	}

	{
		put_symbol_as(c, grm, "NON-TERMINAL-1", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-2", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-3", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-4", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-5", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-6", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-7", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-8", grm_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-9", grm_SYMTYPE_NON_TERMINAL);

		put_symbol(c, grm, "TERMINAL-1", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-2", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-3", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-4", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-5", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-6", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-7", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-8", grm_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-9", grm_SYMTYPE_TERMINAL);

		put_symbol(c, grm, "NON-TERMINAL-1", grm_SYMTYPE_NON_TERMINAL);
	}

	grm_delete(grm);
}

void put_symbol_as(connie_Connie *c, grm_Grammar *grm, const char *symbol, grm_SymbolType type)
{
	const grm_SymbolID *id;
	grm_SymbolType t;
	const char *s;

	id = grm_put_symbol_as(grm, symbol, type);
	A_NOT_NULL(c, id);

	t = grm_get_symbol_type(*id);
	A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, t);

	s = grm_lookup_symbol(grm, *id);
	A_NOT_NULL(c, s);
	A_EQL_STRING(c, symbol, s);
}

void put_symbol(connie_Connie *c, grm_Grammar *grm, const char *symbol, grm_SymbolType expected_type)
{
	const grm_SymbolID *id;
	grm_SymbolType t;
	const char *s;

	id = grm_put_symbol(grm, symbol);
	A_NOT_NULL(c, id);

	t = grm_get_symbol_type(*id);
	A_EQL_INT(c, expected_type, t);

	s = grm_lookup_symbol(grm, *id);
	A_NOT_NULL(c, s);
	A_EQL_STRING(c, symbol, s);
}