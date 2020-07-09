//���ɴ���ڵ�����
#pragma once
#include "operand.h"
#define REG_IRET REG_EAX;	//EAX�Ĵ���Ϊ����ֵ�ļĴ���
extern int loc; //��������������ı����ܴ�С
extern int ind;	//��ǰ���ɵ��Ĵ����λ��
extern int func_back_ind;	//��������Ҫ�޸������������Ĵ�С
extern Symbol* sym_sec_rdata;	//ֻ�����Ž�
//�Ĵ�������
enum e_Register
{
	REG_EAX = 0,
	REG_ECX,
	REG_EDX,
	REG_EBX,
	REG_ESP,
	REG_EBP,
	REG_ESI,
	REG_EDI,
	REG_ANY
};

enum e_AddrForm
{
	ADDR_OTHER,		//��һ�����������ڴ浥Ԫ
	ADDR_REG = 3	//�Ĵ������Ĵ�������Ӧreg_opcode��11
};
void gen_byte(char c);
void gen_dword(unsigned int c);
void gen_prefix(char pfx);
void gen_byte_opcode(char opcode);
void gen_two_byte_opcode(char first, char second);
void gen_modrm(int mode, int reg_opcode, int r_m, Symbol* sym, int c);
void gen_addr32(int r, Symbol* sym, int c);
void load(int r, Operand* opd);
void store(int r, Operand* opd);
void gen_invoke(int nb_args);
void gen_call();