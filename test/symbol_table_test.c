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
    grm_SymbolTable *symtbl;

    symtbl = grm_new_symtbl();
    A_NOT_NULL(c, symtbl);

    {
        const grm_SymbolID *id;
        grm_SymbolID id_1;
        grm_SymbolID id_2;
        grm_SymbolID id_3;
        grm_SymbolID id_4;
        grm_SymbolID id_5;
        grm_SymbolID id_6;
        grm_SymbolID id_7;
        grm_SymbolID id_8;
        grm_SymbolID id_9;

        id = grm_put_in_symtbl(symtbl, "terminal-1", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_1 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-2", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_2 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-3", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_3 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-4", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_4 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-5", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_5 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-6", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_6 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-7", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_7 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-8", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_8 = *id;
        id = grm_put_in_symtbl(symtbl, "terminal-9", grm_SYMTYPE_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_TERMINAL, grm_get_symbol_type(*id));
        id_9 = *id;

        A_EQL_STRING(c, "terminal-1", grm_lookup_in_symtbl(symtbl, id_1));
        A_EQL_STRING(c, "terminal-2", grm_lookup_in_symtbl(symtbl, id_2));
        A_EQL_STRING(c, "terminal-3", grm_lookup_in_symtbl(symtbl, id_3));
        A_EQL_STRING(c, "terminal-4", grm_lookup_in_symtbl(symtbl, id_4));
        A_EQL_STRING(c, "terminal-5", grm_lookup_in_symtbl(symtbl, id_5));
        A_EQL_STRING(c, "terminal-6", grm_lookup_in_symtbl(symtbl, id_6));
        A_EQL_STRING(c, "terminal-7", grm_lookup_in_symtbl(symtbl, id_7));
        A_EQL_STRING(c, "terminal-8", grm_lookup_in_symtbl(symtbl, id_8));
        A_EQL_STRING(c, "terminal-9", grm_lookup_in_symtbl(symtbl, id_9));
    }

    {
        const grm_SymbolID *id;
        grm_SymbolID id_1;
        grm_SymbolID id_2;
        grm_SymbolID id_3;
        grm_SymbolID id_4;
        grm_SymbolID id_5;
        grm_SymbolID id_6;
        grm_SymbolID id_7;
        grm_SymbolID id_8;
        grm_SymbolID id_9;

        id = grm_put_in_symtbl(symtbl, "non-terminal-1", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_1 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-2", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_2 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-3", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_3 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-4", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_4 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-5", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_5 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-6", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_6 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-7", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_7 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-8", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_8 = *id;
        id = grm_put_in_symtbl(symtbl, "non-terminal-9", grm_SYMTYPE_NON_TERMINAL);
        A_NOT_NULL(c, id);
        A_EQL_INT(c, grm_SYMTYPE_NON_TERMINAL, grm_get_symbol_type(*id));
        id_9 = *id;

        A_EQL_STRING(c, "non-terminal-1", grm_lookup_in_symtbl(symtbl, id_1));
        A_EQL_STRING(c, "non-terminal-2", grm_lookup_in_symtbl(symtbl, id_2));
        A_EQL_STRING(c, "non-terminal-3", grm_lookup_in_symtbl(symtbl, id_3));
        A_EQL_STRING(c, "non-terminal-4", grm_lookup_in_symtbl(symtbl, id_4));
        A_EQL_STRING(c, "non-terminal-5", grm_lookup_in_symtbl(symtbl, id_5));
        A_EQL_STRING(c, "non-terminal-6", grm_lookup_in_symtbl(symtbl, id_6));
        A_EQL_STRING(c, "non-terminal-7", grm_lookup_in_symtbl(symtbl, id_7));
        A_EQL_STRING(c, "non-terminal-8", grm_lookup_in_symtbl(symtbl, id_8));
        A_EQL_STRING(c, "non-terminal-9", grm_lookup_in_symtbl(symtbl, id_9));
    }

    grm_delete_symtbl(symtbl);
}
