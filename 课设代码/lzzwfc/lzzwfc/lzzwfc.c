#include <stdio.h>
#include <stdlib.h>
#include "lexial.h"
#include "grammer.h"
#include "error.h"
#include "symbol.h"
#include "lzzwfc.h"
#include "coff.h"
#include "outpe.h"
int line_num;
Symbol* sym_sec_rdata;
void init()
{
    line_num = 1;
    array_init(&array_lib, 4);
    array_init(&array_dll, 4);
    init_lexial();
    sym_sec_rdata = sec_sym_put(".rdata", 0);
    stack_init(&global_sym_stack, 8);
    stack_init(&local_sym_stack, 8);
    init_coff();
    lib_path = get_lib_path();

}
int main(void)
{
    fin = fopen("HelloWorld.c", "rb");
    init();
    output_type = OUTPUT_EXE;
    subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
    //get_ch();
    //get_token();
    //translation_unit();
    printf("\n语法分析通过！");
    //write_obj("HelloWorld.obj");
    load_obj_file("HelloWorld.obj");
    pe_output_file("HelloWorld.exe");
}