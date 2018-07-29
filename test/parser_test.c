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
    good_ASTNodeStore *nodes;
    syms_SymbolStore *syms;
    FILE *target;
    const good_ASTNode *root_node;
    const good_ASTNode *prule_node;
    const good_ASTNode *prule_rhs_node;
    const good_ASTNode *prule_rhs_elem_node;
    const good_RootNodeBody *root_body;
    const good_PRuleNodeBody *prule_body;
    const good_PRuleRHSNodeBody *prule_rhs_body;
    const good_PRuleRHSElementNodeBody *prule_rhs_elem_body;

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

    root_node = good_parse(psr);
    A_NOT_NULL(c, root_node);
    A_EQL_INT(c, good_AST_ROOT, root_node->type);
    root_body = &root_node->body.root;
    A_NULL(c, root_body->terminal_symbol);
    A_NOT_NULL(c, root_body->prule);

    // E
    //     : E '+' T
    //     | T
    //     ;
    {
        prule_node = root_body->prule;
        A_NOT_NULL(c, prule_node);
        A_EQL_INT(c, good_AST_PRULE, prule_node->type);
        prule_body = &prule_node->body.prule;
        A_EQL_STRING(c, "E", syms_lookup(syms, prule_body->lhs));
        A_NOT_NULL(c, prule_body->rhs);
        // : E '+' T
        prule_rhs_node = prule_body->rhs;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // : E '+' T
        //   ^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "E", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : E '+' T
        //     ^^^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_STRING, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "+", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : E '+' T
        //         ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "T", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : E '+' T
        //           ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        // | T
        prule_rhs_node = prule_rhs_body->next;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // | T
        //   ^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "T", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // | T
        //     ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        
        prule_rhs_node = prule_rhs_body->next;
        A_NULL(c, prule_rhs_node);
    }

    // T
    //     : T '*' F
    //     | F
    //     ;
    {
        prule_node = prule_body->next;
        A_NOT_NULL(c, prule_node);
        A_EQL_INT(c, good_AST_PRULE, prule_node->type);
        prule_body = &prule_node->body.prule;
        A_EQL_STRING(c, "T", syms_lookup(syms, prule_body->lhs));
        A_NOT_NULL(c, prule_body->rhs);
        // : T '*' F
        prule_rhs_node = prule_body->rhs;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // : T '*' F
        //   ^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "T", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : T '*' F
        //     ^^^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_STRING, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "*", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : T '*' F
        //         ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "F", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : T '*' F
        //           ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        // | F
        prule_rhs_node = prule_rhs_body->next;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // | F
        //   ^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "F", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // | F
        //     ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        
        prule_rhs_node = prule_rhs_body->next;
        A_NULL(c, prule_rhs_node);
    }

    // F
    //     : '(' E ')'
    //     | id
    //     ;
    {
        prule_node = prule_body->next;
        A_NOT_NULL(c, prule_node);
        A_EQL_INT(c, good_AST_PRULE, prule_node->type);
        prule_body = &prule_node->body.prule;
        A_EQL_STRING(c, "F", syms_lookup(syms, prule_body->lhs));
        A_NOT_NULL(c, prule_body->rhs);
        // : '(' E ')'
        prule_rhs_node = prule_body->rhs;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // : '(' E ')'
        //   ^^^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_STRING, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "(", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : '(' E ')'
        //       ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "E", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : '(' E ')'
        //         ^^^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_STRING, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, ")", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : '(' E ')'
        //             ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        // | id
        prule_rhs_node = prule_rhs_body->next;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // | id
        //   ^^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_SYMBOL, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "id", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // | id
        //      ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        
        prule_rhs_node = prule_rhs_body->next;
        A_NULL(c, prule_rhs_node);
    }

    // id
    //     : '[a-zA-Z_][a-zA-Z0-9_]*'
    //     ;
    {
        prule_node = prule_body->next;
        A_NOT_NULL(c, prule_node);
        A_EQL_INT(c, good_AST_PRULE, prule_node->type);
        prule_body = &prule_node->body.prule;
        A_EQL_STRING(c, "id", syms_lookup(syms, prule_body->lhs));
        A_NOT_NULL(c, prule_body->rhs);
        // : '[a-zA-Z_][a-zA-Z0-9_]*'
        prule_rhs_node = prule_body->rhs;
        A_NOT_NULL(c, prule_rhs_node);
        A_EQL_INT(c, good_AST_PRULE_RHS, prule_rhs_node->type);
        prule_rhs_body = &prule_rhs_node->body.prule_rhs;
        // : '[a-zA-Z_][a-zA-Z0-9_]*'
        //   ^^^^^^^^^^^^^^^^^^^^^^^^
        prule_rhs_elem_node = prule_rhs_body->elem;
        A_NOT_NULL(c, prule_rhs_elem_node);
        A_EQL_INT(c, good_AST_PRULE_RHS_ELEM_STRING, prule_rhs_elem_node->type);
        prule_rhs_elem_body = &prule_rhs_elem_node->body.prule_rhs_element;
        A_EQL_STRING(c, "[a-zA-Z_][a-zA-Z0-9_]*", syms_lookup(syms, prule_rhs_elem_body->symbol));
        A_EQL_INT(c, good_Q_1, prule_rhs_elem_body->quantifier);
        // : '[a-zA-Z_][a-zA-Z0-9_]*'
        //                            ^
        prule_rhs_elem_node = prule_rhs_elem_body->next;
        A_NULL(c, prule_rhs_elem_node);
        
        prule_rhs_node = prule_rhs_body->next;
        A_NULL(c, prule_rhs_node);
    }

    prule_node = prule_body->next;
    A_NULL(c, prule_node);

    good_delete_parser(psr);
    good_delete_ast_node_store(nodes);
    good_delete_tokenizer(tknzr);
    syms_delete(syms);
}
