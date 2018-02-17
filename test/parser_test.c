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
    good_Parser *psr;
    good_Tokenizer *tknzr;
    grm_SymbolTable *symtbl;
    FILE *target;
    const good_AST *root_ast;
    const good_AST *prule_ast;
    const good_AST *prule_lhs_ast;
    const good_AST *prule_rhs_ast;
    const good_AST *prule_rhs_elem_ast;

    target = fopen("test/data/test.goodman", "r");
    A_NOT_NULL(c, target);

    symtbl = grm_new_symtbl();
    A_NOT_NULL(c, symtbl);

    tknzr = good_new_tokenizer(target, symtbl);
    A_NOT_NULL(c, tknzr);

    psr = good_new_parser(tknzr);
    A_NOT_NULL(c, psr);

    root_ast = good_parse(psr);
    A_NOT_NULL(c, root_ast);
    A_EQL_INT(c, good_AST_ROOT, root_ast->type);

    // E
    prule_ast = root_ast->child;
    A_NOT_NULL(c, prule_ast);
    A_EQL_INT(c, good_AST_PRULE, prule_ast->type);
    prule_lhs_ast = prule_ast->child;
    A_NOT_NULL(c, prule_lhs_ast);
    A_EQL_INT(c, good_AST_PRULE_LHS, prule_lhs_ast->type);
    // : E '+' T
    prule_rhs_ast = prule_lhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);
    // | T
    prule_rhs_ast = prule_rhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);

    // T
    prule_ast = prule_ast->brother;
    A_NOT_NULL(c, prule_ast);
    A_EQL_INT(c, good_AST_PRULE, prule_ast->type);
    prule_lhs_ast = prule_ast->child;
    A_NOT_NULL(c, prule_lhs_ast);
    A_EQL_INT(c, good_AST_PRULE_LHS, prule_lhs_ast->type);
    // : T '*' F
    prule_rhs_ast = prule_lhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);
    // | F
    prule_rhs_ast = prule_rhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);

    // F
    prule_ast = prule_ast->brother;
    A_NOT_NULL(c, prule_ast);
    A_EQL_INT(c, good_AST_PRULE, prule_ast->type);
    prule_lhs_ast = prule_ast->child;
    A_NOT_NULL(c, prule_lhs_ast);
    A_EQL_INT(c, good_AST_PRULE_LHS, prule_lhs_ast->type);
    // : '(' E ')'
    prule_rhs_ast = prule_lhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);
    // | id
    prule_rhs_ast = prule_rhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);

    // id
    prule_ast = prule_ast->brother;
    A_NOT_NULL(c, prule_ast);
    A_EQL_INT(c, good_AST_PRULE, prule_ast->type);
    prule_lhs_ast = prule_ast->child;
    A_NOT_NULL(c, prule_lhs_ast);
    A_EQL_INT(c, good_AST_PRULE_LHS, prule_lhs_ast->type);
    // : '[a-zA-Z_][a-zA-Z0-9_]*'
    prule_rhs_ast = prule_lhs_ast->brother;
    A_NOT_NULL(c, prule_rhs_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_ast->type);
    prule_rhs_elem_ast = prule_rhs_ast->child;
    A_NOT_NULL(c, prule_rhs_elem_ast);
    A_EQL_INT(c, good_AST_PRULE_RHS_ELEM, prule_rhs_elem_ast->type);
    prule_rhs_elem_ast = prule_rhs_elem_ast->brother;
    A_NULL(c, prule_rhs_elem_ast);

    prule_ast = prule_ast->brother;
    A_NULL(c, prule_ast);

    good_delete_ast((good_AST *) root_ast);
    good_delete_parser(psr);
    good_delete_tokenizer(tknzr);
}
