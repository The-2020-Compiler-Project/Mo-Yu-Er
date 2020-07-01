#pragma once
typedef struct DynString
{
	int count;		//�ַ����ĳ���
	int capacity;	//����������
	char* str;		//�ַ���ָ��
}DynString;

/************************
 * ����:��̬�ַ�����ʼ��
 * pstr:����ʼ�����ַ���
 * init_size:��ʼ����С
*************************/
void str_init(DynString* pstr, int init_size);

/************************
 * ����:�ͷŶ�̬�ַ���
 * pstr:���ͷŵ��ַ���
*************************/
void str_free(DynString* pstr);
void str_realloc(DynString* pstr, int new_size);
void str_reset(DynString* pstr);
void str_free(DynString* pstr);
void str_cat(DynString* pstr, char ch);
void str_print(DynString* pstr);