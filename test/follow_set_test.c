#include "follow_set.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_followset(connie_Connie *c);
void print_result(const grm_Grammar *grm, const ffset_FollowSetItem *item);

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
        const good_SymbolID *start_symbol;

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
        ffset_FollowSetItem item;

        item.input.flws = flws;
        
        item.input.symbol = *grm_put_symbol(grm, "E");
        ret = ffset_get_flws(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.symbol = *grm_put_symbol(grm, "T");
        ret = ffset_get_flws(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.symbol = *grm_put_symbol(grm, "F");
        ret = ffset_get_flws(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);
    }

    ffset_delete_flws(flws);
}

void print_result(const grm_Grammar *grm, const ffset_FollowSetItem *item)
{
    size_t i;

    printf("%s ... set: [ ", grm_lookup_symbol(grm, item->input.symbol));
    for (i = 0; i < item->output.len; i++) {
        printf("%s ", grm_lookup_symbol(grm, item->output.set[i]));
    }
    printf("], has_eof: %s\n", (item->output.has_eof)? "true" : "false");
}
