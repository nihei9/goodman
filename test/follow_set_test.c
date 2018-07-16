#include "follow_set.h"
#include <connie_sugar.h>
#include <stdio.h>

static void print_result(const syms_SymbolStore *syms, const ffset_FollowSetItem *item)
{
    size_t i;

    printf("%s ... set: [ ", syms_lookup(syms, item->input.symbol));
    for (i = 0; i < item->output.len; i++) {
        printf("%s ", syms_lookup(syms, item->output.set[i]));
    }
    printf("], has_eof: %s\n", (item->output.has_eof)? "true" : "false");
}

static void test_followset(connie_Connie *c)
{
    syms_SymbolStore *syms;
    good_ProductionRules *prules;
    good_Grammar grammar;
    ffset_FirstSet *fsts;
    ffset_FollowSet *flws;
    int ret;

    {
        syms_SymbolID rhs[256];
        const syms_SymbolID *id;
        syms_SymbolID terminal_symbol_id_from;
        syms_SymbolID terminal_symbol_id_to;
        syms_SymbolID id_id;
        syms_SymbolID id_plus;
        syms_SymbolID id_asterisk;
        syms_SymbolID id_l_paren;
        syms_SymbolID id_r_paren;
        syms_SymbolID id_E;
        syms_SymbolID id_T;
        syms_SymbolID id_F;

        syms = syms_new();
        
        id = syms_put(syms, "id");
        id_id = *id;
        terminal_symbol_id_from = *id;
        id = syms_put(syms, "+");
        id_plus = *id;
        id = syms_put(syms, "*");
        id_asterisk = *id;
        id = syms_put(syms, "(");
        id_l_paren = *id;
        id = syms_put(syms, ")");
        id_r_paren = *id;
        terminal_symbol_id_to = *id;

        id = syms_put(syms, "E");
        id_E = *id;
        id = syms_put(syms, "T");
        id_T = *id;
        id = syms_put(syms, "F");
        id_F = *id;

        prules = good_new_prules();

        rhs[0] = id_E;
        rhs[1] = id_plus;
        rhs[2] = id_T;
        good_append_prule(prules, id_E, rhs, 3);
        rhs[0] = id_T;
        good_append_prule(prules, id_E, rhs, 1);
        rhs[0] = id_T;
        rhs[1] = id_asterisk;
        rhs[2] = id_F;
        good_append_prule(prules, id_T, rhs, 3);
        rhs[0] = id_F;
        good_append_prule(prules, id_T, rhs, 1);
        rhs[0] = id_l_paren;
        rhs[1] = id_E;
        rhs[2] = id_r_paren;
        good_append_prule(prules, id_F, rhs, 3);
        rhs[0] = id_id;
        good_append_prule(prules, id_F, rhs, 1);

        grammar.syms = syms;
        grammar.terminal_symbol_id_from = terminal_symbol_id_from;
        grammar.terminal_symbol_id_to = terminal_symbol_id_to;
        grammar.prules = prules;
        grammar.start_symbol = id_E;
    }

    fsts = ffset_new_fsts();
    ret = ffset_calc_fsts(fsts, &grammar);
    
    flws = ffset_new_flws();
    A_NOT_NULL(c, flws);

    ret = ffset_calc_flws(flws, &grammar, fsts);
    A_EQL_INT(c, 0, ret);

    {
        ffset_FollowSetItem item;

        item.input.flws = flws;
        
        item.input.symbol = *syms_put(syms, "E");
        ret = ffset_get_flws(&item);
        A_EQL_INT(c, 0, ret);

        print_result(syms, &item);

        item.input.symbol = *syms_put(syms, "T");
        ret = ffset_get_flws(&item);
        A_EQL_INT(c, 0, ret);

        print_result(syms, &item);

        item.input.symbol = *syms_put(syms, "F");
        ret = ffset_get_flws(&item);
        A_EQL_INT(c, 0, ret);

        print_result(syms, &item);
    }

    ffset_delete_flws(flws);
    ffset_delete_fsts(fsts);
    good_delete_prules(prules);
    syms_delete(syms);
}

int main(void)
{
  connie_Connie *c = connie_new(__FILE__);

  TEST(c, test_followset);

  connie_print(c);

  connie_delete(c);

  return 0;
}
