#pragma once
#include "DynArray.h"
#include "DynString.h"
#include <stdio.h>
enum eTokenCode
{
    /* ��������ָ��� */
    TK_PLUS,		// + �Ӻ�
    TK_MINUS,		// - ����
    TK_STAR,		// * �Ǻ�
    TK_DIVIDE,		// / ����
    TK_MOD,			// % ���������
    TK_EQ,			// == ���ں�
    TK_NEQ,			// != �����ں�
    TK_LT,			// < С�ں�
    TK_LEQ,			// <= С�ڵ��ں�
    TK_GT,			// > ���ں�
    TK_GEQ,			// >= ���ڵ��ں�
    TK_ASSIGN,		// = ��ֵ����� 
    TK_POINTSTO,	// -> ָ��ṹ���Ա�����
    TK_DOT,			// . �ṹ���Ա�����
    TK_AND,         // & ��ַ�������
    TK_OPENPA,		// ( ��Բ����
    TK_CLOSEPA,		// ) ��Բ����
    TK_OPENBR,		// [ ��������
    TK_CLOSEBR,		// ] ��Բ����
    TK_BEGIN,		// { �������
    TK_END,			// } �Ҵ�����
    TK_SEMICOLON,	// ; �ֺ�    
    TK_COMMA,		// , ����
    TK_ELLIPSIS,	// ... ʡ�Ժ�
    TK_EOF,			// �ļ�������

    /* ���� */
    TK_CINT,		// ���ͳ���
    TK_CCHAR,		// �ַ�����
    TK_CSTR,		// �ַ�������

    /* �ؼ��� */
    KW_CHAR,		// char�ؼ���
    KW_SHORT,		// short�ؼ���
    KW_INT,			// int�ؼ���
    KW_VOID,		// void�ؼ���  
    KW_STRUCT,		// struct�ؼ���   
    KW_IF,			// if�ؼ���
    KW_ELSE,		// else�ؼ���
    KW_FOR,			// for�ؼ���
    KW_CONTINUE,	// continue�ؼ���
    KW_BREAK,		// break�ؼ���   
    KW_RETURN,		// return�ؼ���
    KW_SIZEOF,		// sizeof�ؼ���

    KW_ALIGN,		// __align�ؼ���	
    KW_CDECL,		// __cdecl�ؼ��� standard c call
    KW_STDCALL,     // __stdcall�ؼ��� pascal c call

    /* ��ʶ�� */
    TK_IDENT
};

/*********************���ʱ��弰������ʼ*************************************/
/* �������ݽṹ */
typedef struct TkWord
{
    int tkcode;
    struct TkWord* next;
    char* word;
    //����ǽṹ������Ļ���sym_structָ����ű��нṹ�嶨��,sym_identifierΪ��
    //�������ͨ�����Ļ���sym_structΪ�գ�sym_identifierָ����ű�
    struct Symbol* sym_struct;
    struct Symbol* sym_identifier;
}TkWord;
#define MAXKEY 1024
TkWord* tk_hashtable[MAXKEY];
DynArray tktable;

/* ���ʱ�Ĳ��� */
unsigned int ElfHash(char* key);
TkWord* TkWord_direct_insert(TkWord* pTK);
TkWord* TkWord_search(char* p, int key);
TkWord* TkWord_insert(char* p);
void* mallocz(int size);
/*********************���ʱ��弰��������************************************/


/*********************�ʷ�����ȫ�ֱ���������������ʼ**************************/
extern char ch;
extern FILE* fin;
extern DynString tkstr; //�ʷ�������ʱ���浥��
extern DynString outstr;    //�����������,ת���ַ���ֱ�����
extern int tkvalue; //���γ���ֵ�������ڼ������鳤��ʱ����
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
/*********************�ʷ�����ȫ�ֱ�����������������**************************/