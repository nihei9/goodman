#ifndef good_PRINTER_H
#define good_PRINTER_H

#include "grammar.h"
#include "first_set.h"
#include "follow_set.h"

typedef struct good_PrinterParameters {
    const good_Grammar *grammar;
    const ffset_FirstSet *first_set;
    const ffset_FollowSet *follow_set;
} good_PrinterParameters;

int good_print(const good_PrinterParameters *params);

#endif
