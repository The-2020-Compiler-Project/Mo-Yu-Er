#pragma once
#include "stack.h"
//���ŷ��ڷ���ջ��
Stack global_sym_stack;
Stack local_sym_stack;

typedef struct Type
{
	int t;
	struct Symbol* ref;
}Type;

/* ���ŵ����ݽṹ */
typedef struct Symbol
{
	int v;						//���ŵĵ��ʱ���
	int r;						//���Ŵ洢����
	int c;						//���Ź���ֵ
	Type type;					//������������
	struct Symbol* next;		//��������������
	struct Symbol* prev_tok;	//ָ��ǰһ�������ͬ������
}Symbol;




/* �洢���� */
enum e_StorageClass
{
	SC_GLOBAL = 0x00f0,		// �������������ͳ������ַ��������ַ�������,ȫ�ֱ���,��������          
	SC_LOCAL = 0x00f1,		// ջ�б���
	SC_LLOCAL = 0x00f2,       // �Ĵ���������ջ��
	SC_CMP = 0x00f3,       // ʹ�ñ�־�Ĵ���
	SC_VALMASK = 0x00ff,       // �洢�������룬��Ϊ�洢����r������SC_GLOBAL|SC_LVAL��r&SC_VALMASK
							  // ֮�����¶�����洢���͵ĵ���λ�������ж���ȫ�ֱ������Ǿֲ�����
	SC_LVAL = 0x0100,       // ��ֵ       
	SC_SYM = 0x0200,       // ����	

	SC_ANOM = 0x10000000,     // ��������
	SC_STRUCT = 0x20000000,     // �ṹ�����
	SC_MEMBER = 0x40000000,     // �ṹ��Ա����
	SC_PARAMS = 0x80000000,     // ��������
};

/* ���ͱ��� */
enum e_TypeCode
{
	/* types */
	T_INT = 0,			// ����                     
	T_CHAR = 1,			// �ַ���                 
	T_SHORT = 2,			// ������                       
	T_VOID = 3,			// ������                        
	T_PTR = 4,			// ָ��                          
	T_FUNC = 5,			// ����                    
	T_STRUCT = 6,			// �ṹ�� 

	T_BTYPE = 0x000f,		// ������������          
	T_ARRAY = 0x0010,		// ����
};
Symbol* sym_push(Stack* s, int v, int r, int c, Type* type);
Symbol* var_sym_put(int r, int v, int addr, Type* type);
Symbol* func_sym_put(Type* type);
Symbol* sym_search(int v);
Symbol* mk_pointer(Type* type, int scope);
Symbol* mk_array(Type* type, int array_size, int scope);
Symbol* mk_paralist(Symbol* s, int count_para);