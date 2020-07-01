#include "lexial.h"
#include <stdio.h>
#include <string.h>
DynString tkstr;
DynString outstr;
char ch;
int token;
FILE* fin;
int line_num = 0;
unsigned int ElfHash(char* str)
{
    unsigned int hash = 0;
    unsigned int x = 0;

    while (*str)
    {
        hash = (hash << 4) + (*str++);//hash左移4位，把当前字符ASCII存入hash低四位。
        if ((x = hash & 0xF0000000L) != 0)
        {
            //如果最高的四位不为0，则说明字符多余7个，现在正在存第7个字符，如果不处理，再加下一个字符时，第一个字符会被移出，因此要有如下处理。
            //该处理，如果最高位为0，就会仅仅影响5-8位，否则会影响5-31位，因为C语言使用的算数移位
            //因为1-4位刚刚存储了新加入到字符，所以不能>>28
            hash ^= (x >> 24);
            //上面这行代码并不会对X有影响，本身X和hash的高4位相同，下面这行代码&~即对28-31(高4位)位清零。
            hash &= ~x;
        }
    }
    //返回一个符号位为0的数，即丢弃最高位，以免函数外产生影响。(我们可以考虑，如果只有字符，符号位不可能为负)
    return (hash & 0x7FFFFFFF) % MAXKEY;
}

TkWord* TkWord_direct_insert(TkWord* pTK)
{
    int hash;
    array_push(&tktable, pTK);
    hash = ElfHash(pTK->word);       //elfhash不同key也可能映射到同一个地址
    pTK->next = tk_hashtable[hash];  // 链地址法解决冲突
    tk_hashtable[hash] = pTK;
    return pTK;
}

TkWord* TkWord_search(char* word, int key)
{
    TkWord* tp = NULL;
    TkWord* p;
    for (p = tk_hashtable[key]; p != NULL; p = p->next)
    {
        if (!strcmp(p->word, word))
        {
            tp = p;
        }
    }
    return tp;
}

TkWord* TkWord_insert(char* word)
{
    TkWord* tp;
    char* s;
    char* end;
    int len;
    int hash = ElfHash(word);
    tp = TkWord_search(word, hash);
    if (!tp)
    {
        len = strlen(word);
        tp = (TkWord*)mallocz(sizeof(TkWord) + len + 1);
        tp->next = tk_hashtable[hash];
        tk_hashtable[hash] = tp;
        array_push(&tktable, tp);
        s = (char*)tp + sizeof(TkWord);
        tp->word = (char*)s;
        tp->tkcode = tktable.count - 1;
        for (end = word + len; word < end; )
        {
            *s++ = *word++;
        }
        *s = '\0';
        tp->sym_identifier = NULL;
        tp->sym_struct = NULL;
    }
    return tp;
}

void* mallocz(int size)
{
    void* p;
    p = malloc(size);
    if (!p && size)
    {
        //错误处理
    }
    memset(p, 0, size);
    return p;
}

void init_lexial()
{
    TkWord* tp;
    static TkWord keywords[] = {
    {TK_PLUS,		NULL,	  "+",	NULL,	NULL},
    {TK_MINUS,		NULL,	  "-",	NULL,	NULL},
    {TK_STAR,		NULL,	  "*",	NULL,	NULL},
    {TK_DIVIDE,		NULL,	  "/",	NULL,	NULL},
    {TK_MOD,		NULL,	  "%",	NULL,	NULL},
    {TK_EQ,			NULL,	  "==",	NULL,	NULL},
    {TK_NEQ,		NULL,	  "!=",	NULL,	NULL},
    {TK_LT,			NULL,	  "<",	NULL,	NULL},
    {TK_LEQ,		NULL,	  "<=",	NULL,	NULL},
    {TK_GT,			NULL,	  ">",	NULL,	NULL},
    {TK_GEQ,		NULL,	  ">=",	NULL,	NULL},
    {TK_ASSIGN,		NULL,	  "=",	NULL,	NULL},
    {TK_POINTSTO,	NULL,	  "->",	NULL,	NULL},
    {TK_DOT,		NULL,	  ".",	NULL,	NULL},
    {TK_AND,		NULL,	  "&",	NULL,	NULL},
    {TK_OPENPA,		NULL,	  "(",	NULL,	NULL},
    {TK_CLOSEPA,	NULL,	  ")",	NULL,	NULL},
    {TK_OPENBR,		NULL,	  "[",	NULL,	NULL},
    {TK_CLOSEBR,	NULL,	  "]",	NULL,	NULL},
    {TK_BEGIN,		NULL,	  "{",	NULL,	NULL},
    {TK_END,		NULL,	  "}",	NULL,	NULL},
    {TK_SEMICOLON,	NULL,	  ";",	NULL,	NULL},
    {TK_COMMA,		NULL,	  ",",	NULL,	NULL},
    {TK_ELLIPSIS,	NULL,	"...",	NULL,	NULL},
    {TK_EOF,		NULL,	 "End_Of_File",	NULL,	NULL},

    {TK_CINT,		NULL,	 	"整型常量",	NULL,	NULL},
    {TK_CCHAR,		NULL,		"字符常量",	NULL,	NULL},
    {TK_CSTR,		NULL,		"字符串常量",	NULL,	NULL},

    {KW_CHAR,		NULL,		"char",	NULL,	NULL},
    {KW_SHORT,		NULL,		"short",	NULL,	NULL},
    {KW_INT,		NULL,		"int",	NULL,	NULL},
    {KW_VOID,		NULL,		"void",	NULL,	NULL},
    {KW_STRUCT,		NULL,		"struct",	NULL,	NULL},

    {KW_IF,			NULL,		"if"	,	NULL,	NULL},
    {KW_ELSE,		NULL,		"else",	NULL,	NULL},
    {KW_FOR,		NULL,		"for",	NULL,	NULL},
    {KW_CONTINUE,	NULL,		"continue",	NULL,	NULL},
    {KW_BREAK,		NULL,		"break",	NULL,	NULL},
    {KW_RETURN,		NULL,		"return",	NULL,	NULL},
    {KW_SIZEOF,		NULL,		"sizeof",	NULL,	NULL},
    {KW_ALIGN,		NULL,		"__align",	NULL,	NULL},
    {KW_CDECL,		NULL,		"__cdecl",	NULL,	NULL},
    {KW_STDCALL,	NULL,		"__stdcall",	NULL,	NULL},
    {0,				NULL,	NULL,	NULL,		NULL}
    };
    array_init(&tktable, 8);
    for (tp = &keywords[0]; tp->word != NULL; tp++)
    {
        TkWord_direct_insert(tp);
    }
}

void get_ch()
{
    ch = getc(fin);
}

void skip_white_space()
{
    while (ch == ' ' || ch == '\t' || ch == '\r')
    {
        if (ch == '\r')
        {
            get_ch();
            if (ch != '\n')
            {
                return;
            }
            line_num++;
        }
        //printf("%c", ch);
        get_ch();
    }
}

void preprocess()
{
    while (1)
    {
        if (ch == ' ' || ch == '\t' || ch == '\r')
        {
            skip_white_space();
        }
        else if (ch == '/')
        {
            get_ch();
            if (ch == '*')
            {
                //处理注释
            }
            else
            {
                ungetc(ch, fin);
            }
        }
        else
        {
            break;
        }
    }
}

void get_token()
{
    preprocess();
    switch (ch)
    {
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '_':
    {
        TkWord* tp;
        parse_identifier();
        tp = TkWord_insert(tkstr.str);
        token = tp->tkcode;
        break;
    }
    case '0': case '1': case '2': case '3': case '4': 
    case '5': case '6': case '7': case '8': case '9':
        parse_num();
        token = TK_CINT;
        break;
    case '\'':
        parse_cchar();
        token = TK_CCHAR;
        break;
    case '\"':
        parse_cstr();
        token = TK_CSTR;
        break;
    case '(':
        token = TK_OPENPA;
        get_ch();
        break;
    case ')':
        token = TK_CLOSEPA;
        get_ch();
        break;
    case '{':
        token = TK_BEGIN;
        get_ch();
        break;
    case '}':
        token = TK_END;
        get_ch();
        break;
    case ';':
        token = TK_SEMICOLON;
        get_ch();
        break;
    case EOF:
        token = TK_EOF;
        break;
    default:
        get_ch();
        break;
    }
}

int is_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

void parse_identifier()
{
    str_reset(&tkstr);
    str_cat(&tkstr, ch);
    get_ch();
    while (is_digit(ch) || is_letter(ch))
    {
        str_cat(&tkstr, ch);
        get_ch();
    }
    str_cat(&tkstr, '\0');
}

void parse_num()
{
    str_reset(&tkstr);
    str_reset(&outstr);
    while (is_digit(ch))
    {
        str_cat(&outstr, ch);
        str_cat(&tkstr, ch);
        get_ch();
    }
    //ch始终指向下一个char
    str_cat(&tkstr, '\0');
    str_cat(&outstr, '\0');
}

void parse_cchar()
{
    char c;
    str_reset(&tkstr);
    str_reset(&outstr);
    getch();
    if (ch == '\\')
    {
        getch();
        switch (ch)
        {
        case '0':
            c = '\0';
            break;
        case 'a':
            c = '\a';
            break;
        case 'b':
            c = '\b';
            break;
        case 't':
            c = '\t';
            break;
        case 'n':
            c = '\n';
            break;
        case 'v':
            c = '\v';
            break;
        case 'f':
            c = '\f';
            break;
        case 'r':
            c = '\r';
            break;
        case '\'':
            c = '\'';
            break;
        case '\"':
            c = '\"';
            break;
        case '\\':
            c = '\\';
            break;
        default:
            break;
        }
    }
    else
    {
        c = ch;
    }
    str_cat(&tkstr, c);
    get_ch();
    str_cat(&tkstr, ch);
    str_cat(&tkstr, '\0');
    get_ch();
}

void parse_cstr()
{
    char c;
    str_reset(&tkstr);
    str_reset(&outstr);
    str_cat(&tkstr, ch);
    str_cat(&outstr, ch);
    get_ch();
    while (ch != '\"')
    {
        if (ch == '\\')
        {
            str_cat(&outstr, ch); //为了能不转义直接输出转义字符
            get_ch();
            switch (ch)
            {
            case '0':
                c = '\0';
                break;
            case 'a':
                c = '\a';
                break;
            case 'b':
                c = '\b'; 
                break;
            case 't':
                c = '\t';
                break;
            case 'n':
                c = '\n';
                break;
            case 'v':
                c = '\v';
                break;
            case 'f':
                c = '\f';
                break;
            case 'r':
                c = '\r';
                break;
            case '\'':
                c = '\'';
                break;
            case '\"':
                c = '\"';
                break;
            case '\\':
                c = '\\';
                break;
            default:
                break;
            }
        }
        else
        {
            c = ch;
        }
        str_cat(&tkstr, c);
        str_cat(&outstr, ch);
        get_ch();
    }
    str_cat(&tkstr, ch);
    str_cat(&outstr, ch);
    str_cat(&tkstr, '\0');
    str_cat(&outstr, '\0');
    get_ch();
}

char* get_tkstr(int token)
{
    //token超出单词表,此token不存在
    if (token > tktable.count)
    {
        return NULL;
    }
    //是整形常量或字符常量或字符串常量
    else if (token >= TK_CINT && token <= TK_CSTR)
    {
        return outstr.str;
    }
    //进入了单词表,那么token对应数组的下标
    else
    {
        return ((TkWord*)tktable.data[token])->word;
    }
}


void test_lexial()
{
    do
    {
        get_token();
        printf("%d %s\n", token, get_tkstr(token));
    } while (token != TK_EOF);
}
