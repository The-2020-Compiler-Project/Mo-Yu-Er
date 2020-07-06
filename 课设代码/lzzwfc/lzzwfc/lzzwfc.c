#include <stdio.h>
#include <stdlib.h>
#include "lexial.h"
#include "grammer.h"
#include "error.h"
#include "lzzwfc.h"
#include "coff.h"
int line_num;
void init()
{
    line_num = 1;
    init_lexial();
    stack_init(&global_sym_stack, 8);
    stack_init(&local_sym_stack, 8);
    init_coff();

}
int main(void)
{
    fin = fopen("HelloWorld.c", "rb");
    init();
    get_ch();
    get_token();
    translation_unit();
    Symbol* sym = stack_get_top(&global_sym_stack);
    printf("\n语法分析通过！");
    write_obj("HelloWorld.obj");
    printf("%d", sizeof(CoffSym));
}