#include "first_set.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_firstset(connie_Connie *c);

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
        grm_SymbolID *set;
        size_t len;
        int has_empty;
        size_t i;
        
        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 0, 0);
        A_EQL_INT(c, 0, ret);

        printf("0-0 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 0, 1);
        A_EQL_INT(c, 0, ret);

        printf("0-1 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 0, 2);
        A_EQL_INT(c, 0, ret);

        printf("0-2 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 1, 0);
        A_EQL_INT(c, 0, ret);

        printf("1-0 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 2, 0);
        A_EQL_INT(c, 0, ret);

        printf("2-0 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 2, 1);
        A_EQL_INT(c, 0, ret);

        printf("2-1 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 2, 2);
        A_EQL_INT(c, 0, ret);

        printf("2-2 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 3, 0);
        A_EQL_INT(c, 0, ret);

        printf("3-0 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 4, 0);
        A_EQL_INT(c, 0, ret);

        printf("4-0 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 4, 1);
        A_EQL_INT(c, 0, ret);

        printf("4-1 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 4, 2);
        A_EQL_INT(c, 0, ret);

        printf("4-2 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");

        ret = ffset_get_fsts(&set, &len, &has_empty, fsts, 5, 0);
        A_EQL_INT(c, 0, ret);

        printf("5-0 ... set: [ ");
        for (i = 0; i < len; i++) {
            printf("%s ", grm_lookup_symbol(grm, set[i]));
        }
        printf("], has_empty: %s\n", (has_empty)? "true" : "false");
    }

    ffset_delete_fsts(fsts);
}
