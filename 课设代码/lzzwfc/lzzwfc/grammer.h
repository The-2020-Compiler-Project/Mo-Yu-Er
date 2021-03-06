#pragma once
#include "symbol.h"
extern int delay_v;
void translation_unit();
void external_declaration();
void init_declarator(Type* type, int scope);
int type_specifier(Type* type);
void declaration(int scope);
void init_declarator_list(Type* type, int scope);
void initializer();
void struct_specifier();
void struct_declaration_list();
void struct_declaration();
void declarator(Type* type, int scope);
void function_calling_convention();
void struct_member_alignment();
//scope作用域，全局还是局部
void direct_declarator(Type* type, int scope);
void direct_declarator_postfix(Type* type);
int parameter_type_list();
void funcbody();
void statement();
void compound_statement();
int is_type_specifier(int v);
void statement();
void expression_statement();
void for_statement();
void break_statement();
void continue_statement();
void if_statement();
void return_statement();
void assignment_expression();
void expression();
void equality_expression();
void relational_expression();
void additive_expression();
void multiplicative_expression();
void unary_expression();
void argument_expression_list();
void postfix_expression();
void primary_expression();
void sizeof_expression();