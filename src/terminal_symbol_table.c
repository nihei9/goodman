#include "terminal_symbol_table.h"
#include <collections.h>
#include <string.h>

struct good_TerminalSymbolTable {
    hmap_HashMap *formats;
};

good_TerminalSymbolTable *good_new_tsymtbl(void)
{
    good_TerminalSymbolTable *tsymtbl;
    hmap_HashMap *formats;

    tsymtbl = (good_TerminalSymbolTable *) malloc(sizeof (good_TerminalSymbolTable));
    if (tsymtbl == NULL) {
        return NULL;
    }

    formats = hmap_new(c_TYPE_ULONG, c_TYPE_STRING);
    if (formats == NULL) {
        free(tsymtbl);

        return NULL;
    }

    tsymtbl->formats = formats;

    return tsymtbl;
}

void good_delete_tsymtbl(good_TerminalSymbolTable *tsymtbl)
{
    if (tsymtbl == NULL) {
        return;
    }

    // TODO good_put_tsym()内でformatに割り当てたメモリ領域を解放する。

    hmap_delete(tsymtbl->formats);
    tsymtbl->formats = NULL;
    free(tsymtbl);
}

int good_put_tsym(good_TerminalSymbolTable *tsymtbl, good_SymbolID tsym_id, const char *format)
{
    const char *dup_format;
    void *ret;

    dup_format = strdup(format);
    if (dup_format == NULL) {
        return 1;
    }

    ret = hmap_put(tsymtbl->formats, &tsym_id, &dup_format);
    if (ret == NULL) {
        return 1;
    }

    return 0;
}

const char *good_get_tsym_format(const good_TerminalSymbolTable *tsymtbl, good_SymbolID tsym_id)
{
    void *v;

    v = hmap_lookup(tsymtbl->formats, &tsym_id);
    if (v == NULL) {
        return NULL;
    }

    return *((const char **) v);
}
