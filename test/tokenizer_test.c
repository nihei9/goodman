#include "tokenizer.h"
#include <connie_sugar.h>
#include <stdio.h>

int main (void)
{
    connie_Connie *c = connie_new(__FILE__);

    // TODO 実装

    connie_print(c);
    connie_delete(c);
    
    return 0;
}
