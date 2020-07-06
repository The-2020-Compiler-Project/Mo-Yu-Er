#pragma once
typedef struct DynArray
{
	int count;		//��ǰԪ�ظ���
	int capacity;	//����������(����ܳ��ص�Ԫ�ظ���)
	void** data;	//ָ������,ʹ��void*���ڳ����װ
}DynArray;

void array_init(DynArray* parr, int init_size);
void array_free(DynArray* parr);
void array_push(DynArray* parr, void* data);
void array_pop(DynArray* parr);
int array_search(DynArray* parr, int key);
void array_realloc(DynArray* parr, int new_size);