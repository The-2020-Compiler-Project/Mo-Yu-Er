//生成代码节的内容
#pragma once
#include "operand.h"
#define REG_IRET REG_EAX;	//EAX寄存器为返回值的寄存器
extern int loc; //符合语句中声明的变量总大小
extern int ind;	//当前生成到的代码节位置
extern int func_back_ind;	//代码回填，需要修改总声明变量的大小
extern Symbol* sym_sec_rdata;	//只读符号节
//寄存器编码
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
	ADDR_OTHER,		//有一个操作数是内存单元
	ADDR_REG = 3	//寄存器到寄存器，对应reg_opcode的11
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