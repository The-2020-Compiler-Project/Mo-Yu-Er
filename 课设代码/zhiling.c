//操作数存储结构
typedef struct Operand

{

	Type type;

	unsigned short r;

	int value;

	struct Symbol * sym;

}Operand;
```

//操作数栈相关操作

```c
void operand_push(Type * type, int r, int value)

{

	if (optop >= opstack + (OPSTACK_SIZE - 1))

		error（"内存分配失败"）;

	optop++;

	optop->type = *type;

	optop->r = r;

	optop->value = value;

}



void operand pop()

{

	optop--;

}

void operand_swap()

{

	Operand tmp;

	tmp = optop[0];

	optop[0] = optop[-1];

	optop[-1] = tmp;

}

void operand_assign(Operand *opd, int t, int r, int value)

{

	opd->type.t = t;

	opd->r = r;

	opd->value = value;

}