#pragma once
#include "stack.h"
//符号放在符号栈中
Stack global_sym_stack;
Stack local_sym_stack;

typedef struct Type
{
	int t;
	struct Symbol* ref;
}Type;

/* 符号的数据结构 */
typedef struct Symbol
{
	int v;						//符号的单词编码
	int r;						//符号存储类型
	int c;						//符号关联值
	Type type;					//符号数据类型
	struct Symbol* next;		//关联的其他符号
	struct Symbol* prev_tok;	//指向前一个定义的同名符号
}Symbol;




/* 存储类型 */
enum e_StorageClass
{
	SC_GLOBAL = 0x00f0,		// 包括：包括整型常量，字符常量、字符串常量,全局变量,函数定义          
	SC_LOCAL = 0x00f1,		// 栈中变量
	SC_LLOCAL = 0x00f2,       // 寄存器溢出存放栈中
	SC_CMP = 0x00f3,       // 使用标志寄存器
	SC_VALMASK = 0x00ff,       // 存储类型掩码，因为存储类型r可以是SC_GLOBAL|SC_LVAL，r&SC_VALMASK
							  // 之后可以露出它存储类型的低两位，可以判断是全局变量还是局部变量
	SC_LVAL = 0x0100,       // 左值       
	SC_SYM = 0x0200,       // 符号	

	SC_ANOM = 0x10000000,     // 匿名符号
	SC_STRUCT = 0x20000000,     // 结构体符号
	SC_MEMBER = 0x40000000,     // 结构成员变量
	SC_PARAMS = 0x80000000,     // 函数参数
};

/* 类型编码 */
enum e_TypeCode
{
	/* types */
	T_INT = 0,			// 整型                     
	T_CHAR = 1,			// 字符型                 
	T_SHORT = 2,			// 短整型                       
	T_VOID = 3,			// 空类型                        
	T_PTR = 4,			// 指针                          
	T_FUNC = 5,			// 函数                    
	T_STRUCT = 6,			// 结构体 

	T_BTYPE = 0x000f,		// 基本类型掩码          
	T_ARRAY = 0x0010,		// 数组
};
Symbol* sym_push(Stack* s, int v, int r, int c, Type* type);
Symbol* var_sym_put(int r, int v, int addr, Type* type);
Symbol* func_sym_put(Type* type);
Symbol* sym_search(int v);
Symbol* mk_pointer(Type* type, int scope);
Symbol* mk_array(Type* type, int array_size, int scope);
Symbol* mk_paralist(Symbol* s, int count_para);