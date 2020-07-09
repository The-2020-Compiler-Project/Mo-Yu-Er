#pragma once
#include "DynArray.h"
#include "coff.h"
#include <Windows.h>
extern char* outfile;					// ����ļ���
extern int output_type;				// ����ļ�����
extern float scc_version;		// SCC�������汾��

/* ������� */
enum e_OutType
{
    OUTPUT_OBJ,		// Ŀ���ļ�
    OUTPUT_EXE,		// EXE��ִ���ļ�
    OUTPUT_MEMORY	// �ڴ���ֱ�����У������
};

/* ��������ڴ�洢�ṹ */
typedef struct ImportSym
{
    int iat_index;
    int thk_offset;
    IMAGE_IMPORT_BY_NAME imp_sym;
}ImportSym;

/* ����ģ���ڴ�洢�ṹ */
typedef struct ImportInfo
{
    int dll_index;
    DynArray imp_syms;
    IMAGE_IMPORT_DESCRIPTOR imphdr;
}ImportInfo;

/* PE��Ϣ�洢�ṹ */
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