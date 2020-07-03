#pragma once
/* 单词存储结构定义 */
#include"stack.h"
typedef struct TkWord
{
    int  tkcode;					// 单词编码 
    struct TkWord* next;			// 指向哈希冲突的其它单词
    char* spelling;					// 单词字符串 
    struct Symbol* sym_struct;		// 指向单词所表示的结构定义
    struct Symbol* sym_identifier;	// 指向单词所表示的标识符
} TkWord;

#define MAXKEY	1024				// 哈希表容量

extern TkWord* tk_hashtable[MAXKEY];// 单词哈希表
#define CH_EOF   (-1)				// 文件尾标识
/* 类型存储结构定义 */
typedef struct Type
{
    int t;
    struct Symbol* ref;
} Type;

/* 符号存储结构定义 */
typedef struct Symbol
{
    int v;						// 符号的单词编码
    int r;						// 符号关联的寄存器
    int c;						// 符号关联值如结构体的大小，变量的偏移量
    Type type;					// 符号类型
    struct Symbol* next;		// 关联的其它符号，结构体定义关联成员变量符号，函数定义关联参数符号
    struct Symbol* prev_tok;	// 指向前一定义的同名符号
} Symbol;
extern Type char_pointer_type, int_type, default_func_type;
extern Symbol* sym_sec_rdata;
Symbol* struct_search(int v);
Symbol* sym_search(int v);
Symbol* sym_direct_push(Stack* ss, int v, Type* type, int c);
Symbol* sym_push(int v, Type* type, int r, int c);
Symbol* func_sym_push(int v, Type* type);
Symbol* var_sym_put(Type* type, int r, int v, int addr);
