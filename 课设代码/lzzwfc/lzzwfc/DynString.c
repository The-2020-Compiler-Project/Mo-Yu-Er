#include "DynString.h"
#include <stdio.h>
#include <stdlib.h>
void str_init(DynString* pstr, int init_size)
{
	if (pstr != NULL)
	{
		pstr->count = 0;
		pstr->capacity = init_size;
		pstr->str = (char*)malloc(sizeof(char) * init_size);
	}
}

void str_free(DynString* pstr)
{
	if (pstr != NULL)
	{
		pstr->count = 0;
		pstr->capacity = 0;
		if (pstr->str != NULL)
		{
			free(pstr->str);
		}
	}
}

void str_cat(DynString* pstr, char ch)
{
	pstr->count += 1;
	if (pstr->count > pstr->capacity)
	{
		str_realloc(pstr, pstr->count);
	}
	pstr->str[pstr->count - 1] = ch;
}

void str_print(DynString* pstr)
{
	for (int i = 0; i < pstr->count; i++)
	{
		printf("%c", pstr->str[i]);
	}
}

void str_realloc(DynString* pstr, int new_size)
{
	int capacity = pstr->capacity;
	char* str;
	/* 成倍扩展,若每次拓展一个,重复调用realloc,造成浪费 */
	while (capacity < new_size)
	{
		capacity *= 2;
	}
	str = realloc(pstr->str, capacity);
	if (str == NULL)
	{
		//错误处理
		//error("内存分配失败");
	}
	pstr->capacity = capacity;
	pstr->str = str;
}

void str_reset(DynString* pstr)
{
	str_free(pstr);
	str_init(pstr, 16);
}
