#include "printer.h"
#include <stdio.h>

static void good_print_grammar(const good_Grammar *grammar)
{
    // 終端記号表示
    {
        syms_SymbolID lhs_id;

        printf("@terminal-symbol\n");

        for (lhs_id = grammar->terminal_symbol_id_from; lhs_id <= grammar->terminal_symbol_id_to; lhs_id++) {
            const char *lhs_str;
            
            lhs_str = syms_lookup(grammar->syms, lhs_id);
            if (lhs_str == NULL) {
                return;
            }

            printf("%s\n", lhs_str);
        }
    }

    // 生成規則表示
    {
        const good_ProductionRule *prule;
        good_ProductionRuleFilter filter;

        printf("@production-rule\n");

        good_set_prule_filter_match_all(&filter);
        while ((prule = good_next_prule(&filter, grammar->prules)) != NULL) {
            size_t i;

            if (prule->lhs >= grammar->terminal_symbol_id_from && prule->lhs <= grammar->terminal_symbol_id_to) {
                continue;
            }

            printf("%s", syms_lookup(grammar->syms, prule->lhs));
            for (i = 0; i < prule->rhs_len; i++) {
                printf(" %s", syms_lookup(grammar->syms, prule->rhs[i]));
            }
            printf("\n");
        }
    }
}

static void good_print_ffset(const good_Grammar *grammar, const ffset_FirstSet *fsts, const ffset_FollowSet *flws)
{
    // FIRST集合表示
    {
        const good_ProductionRule *prule;
        good_ProductionRuleFilter filter;

        printf("@first-set\n");

        good_set_prule_filter_match_all(&filter);
        while ((prule = good_next_prule(&filter, grammar->prules)) != NULL) {
            ffset_FirstSetItem item;
            size_t i, j;

            if (prule->lhs >= grammar->terminal_symbol_id_from && prule->lhs <= grammar->terminal_symbol_id_to) {
                continue;
            }

            if (prule->rhs_len <= 0) {
                printf("%u(%s)-0 t\n", prule->id, syms_lookup(grammar->syms, prule->lhs));
            }
            else {
                for (i = 0; i < prule->rhs_len; i++) {
                    int ret;

                    item.input.fsts = fsts;
                    item.input.prule_id = prule->id;
                    item.input.offset = i;
                    ret = ffset_get_fsts(&item);
                    if (ret != 0) {
                        return;
                    }

                    printf("%u(%s)-%lu", prule->id, syms_lookup(grammar->syms, prule->lhs), i);
                    if (item.output.has_empty) {
                        printf(" t");
                    }
                    else {
                        printf(" f");
                    }
                    for (j = 0; j < item.output.len; j++) {
                        printf(" %lu(%s)", item.output.set[j], syms_lookup(grammar->syms, item.output.set[j]));
                    }
                    printf("\n");
                }
            }
        }
    }

    // FOLLOW集合表示
    {
        const good_ProductionRule *prule;
        good_ProductionRuleFilter filter;

        printf("@follow-set\n");

        good_set_prule_filter_match_all(&filter);
        while ((prule = good_next_prule(&filter, grammar->prules)) != NULL) {
            ffset_FollowSetItem item;
            size_t i;
            int ret;

            if (prule->lhs >= grammar->terminal_symbol_id_from && prule->lhs <= grammar->terminal_symbol_id_to) {
                continue;
            }

            item.input.flws = flws;
            item.input.symbol = prule->lhs;
            ret = ffset_get_flws(&item);
            if (ret != 0) {
                return;
            }

            printf("%lu(%s)", prule->lhs, syms_lookup(grammar->syms, prule->lhs));
            if (item.output.has_eof) {
                printf(" t");
            }
            else {
                printf(" f");
            }
            for (i = 0; i < item.output.len; i++) {
                printf(" %lu(%s)", item.output.set[i], syms_lookup(grammar->syms, item.output.set[i]));
            }
            printf("\n");
        }
    }
}

int good_print(const good_PrinterParameters *params)
{
    good_print_grammar(params->grammar);
    good_print_ffset(params->grammar, params->first_set, params->follow_set);

    return 0;
}
