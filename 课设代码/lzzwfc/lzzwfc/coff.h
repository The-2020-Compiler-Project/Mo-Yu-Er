#pragma once
#include <Windows.h>
#include "DynArray.h"
int nsec_images;	//number of sections in the image file

#pragma pack(push, 1)	//��ԭ���Ľṹ����뷽ʽѹջ��������1�ֽڶ��룬Ҳ���ǲ�����
//������Ҳ���ܽṹ�����
typedef struct Section
{
	IMAGE_SECTION_HEADER sh;	//��ͷ
	char* data;					//������
	int idx;					//����ţ���1��ʼ
	int sec_size;				//�ڵĴ�С������SizeOfRawData
	struct Section* link;		//���ű�ں��ַ�������ͬһ�ڣ�������һ��
	int* hashtable;				//��ϣ��
	int data_offset;			//�����data�ı����ַ
}Section;

//���ű��е�һ���¼���ֲ�ĳ���Ϊ18�ֽڣ����ű������еļ�¼����
//����ṹ�岻�ܶ��룬���뱣֤CoffSym��18�ֽ�
typedef struct CoffSym
{
	DWORD Name;		//��������,ʵ������ƫ�Ƶ�ַ�����ַ�������
	DWORD Next;		//�й�ϣ��ͻ�������һ�����ܵ�ƫ�Ƶ�ַ
	DWORD Value;				// �������ص�ֵ
	short Section;			// �ڱ������(��1��ʼ),���Ա�ʶ����˷��ŵĽ�*/
	WORD  Type;				// һ����ʾ���͵�����
	BYTE  StorageClass;		// ����һ����ʾ�洢����ö������ֵ
	BYTE  NumberOfAuxSymbols;	// ���ڱ���¼����ĸ������ű���ĸ���
}CoffSym;

#pragma pack(pop)
DynArray sections;	//�����飬���н���Ϣ������������
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