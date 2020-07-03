#include "scc.h"

/*���뵥Ԫ����ʼ����*/
void translation_unit()
{
	while (token != TK_EOF)
	{
		external_declaration(SC_GLOBAL);
	}
}

/*�ⲿ����*/
void external_declaration(int l)
{
	if (!type_specifier())
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
		declarator();
		//������뿴select���ϲ���ȷ��
		if (token == TK_BEGIN)					//�������������
		{
			funcbody();
		}
		//������뿴select���ϲ���ȷ��
		else if (token == TK_ASSIGN)
		{
			get_token();
			initializer();
		}
		else
		{
			while (token == TK_COMMA)
			{
				get_token();
				init_declarator();
			}
		}
	}
}

/*����*/
void declaration()
{
	if (!type_specifier(token))
	{
		expect("�������ַ�");
	}
	if (token != TK_SEMICOLON)
	{
		init_declarator_list();
	}
	skip(TK_SEMICOLON);
}

/*��ֵ��������*/
void init_declarator_list()
{
	init_declarator();
	while (token != TK_SEMICOLON)
	{
		skip(TK_COMMA);
		init_declarator();
	}
}

/*��ֵ������*/
void init_declarator()
{
	declarator();
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

/*�������ַ�
����int��char��short��void���ṹ��*/
int type_specifier()
{
	int is_type = 0;
	if (token == KW_INT || token == KW_CHAR || token == KW_SHORT || token == KW_VOID)
	{
		is_type = 1;
		get_token();
	}
	else if (token == KW_STRUCT)
	{
		is_type = 1;
		struct_specifier();
	}
	return is_type;
}

/*�ṹ���ַ�*/
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

/*�ṹ��������*/
void struct_declaration_list()
{
	while (token != TK_END)
	{
		struct_declaration();
	}
}

/*�ṹ����*/
void struct_declaration()
{
	if (is_type_specifier(token))
	{
		type_specifier();
	}
	declarator();
	while (token != TK_SEMICOLON)
	{
		skip(TK_COMMA);
		declarator();
	}
	skip(TK_SEMICOLON);
}


/*�������õ����
����Ϊ��*/
void function_calling_convention()
{
	if (token == KW_CDECL || token == KW_STDCALL)
	{
		get_token();
		return;
	}
}

/*�ṹ��Ա����*/
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

/*������*/
void declarator()
{
	while (token == TK_STAR)
	{
		get_token();
	}
	function_calling_convention();
	struct_member_alignment();
	direct_declarator();
}

/*ֱ��������*/
void direct_declarator()
{
	if (token >= TK_IDENT)
	{
		get_token();
		direct_declarator_postfix();
	}
	else
	{
		expect("��ʶ��");
	}
}

/*ֱ����������׺*/
void direct_declarator_postfix()
{
	if (token == TK_OPENBR)				//[]��[cint]���
	{
		get_token();
		if (token == TK_CINT)
		{
			get_token();
		}
		skip(TK_CLOSEBR);
	}
	else if (token == TK_OPENPA)		//()��(�β�)���
	{
		get_token();
		if (is_type_specifier(token))
		{
			parameter_type_list();
		}
		skip(TK_CLOSEPA);
	}
}

/*�β����ͱ�*/
void parameter_type_list()
{
	type_specifier();
	if (token == TK_STAR || token == KW_CDECL || token == KW_STDCALL)
	{
		declarator();
	}
	while (token == TK_COMMA)
	{
		get_token();
		type_specifier();
		if (token == TK_STAR || token == KW_CDECL || token == KW_STDCALL)
		{
			declarator();
		}
	}
	if (token == TK_COMMA)
	{
		get_token();
		skip(TK_ELLIPSIS);
	}
}

/*������*/
void funcbody()
{
	compound_statement();
}

void initializer()
{
	assignment_expression();
}

/*���
�������ϣ�ѡ��ѭ������ת�����ʽ���*/
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

/*�������*/
void compound_statement()
{
	skip(TK_BEGIN);
	while (is_type_specifier(token))
	{
		declaration();
	}
	while (token != TK_END)
	{
		statement();
	}
	skip(TK_END);
}

/*�ж��Ƿ�Ϊ�������ַ�*/
int is_type_specifier(int v)
{
	if (v == KW_INT || v == KW_CHAR || v == KW_SHORT || v == KW_VOID || v==KW_STRUCT)
	{
		return 1;
	}
	return 0;
}

/*���ʽ���*/
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

/*ѡ�����*/
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

/*ѭ�����*/
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

/*continue���*/
void continue_statement()
{
	skip(KW_CONTINUE);
	skip(TK_SEMICOLON);
}

/*break���*/
void break_statement()
{
	skip(KW_BREAK);
	skip(TK_SEMICOLON);
}

/*return���*/
void return_statement()
{
	skip(KW_RETURN);
	if (token != TK_SEMICOLON)
	{
		expression();
	}
	skip(TK_SEMICOLON);
}

/*���ʽ*/
void expression()
{
	assignment_expression();
	if (token == TK_COMMA)
	{
		get_token();
		assignment_expression();
	}
}

/*��ֵ���ʽ*/
void assignment_expression()
{
	equality_expression();
	if (token == TK_ASSIGN)
	{
		get_token();
		assignment_expression();
	}
}

/*��ȺͲ��ȱ��ʽ*/
void equality_expression()
{
	relational_expression();
	while (token == TK_EQ || token == TK_NEQ)
	{
		get_token();
		relational_expression();
	}
}

/*��ϵ���ʽ*/
void relational_expression()
{
	additive_expression();
	while (token == TK_LT || token == TK_LEQ || token == TK_GT || token == TK_GEQ)
	{
		get_token();
		additive_expression();
	}
}

/*�Ӽ�����ʽ*/
void additive_expression()
{
	multiplicative_expression();
	while (token == TK_PLUS || token == TK_MINUS)
	{
		get_token();
		multiplicative_expression();
	}
}

/*�˳�����ʽ*/
void multiplicative_expression()
{
	unary_expression();
	while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD)
	{
		get_token();
		unary_expression();
	}
}

/*һԪ���ʽ*/
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

/*sizeof���ʽ*/
void sizeof_expression()
{
	skip(KW_SIZEOF);
	skip(TK_OPENPA);
	type_specifier();
	skip(TK_CLOSEPA);
}

/*��׺���ʽ*/
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

/*���ȱ��ʽ*/
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

/*ʵ�α��ʽ��*/
void argument_expression_list()
{
	assignment_expression();
	while (token == TK_COMMA)
	{
		get_token();
		assignment_expression();
	}
}