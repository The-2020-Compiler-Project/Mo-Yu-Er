#include "operand.h"
Operand opstack[OPSTACK_SIZE]; //²Ù×÷ÊýÕ»
int optop = 0;	//Õ»¶¥Ö¸Õë

void push_operand(Type* type, int r, int value)
{
	Operand op;
	if (optop >= OPSTACK_SIZE)
	{
		error("²Ù×÷Õ»Òç³ö");
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

