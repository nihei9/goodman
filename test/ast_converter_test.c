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
    const good_Grammar *grm;
    good_Parser *psr;
    good_Tokenizer *tknzr;
    good_SymbolTable *symtbl;
    FILE *target;
    const good_AST *ast;

    target = fopen("test/data/test.goodman", "r");
    A_NOT_NULL(c, target);

    symtbl = good_new_symtbl();
    A_NOT_NULL(c, symtbl);

    tknzr = good_new_tokenizer(target, symtbl);
    A_NOT_NULL(c, tknzr);

    psr = good_new_parser(tknzr);
    A_NOT_NULL(c, psr);

    ast = good_parse(psr);
    A_NOT_NULL(c, ast);

    grm = good_new_grammar_from_ast(ast, symtbl);
    A_NOT_NULL(c, grm);

    good_delete_ast((good_AST *) ast);
    good_delete_parser(psr);
    good_delete_tokenizer(tknzr);
}
