#include "grammar.h"
#include <connie_sugar.h>
#include <stdio.h>

static void test_symbols(connie_Connie *c);
static void test_prules(connie_Connie *c);

static void put_symbol_as(connie_Connie *c, grm_Grammar *grm, const char *symbol, good_SymbolType type);
static void put_symbol(connie_Connie *c, grm_Grammar *grm, const char *symbol, good_SymbolType expected_type);

int main(void)
{
  connie_Connie *c = connie_new(__FILE__);

  TEST(c, test_symbols);
  TEST(c, test_prules);

  connie_print(c);

  connie_delete(c);

  return 0;
}

/*
 * 記号登録のテスト
 */
static void test_symbols(connie_Connie *c)
{
	grm_Grammar *grm;
	
	grm = grm_new();
	A_NOT_NULL(c, grm);

	{
		good_SymbolType type;

		type = grm_set_default_symbol_type(grm, good_SYMTYPE_NON_TERMINAL);
		// grm_new()直後のデフォルト値はgood_SYMTYPE_TERMINALとなること。
		A_EQL_INT(c, good_SYMTYPE_TERMINAL, type);

		type = grm_set_default_symbol_type(grm, good_SYMTYPE_TERMINAL);
		// 前段の処理で設定したgood_SYMTYPE_NON_TERMINALが返されること。
		A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, type);

		type = grm_get_default_symbol_type(grm);
		// 前段の処理で設定したgood_SYMTYPE_TERMINALが返されること。
		A_EQL_INT(c, good_SYMTYPE_TERMINAL, type);
	}

	{
		put_symbol_as(c, grm, "NON-TERMINAL-1", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-2", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-3", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-4", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-5", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-6", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-7", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-8", good_SYMTYPE_NON_TERMINAL);
		put_symbol_as(c, grm, "NON-TERMINAL-9", good_SYMTYPE_NON_TERMINAL);

		put_symbol(c, grm, "TERMINAL-1", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-2", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-3", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-4", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-5", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-6", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-7", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-8", good_SYMTYPE_TERMINAL);
		put_symbol(c, grm, "TERMINAL-9", good_SYMTYPE_TERMINAL);

		put_symbol(c, grm, "NON-TERMINAL-1", good_SYMTYPE_NON_TERMINAL);
	}

	grm_delete(grm);
}

#define LEN(rhs) (sizeof (rhs) / sizeof (char *))

static void test_prules(connie_Connie *c)
{
	grm_Grammar *grm;

	grm = grm_new();
	A_NOT_NULL(c, grm);

	grm_set_default_symbol_type(grm, good_SYMTYPE_TERMINAL);
	grm_put_symbol(grm, "id");
	grm_put_symbol(grm, "+");
	grm_put_symbol(grm, "*");
	grm_put_symbol(grm, "(");
	grm_put_symbol(grm, ")");
	
	{
        const char *rhs_E_1[] = {"E", "+", "T"};
        const char *rhs_E_2[] = {"T"};
        const char *rhs_T_1[] = {"T", "*", "F"};
        const char *rhs_T_2[] = {"F"};
        const char *rhs_F_1[] = {"(", "E", ")"};
        const char *rhs_F_2[] = {"id"};
		int ret;

        grm_set_default_symbol_type(grm, good_SYMTYPE_NON_TERMINAL);
        ret = grm_append_prule(grm, "E", rhs_E_1, LEN(rhs_E_1));
		A_EQL_INT(c, 0, ret);
        ret = grm_append_prule(grm, "E", rhs_E_2, LEN(rhs_E_2));
		A_EQL_INT(c, 0, ret);
        ret = grm_append_prule(grm, "T", rhs_T_1, LEN(rhs_T_1));
		A_EQL_INT(c, 0, ret);
        ret = grm_append_prule(grm, "T", rhs_T_2, LEN(rhs_T_2));
		A_EQL_INT(c, 0, ret);
        ret = grm_append_prule(grm, "F", rhs_F_1, LEN(rhs_F_1));
		A_EQL_INT(c, 0, ret);
        ret = grm_append_prule(grm, "F", rhs_F_2, LEN(rhs_F_2));
		A_EQL_INT(c, 0, ret);
    }

	{
		const grm_ProductionRule *prule;
		grm_ProductionRuleFilter filter;
		grm_ProductionRuleFilter *f;

		f = grm_find_all_prule(grm, &filter);
		A_NOT_NULL(c, f);

		prule = grm_next_prule(grm, f);
		A_NOT_NULL(c, prule);
		{
			good_SymbolID lhs = grm_get_pr_lhs(prule);
			const good_SymbolID *rhs = grm_get_pr_rhs(prule);
			size_t rhs_len = grm_get_pr_rhs_len(prule);
			size_t i;

			printf("lhs => %s <%lu>\n", grm_lookup_symbol(grm, lhs), lhs);
			printf("rhs => ");
			for (i = 0; i < rhs_len; i++) {
				printf("%s <%lu> ", grm_lookup_symbol(grm, rhs[i]), rhs[i]);
			}
			printf("\n");
		}
		//A_EQL_STRING(c, "E", grm_lookup_symbol(grm, grm_get_pr_lhs(prule)));

		prule = grm_next_prule(grm, f);
		A_NOT_NULL(c, prule);
		{
			good_SymbolID lhs = grm_get_pr_lhs(prule);
			const good_SymbolID *rhs = grm_get_pr_rhs(prule);
			size_t rhs_len = grm_get_pr_rhs_len(prule);
			size_t i;

			printf("lhs => %s <%lu>\n", grm_lookup_symbol(grm, lhs), lhs);
			printf("rhs => ");
			for (i = 0; i < rhs_len; i++) {
				printf("%s <%lu> ", grm_lookup_symbol(grm, rhs[i]), rhs[i]);
			}
			printf("\n");
		}
		//A_EQL_STRING(c, "E", grm_lookup_symbol(grm, grm_get_pr_lhs(prule)));
		
		prule = grm_next_prule(grm, f);
		A_NOT_NULL(c, prule);
		{
			good_SymbolID lhs = grm_get_pr_lhs(prule);
			const good_SymbolID *rhs = grm_get_pr_rhs(prule);
			size_t rhs_len = grm_get_pr_rhs_len(prule);
			size_t i;

			printf("lhs => %s <%lu>\n", grm_lookup_symbol(grm, lhs), lhs);
			printf("rhs => ");
			for (i = 0; i < rhs_len; i++) {
				printf("%s <%lu> ", grm_lookup_symbol(grm, rhs[i]), rhs[i]);
			}
			printf("\n");
		}
		//A_EQL_STRING(c, "T", grm_lookup_symbol(grm, grm_get_pr_lhs(prule)));
		
		prule = grm_next_prule(grm, f);
		A_NOT_NULL(c, prule);
		{
			good_SymbolID lhs = grm_get_pr_lhs(prule);
			const good_SymbolID *rhs = grm_get_pr_rhs(prule);
			size_t rhs_len = grm_get_pr_rhs_len(prule);
			size_t i;

			printf("lhs => %s <%lu>\n", grm_lookup_symbol(grm, lhs), lhs);
			printf("rhs => ");
			for (i = 0; i < rhs_len; i++) {
				printf("%s <%lu> ", grm_lookup_symbol(grm, rhs[i]), rhs[i]);
			}
			printf("\n");
		}
		//A_EQL_STRING(c, "T", grm_lookup_symbol(grm, grm_get_pr_lhs(prule)));
		
		prule = grm_next_prule(grm, f);
		A_NOT_NULL(c, prule);
		{
			good_SymbolID lhs = grm_get_pr_lhs(prule);
			const good_SymbolID *rhs = grm_get_pr_rhs(prule);
			size_t rhs_len = grm_get_pr_rhs_len(prule);
			size_t i;

			printf("lhs => %s <%lu>\n", grm_lookup_symbol(grm, lhs), lhs);
			printf("rhs => ");
			for (i = 0; i < rhs_len; i++) {
				printf("%s <%lu> ", grm_lookup_symbol(grm, rhs[i]), rhs[i]);
			}
			printf("\n");
		}
		//A_EQL_STRING(c, "F", grm_lookup_symbol(grm, grm_get_pr_lhs(prule)));
		
		prule = grm_next_prule(grm, f);
		A_NOT_NULL(c, prule);
		{
			good_SymbolID lhs = grm_get_pr_lhs(prule);
			const good_SymbolID *rhs = grm_get_pr_rhs(prule);
			size_t rhs_len = grm_get_pr_rhs_len(prule);
			size_t i;

			printf("lhs => %s <%lu>\n", grm_lookup_symbol(grm, lhs), lhs);
			printf("rhs => ");
			for (i = 0; i < rhs_len; i++) {
				printf("%s <%lu> ", grm_lookup_symbol(grm, rhs[i]), rhs[i]);
			}
			printf("\n");
		}
		//A_EQL_STRING(c, "F", grm_lookup_symbol(grm, grm_get_pr_lhs(prule)));

		prule = grm_next_prule(grm, f);
		A_NULL(c, prule);
	}

	grm_delete(grm);
}

static void put_symbol_as(connie_Connie *c, grm_Grammar *grm, const char *symbol, good_SymbolType type)
{
	const good_SymbolID *id;
	good_SymbolType t;
	const char *s;

	id = grm_put_symbol_as(grm, symbol, type);
	A_NOT_NULL(c, id);

	t = good_get_symbol_type(*id);
	A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, t);

	s = grm_lookup_symbol(grm, *id);
	A_NOT_NULL(c, s);
	A_EQL_STRING(c, symbol, s);
}

static void put_symbol(connie_Connie *c, grm_Grammar *grm, const char *symbol, good_SymbolType expected_type)
{
	const good_SymbolID *id;
	good_SymbolType t;
	const char *s;

	id = grm_put_symbol(grm, symbol);
	A_NOT_NULL(c, id);

	t = good_get_symbol_type(*id);
	A_EQL_INT(c, expected_type, t);

	s = grm_lookup_symbol(grm, *id);
	A_NOT_NULL(c, s);
	A_EQL_STRING(c, symbol, s);
}