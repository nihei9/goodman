#include "production_rule_table.h"
#include <connie_sugar.h>
#include <stdio.h>

static void test_append_production_rule(connie_Connie *c);

int main (void)
{
    connie_Connie *c = connie_new(__FILE__);

    TEST(c, test_append_production_rule);

    connie_print(c);
    connie_delete(c);
    
    return 0;
}

#define RHS_LEN(rhs) ((sizeof (rhs)) / (sizeof (grm_SymbolID)))

static void test_append_production_rule(connie_Connie *c)
{
    grm_ProductionRuleTable *prtbl;

    grm_SymbolID lhs_foo = 101;
    grm_SymbolID lhs_bar = 102;
    grm_SymbolID lhs_baz = 103;
    grm_SymbolID rhs_foo_1[] = {1, 2, 3};
    grm_SymbolID rhs_bar_1[] = {2, 4};
    grm_SymbolID rhs_bar_2[] = {5, 6, 7, 8};
    grm_SymbolID rhs_baz_1[] = {};

    prtbl = grm_new_prtbl();
    A_NOT_NULL(c, prtbl);
    printf("[test] grm_new_prtbl()\n");

    {
        int ret;

        ret = grm_append_to_prtbl(prtbl, lhs_foo, rhs_foo_1, RHS_LEN(rhs_foo_1));
        A_EQL_INT(c, 0, ret);
        ret = grm_append_to_prtbl(prtbl, lhs_bar, rhs_bar_1, RHS_LEN(rhs_bar_1));
        A_EQL_INT(c, 0, ret);
        ret = grm_append_to_prtbl(prtbl, lhs_bar, rhs_bar_2, RHS_LEN(rhs_bar_2));
        A_EQL_INT(c, 0, ret);
        ret = grm_append_to_prtbl(prtbl, lhs_baz, rhs_baz_1, RHS_LEN(rhs_baz_1));
        A_EQL_INT(c, 0, ret);
    }

    unsigned int id_foo_1;
    unsigned int id_bar_1, id_bar_2;
    unsigned int id_baz_1;
    {
        grm_ProductionRuleFilter f;
        const grm_ProductionRule *pr;

        grm_set_pr_filter_by_lhs(&f, 101);
        
        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 101, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_foo_1), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_foo_1, RHS_LEN(rhs_foo_1), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_foo_1 = grm_get_pr_id(pr);
        
        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);

        grm_set_pr_filter_by_lhs(&f, 102);
        
        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 102, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_bar_1), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_bar_1, RHS_LEN(rhs_bar_1), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_bar_1 = grm_get_pr_id(pr);
        
        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 102, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_bar_2), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_bar_2, RHS_LEN(rhs_bar_2), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_bar_2 = grm_get_pr_id(pr);

        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);

        grm_set_pr_filter_by_lhs(&f, 103);
        
        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 103, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_baz_1), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_baz_1, RHS_LEN(rhs_baz_1), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_baz_1 = grm_get_pr_id(pr);
        
        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);
    }

    {
        grm_ProductionRuleFilter f;
        const grm_ProductionRule *pr;

        grm_set_pr_filter_by_id(&f, id_foo_1);

        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 101, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_foo_1), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_foo_1, RHS_LEN(rhs_foo_1), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_foo_1 = grm_get_pr_id(pr);
        
        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);

        grm_set_pr_filter_by_id(&f, id_bar_1);

        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 102, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_bar_1), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_bar_1, RHS_LEN(rhs_bar_1), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_bar_1 = grm_get_pr_id(pr);

        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);

        grm_set_pr_filter_by_id(&f, id_bar_2);
        
        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 102, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_bar_2), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_bar_2, RHS_LEN(rhs_bar_2), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_bar_2 = grm_get_pr_id(pr);

        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);

        grm_set_pr_filter_by_id(&f, id_baz_1);

        pr = grm_next_pr(&f, prtbl);
        A_NOT_NULL(c, pr);
        A_EQL_ULONG(c, 103, grm_get_pr_lhs(pr));
        A_EQL_ULONG(c, RHS_LEN(rhs_baz_1), grm_get_pr_rhs_len(pr));
        A_EQL_ULONG_ARRAY(c, rhs_baz_1, RHS_LEN(rhs_baz_1), grm_get_pr_rhs(pr), connie_NOT_IGNORE_ORDER);
        id_baz_1 = grm_get_pr_id(pr);
        
        pr = grm_next_pr(&f, prtbl);
        A_NULL(c, pr);
    }

    grm_delete_prtbl(prtbl);
}
