#pragma once
typedef struct DynString
{
	int count;		//字符串的长度
	int capacity;	//缓冲区容量
	char* str;		//字符串指针
}DynString;

/************************
 * 功能:动态字符串初始化
 * pstr:待初始化的字符串
 * init_size:初始化大小
*************************/
void str_init(DynString* pstr, int init_size);

/************************
 * 功能:释放动态字符串
 * pstr:待释放的字符串
*************************/
void str_free(DynString* pstr);
void str_realloc(DynString* pstr, int new_size);
void str_reset(DynString* pstr);
void str_free(DynString* pstr);
void str_cat(DynString* pstr, char ch);
void str_print(DynString* pstr);