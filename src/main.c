#include "printer.h"
#include "first_set.h"
#include "follow_set.h"
#include "ast_converter.h"
#include "ast_normalizer.h"
#include "parser.h"
#include "tokenizer.h"
#include "logger.h"
#include <stdio.h>

typedef struct good_GoodmanParameters {
    const char *filename;
} good_GoodmanParameters;

static int good_parse_parameters(good_GoodmanParameters *params, int argc, const char *argv[]);
static int good_execute(const good_GoodmanParameters *params);

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
    good_PrinterParameters pprams;
    ffset_FirstSet *fsts = NULL;
    ffset_FollowSet *flws = NULL;
    good_Grammar *grammar = NULL;
    good_ASTNode *ast = NULL;
    good_Parser *psr = NULL;
    good_Tokenizer *tknzr = NULL;
    good_ASTNodeStore *nodes = NULL;
    syms_SymbolStore *syms = NULL;
    FILE *target = NULL;
    int exit_code = 1;
    int ret;

    ret = good_initialize_logger("goodman.log");
    if (ret != 0) {
        printf("Failed to initialize logger\n");

        goto END;
    }
    good_log_info("Initialized logger");
    
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

    nodes = good_new_ast_node_store();
    if (nodes == NULL) {
        printf("Failed to create node store.\n");

        goto END;
    }

    psr = good_new_parser(tknzr, nodes);
    if (psr == NULL) {
        printf("Failed to create parser.\n");

        goto END;
    }

    ast = good_parse(psr);
    if (ast == NULL) {
        printf("Failed to parse.\n");

        good_print_error(good_get_parser_error(psr));

        goto END;
    }
    good_log_info("Parsed .goodman file");

    ast = good_normalize_ast(nodes, ast, syms);
    if (ast == NULL) {
        printf("Failed to normalize AST.\n");

        goto END;
    }

    grammar = good_new_grammar(ast, syms);
    if (grammar == NULL) {
        printf("Failed to create grammar.\n");

        goto END;
    }

    fsts = ffset_new_fsts();
    if (fsts == NULL) {
        printf("Failed to create first set.\n");

        goto END;
    }
    ret = ffset_calc_fsts(fsts, grammar);
    if (ret != 0) {
        printf("Failed to calcurate first set.\n");

        goto END;
    }
    good_log_info("Calcurated FIRST SET");

    flws = ffset_new_flws();
    if (flws == NULL) {
        printf("Failed to create follow set.\n");

        goto END;
    }
    ret = ffset_calc_flws(flws, grammar, fsts);
    if (ret != 0) {
        printf("Failed to calcurate follow set.\n");

        goto END;
    }
    good_log_info("Calcurated FOLLOW SET");

    pprams.grammar = grammar;
    pprams.first_set = fsts;
    pprams.follow_set = flws;

    ret = good_print(&pprams);
    if (ret != 0) {
        printf("Failed to print results.\n");

        goto END;
    }
    good_log_info("Printed results");

    exit_code = 0;

END:
    ffset_delete_flws(flws);
    ffset_delete_fsts(fsts);
    good_delete_grammar(grammar);
    good_delete_ast_node_store(nodes);
    good_delete_parser(psr);
    good_delete_tokenizer(tknzr);
    syms_delete(syms);
    if (target != NULL) {
        fclose(target);
    }

    good_finalize_logger();
    
    return exit_code;
}
