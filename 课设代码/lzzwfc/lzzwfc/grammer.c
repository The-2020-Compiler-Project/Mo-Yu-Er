#include "lzzwfc.h"
#include "lexial.h"
#include "grammer.h"
#include "symbol.h"
#include <stdlib.h>
int token;
int delay_v;
void translation_unit()
{
	while (token != TK_EOF)
	{
		external_declaration();
	}
}

void external_declaration()
{
	Type* type = (Type*)malloc(sizeof(Type));
	if (!type_specifier(type))
	{
		expect("<�������ַ�>");
	}
	if (token == TK_SEMICOLON)
	{
		get_token();
		return;
	}
	else
	{
		declarator(type, SC_GLOBAL);
		//������뿴select���ϲ���ȷ��

		if (token == TK_BEGIN)
		{
			funcbody();
		}
		//������뿴select���ϲ���ȷ��
		else if (token == TK_ASSIGN)
		{
			get_token();
			initializer();
		}
		while (token == TK_COMMA)
		{
			Type* type = (Type*)malloc(sizeof(Type));
			get_token();
			init_declarator(type, SC_GLOBAL);	//����ȫ�ֱ���
		}
		skip(TK_SEMICOLON);
	}
}

void declaration(int scope)
{
	Type* type = (Type*)malloc(sizeof(Type));
	if (!type_specifier(type))
	{
		expect("�������ַ�");
	}
	if (token != TK_SEMICOLON)
	{
		init_declarator_list(type, scope);
	}
	skip(TK_SEMICOLON);
}

void init_declarator_list(Type* type, int scope)
{
	init_declarator(type, scope);
	while (token != TK_SEMICOLON)
	{
		skip(TK_COMMA);
		init_declarator(type, scope);
	}
}

void init_declarator(Type* type, int scope)
{
	declarator(type, scope);
	if (token == TK_ASSIGN)
	{
		get_token();
		initializer();
	}
	else
	{
		return;
	}
}

int type_specifier(Type* type)
{
	int is_type = 0;
	type->ref = NULL;	//��������û�й������ṹ��refָ��ṹ�嶨�壬����ָ��������
	switch (token)
	{
	case KW_INT:
		is_type = 1;
		get_token();
		type->t = T_INT;
		break;
	case KW_CHAR:
		is_type = 1;
		get_token();
		type->t = T_CHAR;
		break;
	case KW_SHORT:
		is_type = 1;
		get_token();
		type->t = T_SHORT;
		break;
	case KW_VOID:
		is_type = 1;
		get_token();
		type->t = T_VOID;
		break;
	case KW_STRUCT:
		is_type = 1;
		struct_specifier();
		break;
	default:
		break;
	}
	return is_type;
}

void struct_specifier()
{
	skip(KW_STRUCT);
	if (token < TK_IDENT)
	{
		error("�ؼ��ֲ�����Ϊ�ṹ����");
	}
	get_token();
	if (token == TK_BEGIN)
	{
		get_token();
		struct_declaration_list();
		skip(TK_END);
	}
}

void struct_declaration_list()
{
	while (token != TK_END)
	{
		struct_declaration();
	}
}

void struct_declaration()
{
	Type* type;
	if (is_type_specifier(token))
	{
		type_specifier(type);
	}
	declarator(type, SC_LOCAL);
	while (token != TK_SEMICOLON)
	{
		skip(TK_COMMA);
		declarator(type, SC_LOCAL);
	}
	skip(TK_SEMICOLON);
}

//����Ϊ��
void function_calling_convention()
{
	if (token == KW_CDECL || token == KW_STDCALL)
	{
		get_token();
		return;
	}
}

void struct_member_alignment()
{
	if (token == KW_ALIGN)
	{
		get_token();
		skip(TK_OPENPA);
		if (token == TK_CINT)
		{
			get_token();
		}
		else
		{
			expect("���γ���");
		}
		skip(TK_CLOSEPA);
	}
}
void declarator(Type* type, int scope)
{
	int addr = 0; //����
	int r = SC_SYM | scope;
	while (token == TK_STAR)
	{
		//ָ����������ű�
		mk_pointer(type, scope);
		get_token();
	}
	function_calling_convention();
	struct_member_alignment();
	direct_declarator(type, scope);
	//���Ǻ������ñ������ű������
	if ( (type->t & T_BTYPE) != T_FUNC)
	{
		//��ʶ��������ű�����嶯��
		if ((type->t & T_ARRAY) == 0)	//�������ͱ�־λΪ0��ʾ�������飬˵����������ֵ
		{
			r = r | SC_LVAL;
		}
		var_sym_put(r, delay_v, addr, type);
	}
	else
	{

	}
}

void direct_declarator(Type* type, int scope)
{
	if (token >= TK_IDENT)
	{
		delay_v = token; //�ü�¼�±�ʶ���ı�ţ����������׺���û��
		get_token();
		direct_declarator_postfix(type, scope);
	}
	else
	{
		expect("��ʶ��");
	}
}

void direct_declarator_postfix(Type* type, int scope)
{
	Symbol* sym;
	int array_size = -1;	//����ĳ���
	//�������������[��˵��������
	if (token == TK_OPENBR)
	{
		get_token();
		if (token == TK_CINT)
		{
			array_size = tkvalue;	//tkvalueΪ[]�е�����ֵ
			get_token();
		}
		skip(TK_CLOSEBR);
		//���������ű�����嶯��
		mk_array(type, array_size, scope);
		direct_declarator_postfix(type, scope);
	}
	//�����ʶ������(��˵���Ǻ���
	else if (token == TK_OPENPA)
	{
		int count_para = 0;	//��������
		sym = func_sym_put(type);
		get_token();
		if (is_type_specifier(token))
		{
			count_para = parameter_type_list();	//�������е��βα���
		}
		skip(TK_CLOSEPA);
		mk_paralist(sym, count_para);
	}
	//��ʶ����ɶҲû�У�˵������ͨ���͵ı���
	else
	{

	}
}

int parameter_type_list()
{
	int count_para = 0;	//�βθ���
	Type* type = (Type*)malloc(sizeof(Type));
	type_specifier(type);
	//tokenΪ*���߱�ʶ��
	if (token == TK_STAR || token >= TK_IDENT);
	{
		count_para++;
		declarator(type,SC_LOCAL);	//�������Ϊ�ֲ�����
	}
	while (token == TK_COMMA)
	{
		Type* new_type = (Type*)malloc(sizeof(Type));
		get_token();
		type_specifier(new_type);
		if (token == TK_STAR || token >= TK_IDENT)
		{
			count_para++;
			declarator(new_type, SC_LOCAL);
		}
	}
	if (token == TK_COMMA)
	{
		get_token();
		skip(TK_ELLIPSIS);
	}
	return count_para;
}

void funcbody()
{
	compound_statement();
}

void initializer()
{
	assignment_expression();
}

void statement()
{
	switch (token)
	{
	case TK_BEGIN:
		compound_statement();
		break;
	case KW_IF:
		if_statement();
		break;
	case KW_RETURN:
		return_statement();
		break;
	case KW_BREAK:
		break_statement();
		break;
	case KW_CONTINUE:
		continue_statement();
		break;
	case KW_FOR:
		for_statement();
		break;
	default:
		expression_statement();
		break;
	}
}

void compound_statement()
{
	skip(TK_BEGIN);
	while (is_type_specifier(token))
	{
		declaration(SC_LOCAL);	//��������б�Ϊ�ֲ�����
	}
	while (token != TK_END)
	{
		statement();
	}
	skip(TK_END);
}

int is_type_specifier(int v)
{
	if (v == KW_INT || v == KW_CHAR || v == KW_SHORT || v == KW_VOID || v==KW_STRUCT)
	{
		return 1;
	}
	return 0;
}

void expression_statement()
{
	if (token == TK_SEMICOLON)
	{
		get_token();
	}
	else
	{
		expression();
		skip(TK_SEMICOLON);
	}
}

void if_statement()
{
	skip(KW_IF);
	skip(TK_OPENPA);
	expression();
	skip(TK_CLOSEPA);
	statement();
	if (token == KW_ELSE)
	{
		get_token();
		statement();
	}
}

void for_statement()
{
	skip(KW_FOR);
	skip(TK_OPENPA);
	expression_statement();
	expression_statement();
	expression();
	skip(TK_CLOSEPA);
	statement();
}

void continue_statement()
{
	skip(KW_CONTINUE);
	skip(TK_SEMICOLON);
}

void break_statement()
{
	skip(KW_BREAK);
	skip(TK_SEMICOLON);
}

void return_statement()
{
	skip(KW_RETURN);
	if (token != TK_SEMICOLON)
	{
		expression();
	}
	skip(TK_SEMICOLON);
}

void expression()
{
	assignment_expression();
	if (token == TK_COMMA)
	{
		get_token();
		assignment_expression();
	}
}

void assignment_expression()
{
	equality_expression();
	if (token == TK_ASSIGN)
	{
		get_token();
		assignment_expression();
	}
}

void equality_expression()
{
	relational_expression();
	while (token == TK_EQ || token == TK_NEQ)
	{
		get_token();
		relational_expression();
	}
}

void relational_expression()
{
	additive_expression();
	while (token == TK_LT || token == TK_LEQ || token == TK_GT || token == TK_GEQ)
	{
		get_token();
		additive_expression();
	}
}

void additive_expression()
{
	multiplicative_expression();
	while (token == TK_PLUS || token == TK_MINUS)
	{
		get_token();
		multiplicative_expression();
	}
}

void multiplicative_expression()
{
	unary_expression();
	while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD)
	{
		get_token();
		unary_expression();
	}
}

void unary_expression()
{
	if (token == TK_AND || token == TK_STAR || token == TK_PLUS || token == TK_MINUS)
	{
		get_token();
		unary_expression();
	}
	else if (token == KW_SIZEOF)
	{
		sizeof_expression();
	}
	else
	{
		postfix_expression();
	}
}

void sizeof_expression()
{
	Type type;
	skip(KW_SIZEOF);
	skip(TK_OPENPA);
	type_specifier(&type);
	skip(TK_CLOSEPA);
}

void postfix_expression()
{
	primary_expression();
	while (1)
	{
		if (token == TK_OPENBR)
		{
			get_token();
			expression();
			skip(TK_CLOSEBR);
		}
		else if (token == TK_DOT || token == TK_POINTSTO)
		{
			get_token();
			if (token < TK_IDENT)
			{
				error("��ʶ������Ϊ�ؼ���");
			}
			else
			{
				get_token();
			}
		}
		else if (token == TK_OPENPA)
		{
			get_token();
			argument_expression_list();
			skip(TK_CLOSEPA);
		}
		else
		{
			break;
		}
	}
}

void primary_expression()
{
	if (token >= TK_IDENT)
	{
		get_token();
		return;
	}
	switch (token)
	{
	case TK_CINT:
	case TK_CSTR:
	case TK_CCHAR:
		get_token();
		break;
	case TK_OPENPA:
		get_token();
		expression();
		skip(TK_CLOSEPA);
		break;
	default:
		break;
	}
}

void argument_expression_list()
{
	assignment_expression();
	while (token == TK_COMMA)
	{
		get_token();
		assignment_expression();
	}
}