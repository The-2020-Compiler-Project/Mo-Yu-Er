#pragma once
#include <stdio.h>
/* 错误级别 */
enum e_ErrorLevel
{
	LEVEL_WARNING,
	LEVEL_ERROR,
};

/* 工作阶段 */
enum e_WorkStage
{
	STAGE_COMPILE,
	STAGE_LINK,
};
void handle_exception(int state, int level, char* fmt, va_list ap);
void warning(char* fmt, ...);
void error(char* fmt, ...);
void expect(char* msg);
void skip(int v);
void link_error(char* fmt, ...);