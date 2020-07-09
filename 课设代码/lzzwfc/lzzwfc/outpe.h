#pragma once
#include "DynArray.h"
#include "coff.h"
#include <Windows.h>
extern char* outfile;					// 输出文件名
extern int output_type;				// 输出文件类型
extern float scc_version;		// SCC编译器版本号

/* 输出类型 */
enum e_OutType
{
    OUTPUT_OBJ,		// 目标文件
    OUTPUT_EXE,		// EXE可执行文件
    OUTPUT_MEMORY	// 内存中直接运行，不输出
};

/* 导入符号内存存储结构 */
typedef struct ImportSym
{
    int iat_index;
    int thk_offset;
    IMAGE_IMPORT_BY_NAME imp_sym;
}ImportSym;

/* 导入模块内存存储结构 */
typedef struct ImportInfo
{
    int dll_index;
    DynArray imp_syms;
    IMAGE_IMPORT_DESCRIPTOR imphdr;
}ImportInfo;

/* PE信息存储结构 */
typedef struct PEInfo
{
    Section* thunk;
    const char* filename;
    DWORD entry_addr;
    DWORD imp_offs;
    DWORD imp_size;
    DWORD iat_offs;
    DWORD iat_size;
    Section** secs;
    int   sec_size;
    DynArray imps;
}PEInfo;

extern DynArray array_dll;
extern DynArray array_lib;
extern char* lib_path;
extern short subsystem;
int pe_output_file(char* filename);
void write_obj(char* name);
int load_obj_file(char* fname);
void relocate_syms();
extern char* entry_symbol;
int pe_find_import(char* symbol);
void add_runtime_libs();
void coffrelocs_fixup();
char* get_lib_path();