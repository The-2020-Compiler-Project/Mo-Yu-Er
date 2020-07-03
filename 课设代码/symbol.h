#pragma once
/* ���ʴ洢�ṹ���� */
#include"stack.h"
typedef struct TkWord
{
    int  tkcode;					// ���ʱ��� 
    struct TkWord* next;			// ָ���ϣ��ͻ����������
    char* spelling;					// �����ַ��� 
    struct Symbol* sym_struct;		// ָ�򵥴�����ʾ�Ľṹ����
    struct Symbol* sym_identifier;	// ָ�򵥴�����ʾ�ı�ʶ��
} TkWord;

#define MAXKEY	1024				// ��ϣ������

extern TkWord* tk_hashtable[MAXKEY];// ���ʹ�ϣ��
#define CH_EOF   (-1)				// �ļ�β��ʶ
/* ���ʹ洢�ṹ���� */
typedef struct Type
{
    int t;
    struct Symbol* ref;
} Type;

/* ���Ŵ洢�ṹ���� */
typedef struct Symbol
{
    int v;						// ���ŵĵ��ʱ���
    int r;						// ���Ź����ļĴ���
    int c;						// ���Ź���ֵ��ṹ��Ĵ�С��������ƫ����
    Type type;					// ��������
    struct Symbol* next;		// �������������ţ��ṹ�嶨�������Ա�������ţ��������������������
    struct Symbol* prev_tok;	// ָ��ǰһ�����ͬ������
} Symbol;
extern Type char_pointer_type, int_type, default_func_type;
extern Symbol* sym_sec_rdata;
Symbol* struct_search(int v);
Symbol* sym_search(int v);
Symbol* sym_direct_push(Stack* ss, int v, Type* type, int c);
Symbol* sym_push(int v, Type* type, int r, int c);
Symbol* func_sym_push(int v, Type* type);
Symbol* var_sym_put(Type* type, int r, int v, int addr);
