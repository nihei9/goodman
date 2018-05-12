#include "grammar.h"
#include "ast_converter.h"
#include "parser.h"
#include "tokenizer.h"
#include <stdio.h>

static void good_print_grammar(const good_Grammar *grammar);

int main(int argc, const char *argv[])
{
    const good_Grammar *grammar = NULL;
    const good_AST *ast = NULL;
    good_Parser *psr = NULL;
    good_Tokenizer *tknzr = NULL;
    good_SymbolTable *symtbl = NULL;
    FILE *target = NULL;
    int rc = 0;

    if (argc <= 1) {
        printf("Target files are missing.\n");

        rc = 1;
        goto END;
    }
    
    target = fopen(argv[1], "r");
    if (target == NULL) {
        printf("Failed to open '%s'.\n", argv[1]);

        rc = 1;
        goto END;
    }

    symtbl = good_new_symtbl();
    if (symtbl == NULL) {
        printf("Failed to create symbol table.\n");

        rc = 1;
        goto END;
    }

    tknzr = good_new_tokenizer(target, symtbl);
    if (tknzr == NULL) {
        printf("Failed to create tokenizer.\n");
        
        rc = 1;
        goto END;
    }

    psr = good_new_parser(tknzr);
    if (psr == NULL) {
        printf("Failed to create parser.\n");
        
        rc = 1;
        goto END;
    }

    ast = good_parse(psr);
    if (ast == NULL) {
        printf("Failed to parse.\n");

        rc = 1;
        goto END;
    }

    grammar = good_new_grammar_from_ast(ast, symtbl);
    if (grammar == NULL) {
        printf("Failed to create grammar.\n");

        rc = 1;
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
    
    return rc;
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
