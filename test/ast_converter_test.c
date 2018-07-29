#include "ast_converter.h"
#include "parser.h"
#include <connie_sugar.h>
#include <stdio.h>

void test_1(connie_Connie *c);

int main (void)
{
    connie_Connie *c = connie_new(__FILE__);

    TEST(c, test_1);

    connie_print(c);
    connie_delete(c);
    
    return 0;
}

void test_1(connie_Connie *c)
{
    good_Grammar *grammar;
    good_Parser *psr;
    good_Tokenizer *tknzr;
    good_ASTNodeStore *nodes;
    syms_SymbolStore *syms;
    FILE *target;
    good_ASTNode *ast;

    target = fopen("test/data/test.goodman", "r");
    A_NOT_NULL(c, target);

    syms = syms_new();
    A_NOT_NULL(c, syms);

    tknzr = good_new_tokenizer(target, syms);
    A_NOT_NULL(c, tknzr);

    nodes = good_new_ast_node_store();
    A_NOT_NULL(c, nodes);

    psr = good_new_parser(tknzr, nodes);
    A_NOT_NULL(c, psr);

    ast = good_parse(psr);
    A_NOT_NULL(c, ast);

    grammar = good_new_grammar(ast, syms);
    A_NOT_NULL(c, grammar);

    good_delete_grammar(grammar);
    good_delete_parser(psr);
    good_delete_ast_node_store(nodes);
    good_delete_tokenizer(tknzr);
    syms_delete(syms);
}
