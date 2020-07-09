#pragma once
#include <Windows.h>
#include "DynArray.h"
#include "symbol.h"
#define CST_FUNC    0x20  //Coff符号类型，函数
#define CST_NOTFUNC 0     //Coff符号类型，非函数
extern int nsec_images;

#pragma pack(push, 1)	//将原来的结构体对齐方式压栈，现在用1字节对齐，也就是不对齐
//节内容也不能结构体对齐
typedef struct Section
{
	IMAGE_SECTION_HEADER sh;	//节头
	char* data;					//节数据
	int idx;					//节序号，从1开始
	int sec_size;				//节的大小，realloc时翻倍，但是最后赋值为data_offset,也就是是SizeOfRawData
	struct Section* link;		//符号表节和字符串节在同一节，关联在一起
	int* hashtable;				//哈希表
	int data_offset;			//相对于data的偏移地址
}Section;

//符号表中的一项纪录，手册的长度为18字节，符号表由所有的纪录构成
//这里结构体不能对齐，必须保证CoffSym是18字节
typedef struct CoffSym
{
	DWORD Name;		//符号名称,实际上是偏移地址，从字符串表找
	DWORD Next;		//有哈希冲突，存放下一个可能的偏移地址
	DWORD Value;				// 与符号相关的值，可能为符号在节的偏移量
	short Section;			// 节表的索引(从1开始),表示这个符号是哪一个节的，比如section=1，说明是代码节中的符号
	WORD  Type;				// 一个表示类型的数字
	BYTE  StorageClass;		// 这是一个表示存储类别的枚举类型值
	BYTE  NumberOfAuxSymbols;	// 跟在本记录后面的辅助符号表项的个数
}CoffSym;

/* 重定位结构定义 */
typedef struct CoffReloc
{
	DWORD offset;	// 需要进行重定位的代码或数据的地址，相对于自己节的data的偏移
	DWORD cfsym;				// 符号表的索引(从0开始)
	BYTE  section;  //对COFF重定位结构进行修改记录Section信息
	BYTE  type;
} CoffReloc;
#pragma pack(pop)
DynArray sections;	//节数组，所有节信息都保存在这里
Section  *sec_text,
		 *sec_data,
		 *sec_bss,
		 *sec_idata,
		 *sec_rdata,
		 *sec_rel,			//重定位信息节
		 *sec_symtab,
		 *sec_dynsymtab;	//链接库符号节
void init_coff();
Section* new_section(char* name, int Characteristics);
Section* new_coffsym_section(char* symtab_name, int Characteristics, char* strtab_name);
int add_coffsym(Section* symtab, char* name, int val, int sec_index, short type, char StorageClass);
int search_coffsym(Section* symtab, char* sym_name);
void* section_ptr_add(Section* sec, int size);
void realloc_section(Section* sec, int new_size);
char* add_coffstr(Section* sec, char* name);
void add_coffreloc(Section* sec, Symbol* sym, int offsset, char type);
void add_update_coffsym(Symbol* s, int val, int sec_index,
	short type, char StorageClass);
void direct_add_coffreloc(int offset, int cfsym, char section, char type);
void write_obj(char* name);