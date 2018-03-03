#include "grammar.h"
#include "ast_converter.h"
#include "parser.h"
#include "tokenizer.h"
#include <stdio.h>

static void good_print_grammar(const grm_Grammar *grm);

int main(int argc, const char *argv[])
{
    const grm_Grammar *grm = NULL;
    const good_AST *ast = NULL;
    good_Parser *psr = NULL;
    good_Tokenizer *tknzr = NULL;
    grm_SymbolTable *symtbl = NULL;
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

    symtbl = grm_new_symtbl();
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

    grm = good_convert_ast_to_grammar(ast, symtbl);
    if (grm == NULL) {
        printf("Failed to create grammar.\n");

        rc = 1;
        goto END;
    }

    good_print_grammar(grm);

END:
    grm_delete((grm_Grammar *) grm);
    good_delete_ast((good_AST *) ast);
    good_delete_parser(psr);
    good_delete_tokenizer(tknzr);
    grm_delete_symtbl(symtbl);
    fclose(target);
    
    return rc;
}

static void good_print_grammar(const grm_Grammar *grm)
{
    const grm_ProductionRule *prule;
    grm_ProductionRuleFilter filter;
    grm_ProductionRuleFilter *f;

    f = grm_find_all_prule(grm, &filter);

    while ((prule = grm_next_prule(grm, f)) != NULL) {
        grm_SymbolID lhs = grm_get_pr_lhs(prule);
        const grm_SymbolID *rhs = grm_get_pr_rhs(prule);
        size_t rhs_len = grm_get_pr_rhs_len(prule);
        size_t i;

        printf("%s: \n", grm_lookup_symbol(grm, lhs));
        for (i = 0; i < rhs_len; i++) {
            printf(" %s", grm_lookup_symbol(grm, rhs[i]));
        }
        printf(";\n");
    }
}
