#include "grammar.h"
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
    good_SymbolTable *symtbl = NULL;
    FILE *target = NULL;
    
    target = fopen(params->filename, "r");
    if (target == NULL) {
        printf("Failed to open '%s'.\n", params->filename);

        goto END;
    }

    symtbl = good_new_symtbl();
    if (symtbl == NULL) {
        printf("Failed to create symbol table.\n");

        goto END;
    }

    tknzr = good_new_tokenizer(target, symtbl);
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

    grammar = good_new_grammar_from_ast(ast, symtbl);
    if (grammar == NULL) {
        printf("Failed to create grammar.\n");

        goto END;
    }

    good_print_grammar(grammar);

END:
    good_delete_grammar((good_Grammar *) grammar);
    good_delete_ast((good_AST *) ast);
    good_delete_parser(psr);
    good_delete_tokenizer(tknzr);
    good_delete_symtbl(symtbl);
    if (target != NULL) {
        fclose(target);
    }
    
    return 1;
}

static void good_print_grammar(const good_Grammar *grammar)
{
    // 終端記号表示
    {
        const grm_Grammar *prtbl;
        const grm_ProductionRule *prule;
        grm_ProductionRuleFilter filter;
        grm_ProductionRuleFilter *f;

        printf("@terminal-symbol\n");

        prtbl = grammar->prtbl;
        
        f = grm_find_all_prule(prtbl, &filter);
        while ((prule = grm_next_prule(prtbl, f)) != NULL) {
            good_SymbolID lhs = grm_get_pr_lhs(prule);
            const good_SymbolID *rhs = grm_get_pr_rhs(prule);

            if (good_get_symbol_type(lhs) != good_SYMTYPE_TERMINAL) {
                continue;
            }

            printf("%s %s\n", grm_lookup_symbol(prtbl, lhs), grm_lookup_symbol(prtbl, rhs[0]));
        }
    }

    // 生成規則表示
    {
        const grm_Grammar *prtbl;
        const grm_ProductionRule *prule;
        grm_ProductionRuleFilter filter;
        grm_ProductionRuleFilter *f;

        printf("@production-rule\n");

        prtbl = grammar->prtbl;
        
        f = grm_find_all_prule(prtbl, &filter);
        while ((prule = grm_next_prule(prtbl, f)) != NULL) {
            good_SymbolID lhs = grm_get_pr_lhs(prule);
            const good_SymbolID *rhs = grm_get_pr_rhs(prule);
            size_t rhs_len = grm_get_pr_rhs_len(prule);
            size_t i;

            if (good_get_symbol_type(lhs) != good_SYMTYPE_NON_TERMINAL) {
                continue;
            }

            printf("%s", grm_lookup_symbol(prtbl, lhs));
            for (i = 0; i < rhs_len; i++) {
                printf(" %s", grm_lookup_symbol(prtbl, rhs[i]));
            }
            printf("\n");
        }
    }
}
