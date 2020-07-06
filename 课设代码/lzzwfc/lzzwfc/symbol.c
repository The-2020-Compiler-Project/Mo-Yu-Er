#include "symbol.h"
#include "lexial.h"
#include "grammer.h"
#include <stdlib.h>
//符号插入符号表
Symbol* sym_push(Stack* s, int v, int r, int c, Type* type)
{
	Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
	sym->v = v;
	sym->r = r;
	sym->c = c;
	sym->type.t = type->t;		//结构体还是一一赋值的好
	sym->type.ref = type->ref;
	stack_push(s, sym, sizeof(Symbol));
	//匿名符号直接返回
	if (v == -1)
	{
		return sym;
	}
	//存储类型可能有多个，通过逻辑或保存在一起
	//0010 0000 0000 0000 0000 0000 0000 0000正数第3位是结构体符号的标志位
	//因此r & 0010 0000 0000 0000 0000 0000 0000 0000看结构是否为0就可以判断是否是结构体符号
	if (r & SC_STRUCT)
	{
		((TkWord*)tktable.data[v])->sym_struct = sym;
	}
	//不是结构体符号，是变量
	else
	{
		((TkWord*)tktable.data[v])->sym_identifier = sym;
	}
	return sym;
}

//scope作用域，插入全局符号表还是局部符号表
Symbol* var_sym_put(int r, int v, int addr, Type* type)
{
	//掩码的作用是获取低两位，由此获知是全局还是局部
	int scope = r & SC_VALMASK;
	if (scope == SC_LOCAL)
	{
		sym_push(&local_sym_stack, v, r, addr, type);
	}
	else if (scope == SC_GLOBAL)
	{
		Symbol* sym = sym_search(v);
		if (sym)
		{
			error("符号%s重定义！", get_tkstr(v));
		}
		sym_push(&global_sym_stack, v, r, addr, type);
	}
}

//函数符号入栈
Symbol* func_sym_put(Type* type)
{
	Type* anom_type = (Type*)malloc(sizeof(Type));
	anom_type->t = type->t;
	anom_type->ref = NULL;
	Symbol* sym_anom = sym_push(&global_sym_stack, -1, SC_ANOM, -1, anom_type);
	type->t = T_FUNC;
	type->ref = sym_anom;
	Symbol* func_sym = sym_push(&global_sym_stack, delay_v, 0, SC_GLOBAL | SC_SYM, type);
	return sym_anom;
}

//查找变量、函数名
Symbol* sym_search(int v)
{
	//单词编码最大是单词表长度-1
	if (v >= tktable.count)
	{
		return NULL;
	}
	return ((TkWord*)tktable.data[v])->sym_identifier;
}

//处理指针变量，头插法
Symbol* mk_pointer(Type* type, int scope)
{
	Symbol* sym; //sym为匿名符号，代表的就是*
	if (scope == SC_GLOBAL)
	{
		sym = sym_push(&global_sym_stack, -1, SC_ANOM, -1, type);
	}
	else if (scope == SC_LOCAL)
	{
		sym = sym_push(&local_sym_stack, -1, SC_ANOM, -1, type);
	}
	//这里相当于头插链表，插入的元素是一个匿名符号
	type->t = T_PTR;
	type->ref = sym;
	return sym;
}

//处理数组类型，尾插法
Symbol* mk_array(Type* type, int array_size, int scope)
{
	Symbol* sym; //sym为匿名符号，代表的就是[]
	if (scope == SC_GLOBAL)
	{
		sym = sym_push(&global_sym_stack, -1, SC_ANOM, array_size, type);
	}
	else if (scope == SC_LOCAL)
	{
		sym = sym_push(&local_sym_stack, -1, SC_ANOM, array_size, type);
	}
	//这里相当于头插链表，插入的元素是一个匿名符号
	
	//当链表中还没有元素
	if (!type->ref)
	{
		sym->type.t = type->t;
		sym->type.ref = NULL;
		type->t = T_PTR | T_ARRAY;
		type->ref = sym;
	}
	else
	{
		Symbol* ps = type->ref;	//遍历到尾节点
		while (ps->type.ref != NULL)
		{
			ps = ps->type.ref;
		}
		//尾插
		sym->type.t = ps->type.t;
		sym->type.ref = NULL;
		ps->type.t = T_PTR | T_ARRAY;
		ps->type.ref = sym;

	}
	return sym;
}

Symbol* mk_paralist(Symbol* s, int count_para)
{
	Symbol* tmp_sym;
	Stack tmp_stack;	//临时存储弹出的形参
	Symbol* end = NULL;	//始终指向尾节点
	stack_init(&tmp_stack, count_para + 1);
	for(int i = 0; i < count_para; i++)
	{
		Symbol* test = stack_get_top(&local_sym_stack);
		stack_push(&tmp_stack, stack_get_top(&local_sym_stack), sizeof(Symbol));
		stack_pop(&local_sym_stack);
	}
	for (int i = 0; i < count_para; i++)
	{
		tmp_sym = stack_get_top(&tmp_stack);
		tmp_sym = stack_push(&local_sym_stack, tmp_sym, sizeof(Symbol));
		stack_pop(&tmp_stack);
		tmp_sym->r = SC_PARAMS;
		if (i == 0)
		{
			s->next = tmp_sym;
			end = tmp_sym;
			end->next = NULL;
			continue;
		}
		end->next = tmp_sym;
		end = tmp_sym;
		end->next = NULL;
	}

}
