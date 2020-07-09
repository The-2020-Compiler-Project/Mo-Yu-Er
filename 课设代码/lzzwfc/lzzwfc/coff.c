#include "coff.h"
#include "lexial.h"
#include "gencode.h"
int nsec_images;

void init_coff()
{
	nsec_images = 0;
	array_init(&sections, 8);	//八个基本节
	//可作为代码执行|包含可执行代码
	sec_text = new_section(".text", IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE);
	//可读|可写|包含已初始的数据
	sec_data = new_section(".data", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_INITIALIZED_DATA);
	//可读|包含初始化数据
	sec_rdata = new_section(".rdata", IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA);
	//可读|可写|包含初始化数据，是导入表节
	sec_idata = new_section(".idata", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_INITIALIZED_DATA);
	//可读|可写|未初始化
	sec_bss = new_section(".bss", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_UNINITIALIZED_DATA);
	//不会形成映像文件的一部分|可读
	sec_rel = new_section(".rel", IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ);
	
	//符号表节和字符串节在同一节
	//不会形成映像文件的一部分|可读
	sec_symtab = new_coffsym_section(".symtab", IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ, ".strtab");
	sec_dynsymtab = new_coffsym_section(".dynsym",IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ, ".dynstr");
	//空表项，每个表项都是20字节
	add_coffsym(sec_symtab, "", 0, 0, 0, IMAGE_SYM_CLASS_NULL);
	//IMAGE_SYM_CLASS_STATIC：符号在节中的偏移，若value=0，表示节名
	//这里表示的就是.data节名
	add_coffsym(sec_symtab, ".data", 0, sec_data->idx, 0, IMAGE_SYM_CLASS_STATIC);
	add_coffsym(sec_symtab, ".bss", 0, sec_bss->idx, 0, IMAGE_SYM_CLASS_STATIC);
	add_coffsym(sec_symtab, ".rdata", 0, sec_rdata->idx, 0, IMAGE_SYM_CLASS_STATIC);
	add_coffsym(sec_dynsymtab, "", 0, 0, 0, IMAGE_SYM_CLASS_NULL);
}

Section* new_section(char* name, int Characteristics)
{
	Section* sec = mallocz(sizeof(Section));	//这里用mallocz更好，如果用malloc，还要初始化
	int init_size = 8;	//初始分配的节的大小，最后成为SizeOfRawData
	sec->data = mallocz(sizeof(char) * init_size);	//init_size个字节的0
	sec->idx = sections.count + 1;	//节序号从1开始，我也想从0开始，为了跟官方做法一样吧
	strcpy(sec->sh.Name, name);
	sec->sh.Characteristics = Characteristics;
	sec->sec_size = init_size;
	array_push(&sections, sec);
	/* IMAGE_SCN_LNK_REMOVE只有一位为1，因此可以当做标志位来用 */
	if ((Characteristics & IMAGE_SCN_LNK_REMOVE) == 0) //标志位无效，可以形成映像文件的一部分
	{
		nsec_images++;
	}
	return sec;
}

//创建符号表节
Section* new_coffsym_section(char* symtab_name, int Characteristics, char* strtab_name)
{
	Section* sec = new_section(symtab_name, Characteristics);
	sec->link = new_section(strtab_name, Characteristics);
	sec->hashtable = mallocz(sizeof(int) * MAXKEY);
	return sec;
}

int add_coffsym(Section* symtab, char* name, int val, int sec_index, short type, char StorageClass)
{
	CoffSym* cfsym;
	char* csname;
	int cs = search_coffsym(symtab, name);
	int keyno;
	int* hashtab = symtab->hashtable;
	if (cs == 0)	//说明该符号还没有入符号表
	{
		cfsym = section_ptr_add(symtab, sizeof(CoffSym));	//预留一个记录的大小
		csname = add_coffstr(symtab->link, name);
		cfsym->Name = csname - symtab->link->data;	//相对于字符串节的数据偏移地址
		cfsym->Value = val;
		cfsym->Section = sec_index;
		cfsym->Type = type;
		cfsym->StorageClass = StorageClass;
		keyno = ElfHash(name);
		cfsym->Next = hashtab[keyno];	//指向上一个冲突
		cs = cfsym - (CoffSym*)symtab->data;	//记录在符号表中的偏移地址
		hashtab[keyno] = cs;	//指向最新的冲突

	}
	return cs;
}

//在符号表中查找某个符号是否存在
int search_coffsym(Section* symtab, char* sym_name)
{
	CoffSym* cfsym;
	char* name;
	int keyno = ElfHash(sym_name); //获取哈希表键值
	int cs = symtab->hashtable[keyno];	//cs的值代表着该符号在符号表中的偏移位置，也就是第几个纪录
	while (cs)
	{
		cfsym = ((CoffSym*)symtab->data) + cs;	//+1代表跳过sizeof(CoffSym)大小的内存，那么+cs就是跳过cs个纪录
		name = symtab->link->data + cfsym->Name; //从字符串节查找
		if (!strcmp(sym_name, name))	//找到了该符号
		{
			return cs;	//返回是第几个纪录
		}
		cs = cfsym->Next;	//哈希冲突了，找下一个可能的纪录
	}

	return cs;
}

//为节数据留空间，也就是最开始的size大小不能动
void* section_ptr_add(Section* sec, int size)
{
	int offset = sec->data_offset;
	sec->data_offset = offset + size;
	if (sec->data_offset > sec->sec_size)
	{
		realloc_section(sec, sec->data_offset); //sec->data_offset为当前偏移地址，也是当前的容量
	}
	return sec->data + offset; //返回原来的地址，当前偏移量已经更改，但数据还没保存
}

void realloc_section(Section* sec, int new_size)
{
	char* data;
	int size = sec->sec_size;
	//和之前一样，翻倍size，避免频繁调用realloc
	while (size < new_size)
	{
		size = size * 2;
	}
	data = realloc(sec->data, size);
	if (!data)
	{
		error("内存分配失败");
	}
	memset(data + sec->sec_size, 0, size - sec->sec_size);	//原来的data保持不变，新申请的内存全部为0
	sec->data = data;
	sec->sec_size = size;
}

//向字符串节添加符号
char* add_coffstr(Section* sec, char* name)
{
	int len = strlen(name);
	char* pstr = section_ptr_add(sec, len + 1); //\0也要写入内存
	memcpy(pstr, name, len);	//这里不用len+1是因为分配的是len+1大小的内存，但是最后一个字节没用为0
	return pstr;				//就相当于添加了\0
}

void add_coffreloc(Section* sec, Symbol* sym, int offset, char type)
{
	int cfsym;
	char* name;
	if (!sym->c)
		add_update_coffsym(sym, 0, IMAGE_SYM_UNDEFINED, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL);
	name = ((TkWord*)tktable.data[sym->v])->word;
	cfsym = search_coffsym(sec_symtab, name);
	direct_add_coffreloc(offset, cfsym, sec->idx, type);
}

void add_update_coffsym(Symbol* s, int val, int sec_index,
	short type, char StorageClass)
{
	char* name;
	CoffSym* cfsym;
	if (!s->c)
	{
		name = ((TkWord*)tktable.data[s->v])->word;
		s->c = add_coffsym(sec_symtab, name, val, sec_index, type, StorageClass);
	}
	else //函数先声明后定义
	{
		cfsym = &((CoffSym*)sec_symtab->data)[s->c];
		cfsym->Value = val;
		cfsym->Section = sec_index;
	}
}

void direct_add_coffreloc(int offset, int cfsym, char section, char type)
{
	CoffReloc* rel = section_ptr_add(sec_rel, sizeof(CoffReloc));
	rel->offset = offset;
	rel->cfsym = cfsym;
	rel->section = section;
	rel->type = type;
}

//从当前位置到new_pos补0
void fpad(FILE* fp, int new_pos)
{
	int curpos = ftell(fp); //ftell返回当前文件位置，是当前的字节
	while (++curpos <= new_pos)
		fputc(0, fp);
}

void write_obj(char* name)
{
	int file_offset;
	FILE* fout = fopen(name, "wb");
	int i, sh_size, nsec_obj = 0;
	IMAGE_FILE_HEADER* fh;
	nsec_obj = sections.count - 2;	//要写入到obj的节的数目，这里以后还要改，还得删去动态链接节（已改）
	sh_size = sizeof(IMAGE_SECTION_HEADER); //40
	file_offset = sizeof(IMAGE_FILE_HEADER) + nsec_obj * sh_size; //20+8*40=340;
	fpad(fout, file_offset);	//先写入COFF文件头和节头表，全为0
	fh = mallocz(sizeof(IMAGE_FILE_HEADER));
	for (i = 0; i < nsec_obj; i++)
	{
		Section* sec = (Section*)sections.data[i];
		if (sec->data == NULL) continue;
		fwrite(sec->data, 1, sec->data_offset, fout);	//写入节数据，sec->data_offset个字节
		sec->sh.PointerToRawData = file_offset;
		sec->sh.SizeOfRawData = sec->data_offset;
		file_offset += sec->data_offset;
	}
	fseek(fout, 0, SEEK_SET);	//文件指针指向文件开始
	fh->Machine = IMAGE_FILE_MACHINE_I386;
	fh->NumberOfSections = nsec_obj;
	fh->PointerToSymbolTable = sec_symtab->sh.PointerToRawData;	//指向符号表的指针
	fh->NumberOfSymbols = sec_symtab->sh.SizeOfRawData / sizeof(CoffSym); //有几个符号记录
	fwrite(fh, 1, sizeof(IMAGE_FILE_HEADER), fout); // 将COFF文件头真正写入，之前因为节数据未生成，部分COFF文件头不能确定
	for (i = 0; i < nsec_obj; i++)
	{
		Section* sec = (Section*)sections.data[i];
		/* 因为之前节数据还没生成，所以必须等节数据生成后才能确定节头表的指向节的起始地址和节大小 */
		fwrite(sec->sh.Name, 1, sh_size, fout); //将节头表的内容真正写入，由于结构体是连续存储的单元
												//因此只需要给第一个成员变量的地址就行了
	}

	free(fh);
	fclose(fout);
}
