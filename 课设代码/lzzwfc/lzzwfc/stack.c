#include "stack.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>

//void** base = (void**)malloc(sizeof(void*) * 10);
//void** top = base;
//top++;
//printf("%d", top - base);
//���Ϊ1
//base = base + 1��������ָ�����Ԫ�صĴ�С����ͬ��base++
void stack_init(Stack* stack, int init_size)
{
	stack->base = (void**)malloc(sizeof(void**) * init_size);
	if (!stack->base)
	{
		error("ջ�ڴ����ʧ��");
	}
	stack->top = stack->base;
	stack->stacksize = init_size;
}

//ע�⣺
//baseָ������ջ���ڴ浥Ԫ
//*base�ǵ�һ��ջԪ��ָ����ڴ浥Ԫ
//**base�Ǿ��������ֵ
void* stack_push(Stack* stack, void* element, int size)
{
	//ջ���ˣ���Ҫ����
	if (stack->top - stack->base >= stack->stacksize)
	{
		int new_stack_size = stack->stacksize * 2;
		stack->base = (void**)realloc(stack->base, sizeof(void**) * new_stack_size);
		stack->top = stack->base + stack->stacksize;
		stack->stacksize = new_stack_size;
	}
	*stack->top = (void*)malloc(size);
	memcpy(*(stack->top), element, size);	//ע�������пӰ��������sizeӦ����ʵ��ʵ���ڴ��С������ָ���С
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