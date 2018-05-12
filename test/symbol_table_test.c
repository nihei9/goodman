#include "symbol_table.h"
#include <connie_sugar.h>
#include <stdio.h>

static void test_put_terminal_symbol(connie_Connie *c);

int main (void)
{
    connie_Connie *c = connie_new(__FILE__);

    TEST(c, test_put_terminal_symbol);

    connie_print(c);
    connie_delete(c);
    
    return 0;
}

static void test_put_terminal_symbol(connie_Connie *c)
{
    good_SymbolTable *symtbl;

    symtbl = good_new_symtbl();
    A_NOT_NULL(c, symtbl);

    {
        const good_SymbolID *id;
        good_SymbolID id_1;
        good_SymbolID id_2;
        good_SymbolID id_3;
        good_SymbolID id_4;
        good_SymbolID id_5;
        good_SymbolID id_6;
        good_SymbolID id_7;
        good_SymbolID id_8;
        good_SymbolID id_9;

        id = good_put_in_symtbl(symtbl, "terminal-1", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_1 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-2", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_2 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-3", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_3 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-4", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_4 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-5", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_5 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-6", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_6 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-7", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_7 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-8", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_8 = *id;
        id = good_put_in_symtbl(symtbl, "terminal-9", good_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_TERMINAL, good_get_symbol_type(*id));
        id_9 = *id;

        A_EQL_STRING(c, "terminal-1", good_lookup_in_symtbl(symtbl, id_1));
        A_EQL_STRING(c, "terminal-2", good_lookup_in_symtbl(symtbl, id_2));
        A_EQL_STRING(c, "terminal-3", good_lookup_in_symtbl(symtbl, id_3));
        A_EQL_STRING(c, "terminal-4", good_lookup_in_symtbl(symtbl, id_4));
        A_EQL_STRING(c, "terminal-5", good_lookup_in_symtbl(symtbl, id_5));
        A_EQL_STRING(c, "terminal-6", good_lookup_in_symtbl(symtbl, id_6));
        A_EQL_STRING(c, "terminal-7", good_lookup_in_symtbl(symtbl, id_7));
        A_EQL_STRING(c, "terminal-8", good_lookup_in_symtbl(symtbl, id_8));
        A_EQL_STRING(c, "terminal-9", good_lookup_in_symtbl(symtbl, id_9));
    }

    {
        const good_SymbolID *id;
        good_SymbolID id_1;
        good_SymbolID id_2;
        good_SymbolID id_3;
        good_SymbolID id_4;
        good_SymbolID id_5;
        good_SymbolID id_6;
        good_SymbolID id_7;
        good_SymbolID id_8;
        good_SymbolID id_9;

        id = good_put_in_symtbl(symtbl, "non-terminal-1", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_1 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-2", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_2 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-3", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_3 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-4", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_4 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-5", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_5 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-6", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_6 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-7", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_7 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-8", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_8 = *id;
        id = good_put_in_symtbl(symtbl, "non-terminal-9", good_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, good_SYMTYPE_NON_TERMINAL, good_get_symbol_type(*id));
        id_9 = *id;

        A_EQL_STRING(c, "non-terminal-1", good_lookup_in_symtbl(symtbl, id_1));
        A_EQL_STRING(c, "non-terminal-2", good_lookup_in_symtbl(symtbl, id_2));
        A_EQL_STRING(c, "non-terminal-3", good_lookup_in_symtbl(symtbl, id_3));
        A_EQL_STRING(c, "non-terminal-4", good_lookup_in_symtbl(symtbl, id_4));
        A_EQL_STRING(c, "non-terminal-5", good_lookup_in_symtbl(symtbl, id_5));
        A_EQL_STRING(c, "non-terminal-6", good_lookup_in_symtbl(symtbl, id_6));
        A_EQL_STRING(c, "non-terminal-7", good_lookup_in_symtbl(symtbl, id_7));
        A_EQL_STRING(c, "non-terminal-8", good_lookup_in_symtbl(symtbl, id_8));
        A_EQL_STRING(c, "non-terminal-9", good_lookup_in_symtbl(symtbl, id_9));
    }

    good_delete_symtbl(symtbl);
}
