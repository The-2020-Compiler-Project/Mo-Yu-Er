#pragma once
#include "DynArray.h"
#include "DynString.h"
#include <stdio.h>
enum eTokenCode
{
    /* 运算符及分隔符 */
    TK_PLUS,		// + 加号
    TK_MINUS,		// - 减号
    TK_STAR,		// * 星号
    TK_DIVIDE,		// / 除号
    TK_MOD,			// % 求余运算符
    TK_EQ,			// == 等于号
    TK_NEQ,			// != 不等于号
    TK_LT,			// < 小于号
    TK_LEQ,			// <= 小于等于号
    TK_GT,			// > 大于号
    TK_GEQ,			// >= 大于等于号
    TK_ASSIGN,		// = 赋值运算符 
    TK_POINTSTO,	// -> 指向结构体成员运算符
    TK_DOT,			// . 结构体成员运算符
    TK_AND,         // & 地址与运算符
    TK_OPENPA,		// ( 左圆括号
    TK_CLOSEPA,		// ) 右圆括号
    TK_OPENBR,		// [ 左中括号
    TK_CLOSEBR,		// ] 右圆括号
    TK_BEGIN,		// { 左大括号
    TK_END,			// } 右大括号
    TK_SEMICOLON,	// ; 分号    
    TK_COMMA,		// , 逗号
    TK_ELLIPSIS,	// ... 省略号
    TK_EOF,			// 文件结束符

    /* 常量 */
    TK_CINT,		// 整型常量
    TK_CCHAR,		// 字符常量
    TK_CSTR,		// 字符串常量

    /* 关键字 */
    KW_CHAR,		// char关键字
    KW_SHORT,		// short关键字
    KW_INT,			// int关键字
    KW_VOID,		// void关键字  
    KW_STRUCT,		// struct关键字   
    KW_IF,			// if关键字
    KW_ELSE,		// else关键字
    KW_FOR,			// for关键字
    KW_CONTINUE,	// continue关键字
    KW_BREAK,		// break关键字   
    KW_RETURN,		// return关键字
    KW_SIZEOF,		// sizeof关键字

    KW_ALIGN,		// __align关键字	
    KW_CDECL,		// __cdecl关键字 standard c call
    KW_STDCALL,     // __stdcall关键字 pascal c call

    /* 标识符 */
    TK_IDENT
};

/*********************单词表定义及操作开始*************************************/
/* 单词数据结构 */
typedef struct TkWord
{
    int tkcode;
    struct TkWord* next;
    char* word;
    //如果是结构体变量的话，sym_struct指向符号表中结构体定义,sym_identifier为空
    //如果是普通变量的话，sym_struct为空，sym_identifier指向符号表
    struct Symbol* sym_struct;
    struct Symbol* sym_identifier;
}TkWord;
#define MAXKEY 1024
TkWord* tk_hashtable[MAXKEY];
DynArray tktable;

/* 单词表的操作 */
unsigned int ElfHash(char* key);
TkWord* TkWord_direct_insert(TkWord* pTK);
TkWord* TkWord_search(char* p, int key);
TkWord* TkWord_insert(char* p);
void* mallocz(int size);
/*********************单词表定义及操作结束************************************/


/*********************词法分析全局变量及函数声明开始**************************/
extern char ch;
extern FILE* fin;
extern DynString tkstr; //词法分析临时保存单词
extern DynString outstr;    //用作输出单词,转义字符可直接输出
extern int tkvalue; //整形常量值，比如在计算数组长度时有用
void init_lexial();
void get_ch();
void skip_white_space();
void preprocess();
void get_token();
int is_letter(char c);
int is_digit(char c);
void parse_identifier();
void parse_num();
void parse_cchar();
void parse_cstr();
void parse_comment();
char* get_tkstr(int token);
void color_token();
void test_lexial();
/*********************词法分析全局变量及函数声明结束**************************/