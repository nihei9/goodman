#ifndef good_SYMBOL_TABLE_H
#define good_SYMBOL_TABLE_H

/*
 * 記号種別
 */
typedef enum good_SymbolType {
    good_SYMTYPE_TERMINAL,
    good_SYMTYPE_NON_TERMINAL,
} good_SymbolType;

typedef struct good_SymbolTable good_SymbolTable;
typedef unsigned long int good_SymbolID;

good_SymbolTable *good_new_symtbl(void);
void good_delete_symtbl(good_SymbolTable *symtbl);
const good_SymbolID *good_put_in_symtbl(good_SymbolTable *symtbl, const char *symbol, good_SymbolType type);
const char *good_lookup_in_symtbl(const good_SymbolTable *symtbl, good_SymbolID id);
good_SymbolType good_get_symbol_type(good_SymbolID id);

#endif
