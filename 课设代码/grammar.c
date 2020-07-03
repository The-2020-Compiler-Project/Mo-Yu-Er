#include "scc.h"

/*翻译单元，开始翻译*/
void translation_unit()
{
	while (token != TK_EOF)
	{
		external_declaration(SC_GLOBAL);
	}
}

/*外部声明*/
void external_declaration(int l)
{
	if (!type_specifier())
	{
		expect("<类型区分符>");
	}
	if (token == TK_SEMICOLON)
	{
		get_token();
		return;
	}
	else
	{
		declarator();
		//这里必须看select集合才能确定
		if (token == TK_BEGIN)					//函数声明的情况
		{
			funcbody();
		}
		//这里必须看select集合才能确定
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

/*声明*/
void declaration()
{
	if (!type_specifier(token))
	{
		expect("类型区分符");
	}
	if (token != TK_SEMICOLON)
	{
		init_declarator_list();
	}
	skip(TK_SEMICOLON);
}

/*初值声明符表*/
void init_declarator_list()
{
	init_declarator();
	while (token != TK_SEMICOLON)
	{
		skip(TK_COMMA);
		init_declarator();
	}
}

/*初值声明符*/
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

/*类型区分符
包括int，char，short，void，结构体*/
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

/*结构区分符*/
void struct_specifier()
{
	skip(KW_STRUCT);
	if (token < TK_IDENT)
	{
		error("关键字不能作为结构体名");
	}
	get_token();
	if (token == TK_BEGIN)
	{
		get_token();
		struct_declaration_list();
		skip(TK_END);
	}
}

/*结构声明符表*/
void struct_declaration_list()
{
	while (token != TK_END)
	{
		struct_declaration();
	}
}

/*结构声明*/
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


/*函数调用的情况
可以为空*/
void function_calling_convention()
{
	if (token == KW_CDECL || token == KW_STDCALL)
	{
		get_token();
		return;
	}
}

/*结构成员对齐*/
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
			expect("整形常量");
		}
		skip(TK_CLOSEPA);
	}
}

/*声明符*/
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

/*直接声明符*/
void direct_declarator()
{
	if (token >= TK_IDENT)
	{
		get_token();
		direct_declarator_postfix();
	}
	else
	{
		expect("标识符");
	}
}

/*直接声明符后缀*/
void direct_declarator_postfix()
{
	if (token == TK_OPENBR)				//[]和[cint]情况
	{
		get_token();
		if (token == TK_CINT)
		{
			get_token();
		}
		skip(TK_CLOSEBR);
	}
	else if (token == TK_OPENPA)		//()和(形参)情况
	{
		get_token();
		if (is_type_specifier(token))
		{
			parameter_type_list();
		}
		skip(TK_CLOSEPA);
	}
}

/*形参类型表*/
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

/*函数体*/
void funcbody()
{
	compound_statement();
}

void initializer()
{
	assignment_expression();
}

/*语句
包括复合，选择，循环，跳转，表达式语句*/
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

/*复合语句*/
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

/*判断是否为类型区分符*/
int is_type_specifier(int v)
{
	if (v == KW_INT || v == KW_CHAR || v == KW_SHORT || v == KW_VOID || v==KW_STRUCT)
	{
		return 1;
	}
	return 0;
}

/*表达式语句*/
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

/*选择语句*/
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

/*循环语句*/
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

/*continue语句*/
void continue_statement()
{
	skip(KW_CONTINUE);
	skip(TK_SEMICOLON);
}

/*break语句*/
void break_statement()
{
	skip(KW_BREAK);
	skip(TK_SEMICOLON);
}

/*return语句*/
void return_statement()
{
	skip(KW_RETURN);
	if (token != TK_SEMICOLON)
	{
		expression();
	}
	skip(TK_SEMICOLON);
}

/*表达式*/
void expression()
{
	assignment_expression();
	if (token == TK_COMMA)
	{
		get_token();
		assignment_expression();
	}
}

/*赋值表达式*/
void assignment_expression()
{
	equality_expression();
	if (token == TK_ASSIGN)
	{
		get_token();
		assignment_expression();
	}
}

/*相等和不等表达式*/
void equality_expression()
{
	relational_expression();
	while (token == TK_EQ || token == TK_NEQ)
	{
		get_token();
		relational_expression();
	}
}

/*关系表达式*/
void relational_expression()
{
	additive_expression();
	while (token == TK_LT || token == TK_LEQ || token == TK_GT || token == TK_GEQ)
	{
		get_token();
		additive_expression();
	}
}

/*加减类表达式*/
void additive_expression()
{
	multiplicative_expression();
	while (token == TK_PLUS || token == TK_MINUS)
	{
		get_token();
		multiplicative_expression();
	}
}

/*乘除类表达式*/
void multiplicative_expression()
{
	unary_expression();
	while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD)
	{
		get_token();
		unary_expression();
	}
}

/*一元表达式*/
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

/*sizeof表达式*/
void sizeof_expression()
{
	skip(KW_SIZEOF);
	skip(TK_OPENPA);
	type_specifier();
	skip(TK_CLOSEPA);
}

/*后缀表达式*/
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
				error("标识符不能为关键字");
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

/*初等表达式*/
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

/*实参表达式表*/
void argument_expression_list()
{
	assignment_expression();
	while (token == TK_COMMA)
	{
		get_token();
		assignment_expression();
	}
}