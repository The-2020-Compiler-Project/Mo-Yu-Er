#include "operand.h"
Operand opstack[OPSTACK_SIZE]; //������ջ
int optop = 0;	//ջ��ָ��

void push_operand(Type* type, int r, int value)
{
	Operand op;
	if (optop >= OPSTACK_SIZE)
	{
		error("����ջ���");
	}
	op.type = *type;
	op.r = r;
	op.value = value;
	opstack[optop] = op;
	optop++;

}

void pop_operand()
{
	optop--;
}

