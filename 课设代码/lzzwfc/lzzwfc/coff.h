#pragma once
#include <Windows.h>
#include "DynArray.h"
int nsec_images;	//number of sections in the image file

#pragma pack(push, 1)	//将原来的结构体对齐方式压栈，现在用1字节对齐，也就是不对齐
//节内容也不能结构体对齐
typedef struct Section
{
	IMAGE_SECTION_HEADER sh;	//节头
	char* data;					//节数据
	int idx;					//节序号，从1开始
	int sec_size;				//节的大小，就是SizeOfRawData
	struct Section* link;		//符号表节和字符串节在同一节，关联在一起
	int* hashtable;				//哈希表
	int data_offset;			//相对于data的编译地址
}Section;

//符号表中的一项纪录，手册的长度为18字节，符号表由所有的纪录构成
//这里结构体不能对齐，必须保证CoffSym是18字节
typedef struct CoffSym
{
	DWORD Name;		//符号名称,实际上是偏移地址，从字符串表找
	DWORD Next;		//有哈希冲突，存放下一个可能的偏移地址
	DWORD Value;				// 与符号相关的值
	short Section;			// 节表的索引(从1开始),用以标识定义此符号的节*/
	WORD  Type;				// 一个表示类型的数字
	BYTE  StorageClass;		// 这是一个表示存储类别的枚举类型值
	BYTE  NumberOfAuxSymbols;	// 跟在本记录后面的辅助符号表项的个数
}CoffSym;

#pragma pack(pop)
DynArray sections;	//节数组，所有节信息都保存在这里
Section  *sec_text,
		 *sec_data,
		 *sec_bss,
		 *sec_idata,
		 *sec_rdata,
		 *sec_rel,
		 *sec_symtab;
void init_coff();
Section* new_section(char* name, int Characteristics);
Section* new_coffsym_section(char* symtab_name, int Characteristics, char* strtab_name);
int add_coffsym(Section* symtab, char* name, int val, int sec_index, short type, char StorageClass);
int search_coffsym(Section* symtab, char* sym_name);
void* section_ptr_add(Section* sec, int size);
void realloc_section(Section* sec, int new_size);
char* add_coffstr(Section* sec, char* name);
void write_obj(char* name);