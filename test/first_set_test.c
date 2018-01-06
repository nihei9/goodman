#include "first_set.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_firstset(connie_Connie *c);
void print_result(const grm_Grammar *grm, const ffset_FirstSetItem *item);

int main(void)
{
  connie_Connie *c = connie_new(__FILE__);

  TEST(c, test_firstset);

  connie_print(c);

  connie_delete(c);

  return 0;
}

#define RHS_LEN(rhs) (sizeof (rhs) / sizeof (char *))

void test_firstset(connie_Connie *c)
{
    grm_Grammar *grm;
    ffset_FirstSet *fsts;
    int ret;

    {
        const char *rhs_E_1[] = {"E", "+", "T"};
        const char *rhs_E_2[] = {"T"};
        const char *rhs_T_1[] = {"T", "*", "F"};
        const char *rhs_T_2[] = {"F"};
        const char *rhs_F_1[] = {"(", "E", ")"};
        const char *rhs_F_2[] = {"id"};

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
    }
    
    fsts = ffset_new_fsts();
    A_NOT_NULL(c, fsts);

    ret = ffset_calc_fsts(fsts, grm);
    A_EQL_INT(c, 0, ret);

    {
        ffset_FirstSetItem item;
        
        item.input.fsts = fsts;

        item.input.prule_id = 0;
        item.input.offset = 0;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 0;
        item.input.offset = 1;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 0;
        item.input.offset = 2;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 1;
        item.input.offset = 0;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 2;
        item.input.offset = 0;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 2;
        item.input.offset = 2;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 2;
        item.input.offset = 2;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 3;
        item.input.offset = 0;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 4;
        item.input.offset = 0;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 4;
        item.input.offset = 1;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 4;
        item.input.offset = 2;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);

        item.input.prule_id = 5;
        item.input.offset = 0;
        ret = ffset_get_fsts(&item);
        A_EQL_INT(c, 0, ret);

        print_result(grm, &item);
    }

    ffset_delete_fsts(fsts);
}

void print_result(const grm_Grammar *grm, const ffset_FirstSetItem *item)
{
    size_t i;

    printf("%u-%lu ... set: [ ", item->input.prule_id, item->input.offset);
    for (i = 0; i < item->output.len; i++) {
        printf("%s ", grm_lookup_symbol(grm, item->output.set[i]));
    }
    printf("], has_empty: %s\n", (item->output.has_empty)? "true" : "false");
}
