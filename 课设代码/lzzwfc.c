#include <stdio.h>
#include <stdlib.h>
#include "lexial.h"

int main(void)
{
    fin = fopen("HelloWorld.c", "rb");
    init_lexial();
    get_ch();
    test_lexial();
    //DynArray arr;
    //array_init(arr, 4);
    //array_realloc(&arr, 32);
    return 0;
}