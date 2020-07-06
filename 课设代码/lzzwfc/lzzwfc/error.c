#include "error.h"
#include "lzzwfc.h"
#include "lexial.h"
#include <stdarg.h>
char* filename;
int line_num;
//�ܴ������õ��˿ɱ������
void handle_exception(int state, int level, char* fmt, va_list ap)
{
	char buf[1024];
	vsprintf(buf, fmt, ap);
	if (state == STAGE_COMPILE)
	{
		if (level == LEVEL_WARNING)
		{
			printf("%s�ĵ�%d�б��뾯��:%s", filename, line_num, buf);
		}
		else if (level == LEVEL_ERROR)
		{
			printf("%s�ĵ�%d�б������:%s", filename, line_num, buf);
			exit(-1);
		}
	}
	else
	{
		printf("���Ӵ���:%s", buf);
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
	error("ȱ��%s", msg);
}

void skip(int v)
{
	if (token != v)
	{
		error("Ӧ��Ϊ%s", get_tkstr(v));
	}
	get_token();
}
