#ifndef grm_SYMBOL_TABLE_H
#define grm_SYMBOL_TABLE_H

/*
 * 記号種別
 */
typedef enum grm_SymbolType {
    grm_SYMTYPE_TERMINAL,
    grm_SYMTYPE_NON_TERMINAL,
} grm_SymbolType;

typedef struct grm_SymbolTable grm_SymbolTable;
typedef unsigned long int grm_SymbolID;

grm_SymbolTable *grm_new_symtbl(void);
void grm_delete_symtbl(grm_SymbolTable *symtbl);
const grm_SymbolID *grm_put_in_symtbl(grm_SymbolTable *symtbl, const char *symbol, grm_SymbolType type);
const char *grm_lookup_in_symtbl(const grm_SymbolTable *symtbl, grm_SymbolID id);
grm_SymbolType grm_get_symbol_type(grm_SymbolID id);

#endif
