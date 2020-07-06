#pragma once
typedef struct Stack
{
	void** base;
	void** top; //栈顶指针，始终指向空白处，也就比栈顶元素大一
	int stacksize;
}Stack;
void stack_init(Stack* stack, int init_size);
void* stack_push(Stack* stack, void* element, int size);
void stack_pop(Stack* stack);
void* stack_get_top(Stack* stack);
int stack_is_empty(Stack* stack);