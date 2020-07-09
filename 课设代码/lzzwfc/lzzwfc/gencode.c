#include "gencode.h"
#include "coff.h"
#include <stdio.h>
int ind = 0;
int func_back_ind;
Symbol* sym_sec_rdata;	//只读符号节
void gen_byte(char c)
{
	int i = ind + 1;
	if (ind >= sec_text->sec_size)
	{
		realloc_section(sec_text, i);
	}
	sec_text->data[ind] = c;	//向代码节添加一个字符
	ind = i;
}

void gen_dword(unsigned int c)
{
	gen_byte(c);	//字节截断，低位低地址
	gen_byte(c >> 8);
	gen_byte(c >> 16);
	gen_byte(c >> 24);

}

void gen_prefix(char pfx)
{
	gen_byte(pfx);
}

void gen_byte_opcode(char opcode)
{
	gen_byte(opcode);
}

void gen_two_byte_opcode(char first, char second)
{
	gen_byte(first);
	gen_byte(second);
}

void gen_modrm(int mode, int reg_opcode, int r_m, Symbol* sym, int c)
{
	//两个操作数都是寄存器直接寻址
	if (mode == 3) //mode=11b
	{
		gen_byte((mode << 6) | (reg_opcode << 3) | (r_m & SC_VALMASK));
	}
}

void gen_addr32(int r, Symbol* sym, int c)
{
	if (r & SC_SYM)
	{
		add_coffreloc(sec_text, sym, ind, IMAGE_REL_I386_DIR32);
	}
	gen_dword(c);
}

//将操作数opd存到寄存器r
//r为reg_opcode编码，参考intel手册的寄存器编码次序
void load(int r, Operand* opd)
{
	int v, ft, fv, fr;
	fr = opd->r;	//带有数据存储结构信息的ModR/M的r_m
	ft = opd->type.t; //操作数字节、字还是双字
	fv = opd->value; //操作数的值
	v = fr & SC_VALMASK;
	//如果为左值，比如a = b;翻译为LD R,b;ST R, a;对应第一条语句
	if (fr & SC_LVAL)
	{
		//操作数为字节
		if ((ft & T_BTYPE) == T_CHAR)
		{
			gen_two_byte_opcode(0x0f, 0xbe);
		}
		//操作数为字
		else if ((ft & T_BTYPE) == T_SHORT)
		{
			gen_two_byte_opcode(0x0f, 0xbf);
		}
		else
		{
			gen_byte_opcode(0x8b);
		}
		gen_modrm(ADDR_OTHER, r, fr, opd->sym, fv);

	}
	//不是左值
	else
	{
		//a=3或a="helloworld"，常量一定为全局变量
		if (v == SC_GLOBAL)
		{
			gen_byte_opcode(0xb8 + r);
			gen_addr32(fr, opd->sym, fv); //全局符号都需要重定位
		}
		//a=&b;
		else if (v == SC_LOCAL)
		{
			gen_byte_opcode(0x8d);
			gen_modrm(ADDR_OTHER, r, SC_LOCAL, opd->sym, fv);
		}
		//a=a>b
		else if (v == SC_CMP)
		{
		}
	}
}

//函数结尾代码
void gen_func_end()
{
	int cur_ind;
	gen_byte_opcode(0x8b);
	gen_modrm(ADDR_REG, REG_ESP, REG_EBP, NULL, 0);
	gen_byte_opcode(0x5d);
	gen_byte_opcode(0xc3);
	cur_ind = ind;
	ind = func_back_ind;
	gen_dword(loc); //回填
	ind = cur_ind;
}

//生成函数开始的汇编码
void gen_func_pre()
{
	gen_byte_opcode(0x50 + REG_EBP); //push ebp
	/* mov ebp, esp */
	gen_byte_opcode(0x89);
	gen_modrm(ADDR_REG, REG_ESP, REG_EBP, NULL, 0);

	gen_byte_opcode(0x81);
	gen_modrm(ADDR_REG, 5, 4, NULL, 0);

	func_back_ind = ind;
	gen_dword(0); //这里需要代码回填，填上声明变量的大小
}

void gen_invoke(int nb_args)
{
	for (int i = 0; i < nb_args; i++)
	{
		int r = allocate_reg(REG_ANY);
		load(r, &opstack[optop-1]);
		gen_byte_opcode(0x50 + r);
		pop_operand();
	}
	gen_call();
}

int allocate_reg(int r)
{
	/* 在通用寄存中找空闲寄存器 */
	for (int i = 0; i <= REG_EBX; i++)
	{
		if (r == REG_ANY || r == i)
		{
			int used = 0;
			for (int j = 0; j <= optop; j++)
			{
				if ((opstack[j].r & SC_VALMASK) == i)
				{
					used = 1;
					break;
				}
			}
			if (used == 0) return i;
		}
	}
}

void gen_call()
{
	add_coffreloc(sec_text, opstack[optop-1].sym, ind + 1, IMAGE_REL_I386_REL32);

	gen_byte_opcode(0xe8);
	gen_dword(opstack[optop-1].value - 4);
}