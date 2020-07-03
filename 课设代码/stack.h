#pragma once
#include <stdio.h>
#include <windows.h>
/*ջ����*/
typedef struct Stack
{
    void** base;	// ջ��ָ��
    void** top;		// ջ��ָ��
    int stacksize;	// ջ��ǰ��ʹ�õ��������,��Ԫ��Ϊ��λ	
} Stack;

void stack_init(Stack* stack, int initsize);
void* stack_push(Stack* stack, void* element, int size);
void stack_pop(Stack* stack);
void* stack_get_top(Stack* stack);
int stack_is_empty(Stack* stack);
void stack_destroy(Stack* stack);