#include "ast_converter.h"
#include "parser.h"
#include "tokenizer.h"
#include <stdio.h>

typedef struct good_GoodmanParameters {
    const char *filename;
} good_GoodmanParameters;

static int good_parse_parameters(good_GoodmanParameters *params, int argc, const char *argv[]);
static int good_execute(const good_GoodmanParameters *params);
static void good_print_grammar(const good_Grammar *grammar);

int main(int argc, const char *argv[])
{
    good_GoodmanParameters params;
    int ret;

    ret = good_parse_parameters(&params, argc, argv);
    if (ret != 0) {
        return 1;
    }

    ret = good_execute(&params);
    if (ret != 0) {
        return 1;
    }

    return 0;
}

static int good_parse_parameters(good_GoodmanParameters *params, int argc, const char *argv[])
{
    if (argc <= 1) {
        printf("Target files are missing.\n");

        return 1;
    }

    params->filename = argv[1];

    return 0;
}

static int good_execute(const good_GoodmanParameters *params)
{
    const good_Grammar *grammar = NULL;
    const good_AST *ast = NULL;
    good_Parser *psr = NULL;
    good_Tokenizer *tknzr = NULL;
    syms_SymbolStore *syms = NULL;
    FILE *target = NULL;
    int exit_code = 1;
    
    target = fopen(params->filename, "r");
    if (target == NULL) {
        printf("Failed to open '%s'.\n", params->filename);

        goto END;
    }

    syms = syms_new();
    if (syms == NULL) {
        printf("Failed to create symbol store.\n");

        goto END;
    }

    tknzr = good_new_tokenizer(target, syms);
    if (tknzr == NULL) {
        printf("Failed to create tokenizer.\n");

        goto END;
    }

    psr = good_new_parser(tknzr);
    if (psr == NULL) {
        printf("Failed to create parser.\n");

        goto END;
    }

    ast = good_parse(psr);
    if (ast == NULL) {
        printf("Failed to parse.\n");

        goto END;
    }

    grammar = good_new_grammar((good_AST *) ast, syms);
    if (grammar == NULL) {
        printf("Failed to create grammar.\n");

        goto END;
    }

    good_print_grammar(grammar);

    exit_code = 0;

END:
    good_delete_grammar((good_Grammar *) grammar);
    good_delete_ast((good_AST *) ast);
    good_delete_parser(psr);
    good_delete_tokenizer(tknzr);
    syms_delete(syms);
    if (target != NULL) {
        fclose(target);
    }
    
    return exit_code;
}

static void good_print_grammar(const good_Grammar *grammar)
{
    // 終端記号表示
    {
        syms_SymbolID lhs_id;

        printf("@terminal-symbol\n");

        for (lhs_id = grammar->terminal_symbol_id_from; lhs_id <= grammar->terminal_symbol_id_to; lhs_id++) {
            const char *lhs_str;
            good_ProductionRuleFilter filter;
            const good_ProductionRule *prule;

            lhs_str = syms_lookup(grammar->syms, lhs_id);
            if (lhs_str == NULL) {
                return;
            }

            good_set_prule_filter_by_lhs(&filter, lhs_id);
            prule = good_next_prule(&filter, grammar->prules);
            if (prule == NULL) {
                return;
            }
            printf("%s %s\n", lhs_str, syms_lookup(grammar->syms, prule->rhs[0]));
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
