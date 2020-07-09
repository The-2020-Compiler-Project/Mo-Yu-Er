#pragma once
#define FUNC_PROLOG_SIZE 9
extern char* filename; //待编译文件名称
extern int line_num;    //源程序行数
extern int token;	//每一次的token
void init();