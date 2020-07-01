#pragma once
typedef struct DynArray
{
	int count;		//当前元素个数
	int capacity;	//缓冲区长度(最多能承载的元素个数)
	void** data;	//指针数组,使用void*便于抽象封装
}DynArray;

void array_init(DynArray* parr, int init_size);
void array_free(DynArray* parr);
void array_push(DynArray* parr, void* data);
void array_pop(DynArray* parr);
int array_search(DynArray* parr, int key);
void array_realloc(DynArray* parr, int new_size);