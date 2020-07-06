#include "stack.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>

//void** base = (void**)malloc(sizeof(void*) * 10);
//void** top = base;
//top++;
//printf("%d", top - base);
//结果为1
//base = base + 1，是整体指针加上元素的大小，等同于base++
void stack_init(Stack* stack, int init_size)
{
	stack->base = (void**)malloc(sizeof(void**) * init_size);
	if (!stack->base)
	{
		error("栈内存分配失败");
	}
	stack->top = stack->base;
	stack->stacksize = init_size;
}

//注意：
//base指向整个栈的内存单元
//*base是第一个栈元素指向的内存单元
//**base是具体的数据值
void* stack_push(Stack* stack, void* element, int size)
{
	//栈满了，需要扩容
	if (stack->top - stack->base >= stack->stacksize)
	{
		int new_stack_size = stack->stacksize * 2;
		stack->base = (void**)realloc(stack->base, sizeof(void**) * new_stack_size);
		stack->top = stack->base + stack->stacksize;
		stack->stacksize = new_stack_size;
	}
	*stack->top = (void*)malloc(size);
	memcpy(*(stack->top), element, size);	//注意这里有坑啊，这里的size应该是实打实的内存大小，不是指针大小
	stack->top++;
	return *(stack->top - 1);
}

void stack_pop(Stack* stack)
{
	if (!stack_is_empty(stack))
	{
		stack->top--;
		//free(*stack->top);
		*stack->top = NULL;
	}
}

void* stack_get_top(Stack* stack)
{
	if (stack_is_empty(stack))
	{
		return NULL;
	}
	void** element = stack->top - 1;
	return *element;
}

int stack_is_empty(Stack* stack)
{
	int is_empty;
	is_empty = stack->top == stack->base ? 1 : 0;
	return is_empty;
}