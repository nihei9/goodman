#include "follow_set.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_followset(connie_Connie *c);

int main(void)
{
  connie_Connie *c = connie_new(__FILE__);

  TEST(c, test_followset);

  connie_print(c);

  connie_delete(c);

  return 0;
}

#define RHS_LEN(rhs) (sizeof (rhs) / sizeof (char *))

void test_followset(connie_Connie *c)
{
    grm_Grammar *grm;
    ffset_FollowSet *flws;
    ffset_FirstSet *fsts;
    int ret;

    {
        const char *rhs_E_1[] = {"E", "+", "T"};
        const char *rhs_E_2[] = {"T"};
        const char *rhs_T_1[] = {"T", "*", "F"};
        const char *rhs_T_2[] = {"F"};
        const char *rhs_F_1[] = {"(", "E", ")"};
        const char *rhs_F_2[] = {"id"};
        const grm_SymbolID *start_symbol;

        grm = grm_new();

        grm_set_default_symbol_type(grm, grm_SYMTYPE_TERMINAL);
        grm_put_symbol(grm, "id");
        grm_put_symbol(grm, "+");
        grm_put_symbol(grm, "*");
        grm_put_symbol(grm, "(");
        grm_put_symbol(grm, ")");

        grm_set_default_symbol_type(grm, grm_SYMTYPE_NON_TERMINAL);
        grm_append_prule(grm, "E", rhs_E_1, RHS_LEN(rhs_E_1));
        grm_append_prule(grm, "E", rhs_E_2, RHS_LEN(rhs_E_2));
        grm_append_prule(grm, "T", rhs_T_1, RHS_LEN(rhs_T_1));
        grm_append_prule(grm, "T", rhs_T_2, RHS_LEN(rhs_T_2));
        grm_append_prule(grm, "F", rhs_F_1, RHS_LEN(rhs_F_1));
        grm_append_prule(grm, "F", rhs_F_2, RHS_LEN(rhs_F_2));

        start_symbol = grm_put_symbol(grm, "E");
        grm_set_start_symbol(grm, *start_symbol);
    }

    fsts = ffset_new_fsts();
    ret = ffset_calc_fsts(fsts, grm);
    
    flws = ffset_new_flws();
    A_NOT_NULL(c, flws);

    ret = ffset_calc_flws(flws, grm, fsts);
    A_EQL_INT(c, 0, ret);

    {
        const grm_SymbolID *sym;
        grm_SymbolID *set;
        size_t len;
        int has_eof;
        size_t i;
        
        sym = grm_put_symbol(grm, "E");
        ret = ffset_get_flws(&set, &len, &has_eof, flws, *sym);
        A_EQL_INT(c, 0, ret);

        printf("%s ... set: [ ", grm_lookup_symbol(grm, *sym));
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_eof: %s\n", (has_eof)? "true" : "false");

        sym = grm_put_symbol(grm, "T");
        ret = ffset_get_flws(&set, &len, &has_eof, flws, *sym);
        A_EQL_INT(c, 0, ret);

        printf("%s ... set: [ ", grm_lookup_symbol(grm, *sym));
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_eof: %s\n", (has_eof)? "true" : "false");

        sym = grm_put_symbol(grm, "F");
        ret = ffset_get_flws(&set, &len, &has_eof, flws, *sym);
        A_EQL_INT(c, 0, ret);

        printf("%s ... set: [ ", grm_lookup_symbol(grm, *sym));
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_eof: %s\n", (has_eof)? "true" : "false");
    }

    ffset_delete_flws(flws);
}