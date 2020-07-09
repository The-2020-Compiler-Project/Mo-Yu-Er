#include "DynArray.h"
#include <stdlib.h>
void array_init(DynArray* parr, int init_size)
{
	if (parr)
	{
		parr->count = 0;
		parr->capacity = init_size;
		parr->data = (void**)malloc(sizeof(void*) * init_size);
	}
}

void array_free(DynArray* parr)
{
	if (parr)
	{
		void** p;
		parr->capacity = 0;
		for (p = parr->data; parr->count != 0; --parr->count, ++p)
		{
			if (*p)
			{
				free(*p);
				*p = NULL;
			}
		}
		parr->data = NULL;
	}
}

void array_push(DynArray* parr, void* data)
{
	parr->count += 1;
	if (parr->count > parr->capacity)
	{
		array_realloc(parr, parr->count);
	}
	parr->data[parr->count - 1] = data;

}

void array_pop(DynArray* parr)
{
	parr->count -= 1;
	parr->data[parr->count] = NULL;
}

int array_search(DynArray* parr, int key)
{
	int** p = (int**)parr->data;
	for (int i = 0; i < parr->count; i++, p++)
	{
		if (**p == key)
		{
			return i;
		}
	}
	return -1;
}

void array_realloc(DynArray* parr, int new_size)
{
	int capacity = parr->capacity;
	void* data;
	while (capacity < new_size)
	{
		capacity *= 2;
	}
	data = realloc(parr->data, capacity * sizeof(void*));
	if (!data)
	{
		//´íÎó´¦Àí
	}
	parr->capacity = capacity;
	parr->data = data;
}
