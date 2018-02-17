#include "ast.h"
#include <stdlib.h>

good_AST *good_new_ast(good_ASTType type, const good_ASTValue *value)
{
    good_AST *ast;

    ast = (good_AST *) malloc(sizeof (good_AST));
    if (ast == NULL) {
        return NULL;
    }

    ast->type = type;
    if (value != NULL) {
        ast->value = *value;
    }
    ast->brother = NULL;
    ast->child = NULL;
    ast->_num_child = 0;

    return ast;
}

void good_delete_ast(good_AST *ast)
{
    if (ast == NULL) {
        return;
    }

    good_delete_ast(ast->child);
    good_delete_ast(ast->brother);

    free(ast);
}

good_AST *good_append_child(good_AST *parent, good_AST *child)
{
    good_AST *youngest_child;

    if (parent == NULL) {
        return child;
    }

    if (parent->child == NULL) {
        parent->child = child;
        parent->_num_child++;

        return parent;
    }

    youngest_child = parent->child;
    while (youngest_child->brother != NULL) {
        youngest_child = youngest_child->brother;
    }
    youngest_child->brother = child;
    parent->_num_child++;

    return parent;
}

size_t good_count_child(const good_AST *ast)
{
    return ast->_num_child;
}
