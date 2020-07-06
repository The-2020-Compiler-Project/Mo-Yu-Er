#include "error.h"
#include "lzzwfc.h"
#include "lexial.h"
#include <stdarg.h>
char* filename;
int line_num;
//总错误处理，用到了可变参数表
void handle_exception(int state, int level, char* fmt, va_list ap)
{
	char buf[1024];
	vsprintf(buf, fmt, ap);
	if (state == STAGE_COMPILE)
	{
		if (level == LEVEL_WARNING)
		{
			printf("%s的第%d行编译警告:%s", filename, line_num, buf);
		}
		else if (level == LEVEL_ERROR)
		{
			printf("%s的第%d行编译错误:%s", filename, line_num, buf);
			exit(-1);
		}
	}
	else
	{
		printf("链接错误:%s", buf);
	}
}

void error(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
	va_end(ap);
}

void expect(char* msg)
{
	error("缺少%s", msg);
}

void skip(int v)
{
	if (token != v)
	{
		error("应该为%s", get_tkstr(v));
	}
	get_token();
}
