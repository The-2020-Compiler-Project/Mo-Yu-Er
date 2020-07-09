//操作数结果定义及操作数栈操作
#pragma once
#include "symbol.h"
#define OPSTACK_SIZE 256
typedef struct Operand
{
	Type type; //数据类型
	unsigned short r; //可以带有存储类型信息，也可以是寄存器编号
	int value;
	struct Symbol* sym;
}Operand;

extern Operand opstack[OPSTACK_SIZE]; //操作数栈
extern int optop;	//栈顶指针
void push_operand(Type* type, int r, int value);
void pop_operand();
