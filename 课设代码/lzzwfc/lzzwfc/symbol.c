#include "symbol.h"
#include "lexial.h"
#include "grammer.h"
#include <stdlib.h>
//���Ų�����ű�
Symbol* sym_push(Stack* s, int v, int r, int c, Type* type)
{
	Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
	sym->v = v;
	sym->r = r;
	sym->c = c;
	sym->type.t = type->t;		//�ṹ�廹��һһ��ֵ�ĺ�
	sym->type.ref = type->ref;
	stack_push(s, sym, sizeof(Symbol));
	//��������ֱ�ӷ���
	if (v == -1)
	{
		return sym;
	}
	//�洢���Ϳ����ж����ͨ���߼��򱣴���һ��
	//0010 0000 0000 0000 0000 0000 0000 0000������3λ�ǽṹ����ŵı�־λ
	//���r & 0010 0000 0000 0000 0000 0000 0000 0000���ṹ�Ƿ�Ϊ0�Ϳ����ж��Ƿ��ǽṹ�����
	if (r & SC_STRUCT)
	{
		((TkWord*)tktable.data[v])->sym_struct = sym;
	}
	//���ǽṹ����ţ��Ǳ���
	else
	{
		((TkWord*)tktable.data[v])->sym_identifier = sym;
	}
	return sym;
}

//scope�����򣬲���ȫ�ַ��ű��Ǿֲ����ű�
Symbol* var_sym_put(int r, int v, int addr, Type* type)
{
	//����������ǻ�ȡ����λ���ɴ˻�֪��ȫ�ֻ��Ǿֲ�
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
			error("����%s�ض��壡", get_tkstr(v));
		}
		sym_push(&global_sym_stack, v, r, addr, type);
	}
}

//����������ջ
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

//���ұ�����������
Symbol* sym_search(int v)
{
	//���ʱ�������ǵ��ʱ���-1
	if (v >= tktable.count)
	{
		return NULL;
	}
	return ((TkWord*)tktable.data[v])->sym_identifier;
}

//����ָ�������ͷ�巨
Symbol* mk_pointer(Type* type, int scope)
{
	Symbol* sym; //symΪ�������ţ�����ľ���*
	if (scope == SC_GLOBAL)
	{
		sym = sym_push(&global_sym_stack, -1, SC_ANOM, -1, type);
	}
	else if (scope == SC_LOCAL)
	{
		sym = sym_push(&local_sym_stack, -1, SC_ANOM, -1, type);
	}
	//�����൱��ͷ�����������Ԫ����һ����������
	type->t = T_PTR;
	type->ref = sym;
	return sym;
}

//�����������ͣ�β�巨
Symbol* mk_array(Type* type, int array_size, int scope)
{
	Symbol* sym; //symΪ�������ţ�����ľ���[]
	if (scope == SC_GLOBAL)
	{
		sym = sym_push(&global_sym_stack, -1, SC_ANOM, array_size, type);
	}
	else if (scope == SC_LOCAL)
	{
		sym = sym_push(&local_sym_stack, -1, SC_ANOM, array_size, type);
	}
	//�����൱��ͷ�����������Ԫ����һ����������
	
	//�������л�û��Ԫ��
	if (!type->ref)
	{
		sym->type.t = type->t;
		sym->type.ref = NULL;
		type->t = T_PTR | T_ARRAY;
		type->ref = sym;
	}
	else
	{
		Symbol* ps = type->ref;	//������β�ڵ�
		while (ps->type.ref != NULL)
		{
			ps = ps->type.ref;
		}
		//β��
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
	Stack tmp_stack;	//��ʱ�洢�������β�
	Symbol* end = NULL;	//ʼ��ָ��β�ڵ�
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
