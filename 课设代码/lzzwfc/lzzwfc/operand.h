//������������弰������ջ����
#pragma once
#include "symbol.h"
#define OPSTACK_SIZE 256
typedef struct Operand
{
	Type type; //��������
	unsigned short r; //���Դ��д洢������Ϣ��Ҳ�����ǼĴ������
	int value;
	struct Symbol* sym;
}Operand;

extern Operand opstack[OPSTACK_SIZE]; //������ջ
extern int optop;	//ջ��ָ��
void push_operand(Type* type, int r, int value);
void pop_operand();
