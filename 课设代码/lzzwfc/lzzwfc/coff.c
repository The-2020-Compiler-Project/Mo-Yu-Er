#include "coff.h"
#include "lexial.h"
#include "gencode.h"
int nsec_images;

void init_coff()
{
	nsec_images = 0;
	array_init(&sections, 8);	//�˸�������
	//����Ϊ����ִ��|������ִ�д���
	sec_text = new_section(".text", IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE);
	//�ɶ�|��д|�����ѳ�ʼ������
	sec_data = new_section(".data", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_INITIALIZED_DATA);
	//�ɶ�|������ʼ������
	sec_rdata = new_section(".rdata", IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA);
	//�ɶ�|��д|������ʼ�����ݣ��ǵ�����
	sec_idata = new_section(".idata", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_INITIALIZED_DATA);
	//�ɶ�|��д|δ��ʼ��
	sec_bss = new_section(".bss", IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_UNINITIALIZED_DATA);
	//�����γ�ӳ���ļ���һ����|�ɶ�
	sec_rel = new_section(".rel", IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ);
	
	//���ű�ں��ַ�������ͬһ��
	//�����γ�ӳ���ļ���һ����|�ɶ�
	sec_symtab = new_coffsym_section(".symtab", IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ, ".strtab");
	sec_dynsymtab = new_coffsym_section(".dynsym",IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ, ".dynstr");
	//�ձ��ÿ�������20�ֽ�
	add_coffsym(sec_symtab, "", 0, 0, 0, IMAGE_SYM_CLASS_NULL);
	//IMAGE_SYM_CLASS_STATIC�������ڽ��е�ƫ�ƣ���value=0����ʾ����
	//�����ʾ�ľ���.data����
	add_coffsym(sec_symtab, ".data", 0, sec_data->idx, 0, IMAGE_SYM_CLASS_STATIC);
	add_coffsym(sec_symtab, ".bss", 0, sec_bss->idx, 0, IMAGE_SYM_CLASS_STATIC);
	add_coffsym(sec_symtab, ".rdata", 0, sec_rdata->idx, 0, IMAGE_SYM_CLASS_STATIC);
	add_coffsym(sec_dynsymtab, "", 0, 0, 0, IMAGE_SYM_CLASS_NULL);
}

Section* new_section(char* name, int Characteristics)
{
	Section* sec = mallocz(sizeof(Section));	//������mallocz���ã������malloc����Ҫ��ʼ��
	int init_size = 8;	//��ʼ����ĽڵĴ�С������ΪSizeOfRawData
	sec->data = mallocz(sizeof(char) * init_size);	//init_size���ֽڵ�0
	sec->idx = sections.count + 1;	//����Ŵ�1��ʼ����Ҳ���0��ʼ��Ϊ�˸��ٷ�����һ����
	strcpy(sec->sh.Name, name);
	sec->sh.Characteristics = Characteristics;
	sec->sec_size = init_size;
	array_push(&sections, sec);
	/* IMAGE_SCN_LNK_REMOVEֻ��һλΪ1����˿��Ե�����־λ���� */
	if ((Characteristics & IMAGE_SCN_LNK_REMOVE) == 0) //��־λ��Ч�������γ�ӳ���ļ���һ����
	{
		nsec_images++;
	}
	return sec;
}

//�������ű��
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
	if (cs == 0)	//˵���÷��Ż�û������ű�
	{
		cfsym = section_ptr_add(symtab, sizeof(CoffSym));	//Ԥ��һ����¼�Ĵ�С
		csname = add_coffstr(symtab->link, name);
		cfsym->Name = csname - symtab->link->data;	//������ַ����ڵ�����ƫ�Ƶ�ַ
		cfsym->Value = val;
		cfsym->Section = sec_index;
		cfsym->Type = type;
		cfsym->StorageClass = StorageClass;
		keyno = ElfHash(name);
		cfsym->Next = hashtab[keyno];	//ָ����һ����ͻ
		cs = cfsym - (CoffSym*)symtab->data;	//��¼�ڷ��ű��е�ƫ�Ƶ�ַ
		hashtab[keyno] = cs;	//ָ�����µĳ�ͻ

	}
	return cs;
}

//�ڷ��ű��в���ĳ�������Ƿ����
int search_coffsym(Section* symtab, char* sym_name)
{
	CoffSym* cfsym;
	char* name;
	int keyno = ElfHash(sym_name); //��ȡ��ϣ���ֵ
	int cs = symtab->hashtable[keyno];	//cs��ֵ�����Ÿ÷����ڷ��ű��е�ƫ��λ�ã�Ҳ���ǵڼ�����¼
	while (cs)
	{
		cfsym = ((CoffSym*)symtab->data) + cs;	//+1��������sizeof(CoffSym)��С���ڴ棬��ô+cs��������cs����¼
		name = symtab->link->data + cfsym->Name; //���ַ����ڲ���
		if (!strcmp(sym_name, name))	//�ҵ��˸÷���
		{
			return cs;	//�����ǵڼ�����¼
		}
		cs = cfsym->Next;	//��ϣ��ͻ�ˣ�����һ�����ܵļ�¼
	}

	return cs;
}

//Ϊ���������ռ䣬Ҳ�����ʼ��size��С���ܶ�
void* section_ptr_add(Section* sec, int size)
{
	int offset = sec->data_offset;
	sec->data_offset = offset + size;
	if (sec->data_offset > sec->sec_size)
	{
		realloc_section(sec, sec->data_offset); //sec->data_offsetΪ��ǰƫ�Ƶ�ַ��Ҳ�ǵ�ǰ������
	}
	return sec->data + offset; //����ԭ���ĵ�ַ����ǰƫ�����Ѿ����ģ������ݻ�û����
}

void realloc_section(Section* sec, int new_size)
{
	char* data;
	int size = sec->sec_size;
	//��֮ǰһ��������size������Ƶ������realloc
	while (size < new_size)
	{
		size = size * 2;
	}
	data = realloc(sec->data, size);
	if (!data)
	{
		error("�ڴ����ʧ��");
	}
	memset(data + sec->sec_size, 0, size - sec->sec_size);	//ԭ����data���ֲ��䣬��������ڴ�ȫ��Ϊ0
	sec->data = data;
	sec->sec_size = size;
}

//���ַ�������ӷ���
char* add_coffstr(Section* sec, char* name)
{
	int len = strlen(name);
	char* pstr = section_ptr_add(sec, len + 1); //\0ҲҪд���ڴ�
	memcpy(pstr, name, len);	//���ﲻ��len+1����Ϊ�������len+1��С���ڴ棬�������һ���ֽ�û��Ϊ0
	return pstr;				//���൱�������\0
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
	else //��������������
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

//�ӵ�ǰλ�õ�new_pos��0
void fpad(FILE* fp, int new_pos)
{
	int curpos = ftell(fp); //ftell���ص�ǰ�ļ�λ�ã��ǵ�ǰ���ֽ�
	while (++curpos <= new_pos)
		fputc(0, fp);
}

void write_obj(char* name)
{
	int file_offset;
	FILE* fout = fopen(name, "wb");
	int i, sh_size, nsec_obj = 0;
	IMAGE_FILE_HEADER* fh;
	nsec_obj = sections.count - 2;	//Ҫд�뵽obj�Ľڵ���Ŀ�������Ժ�Ҫ�ģ�����ɾȥ��̬���ӽڣ��Ѹģ�
	sh_size = sizeof(IMAGE_SECTION_HEADER); //40
	file_offset = sizeof(IMAGE_FILE_HEADER) + nsec_obj * sh_size; //20+8*40=340;
	fpad(fout, file_offset);	//��д��COFF�ļ�ͷ�ͽ�ͷ��ȫΪ0
	fh = mallocz(sizeof(IMAGE_FILE_HEADER));
	for (i = 0; i < nsec_obj; i++)
	{
		Section* sec = (Section*)sections.data[i];
		if (sec->data == NULL) continue;
		fwrite(sec->data, 1, sec->data_offset, fout);	//д������ݣ�sec->data_offset���ֽ�
		sec->sh.PointerToRawData = file_offset;
		sec->sh.SizeOfRawData = sec->data_offset;
		file_offset += sec->data_offset;
	}
	fseek(fout, 0, SEEK_SET);	//�ļ�ָ��ָ���ļ���ʼ
	fh->Machine = IMAGE_FILE_MACHINE_I386;
	fh->NumberOfSections = nsec_obj;
	fh->PointerToSymbolTable = sec_symtab->sh.PointerToRawData;	//ָ����ű��ָ��
	fh->NumberOfSymbols = sec_symtab->sh.SizeOfRawData / sizeof(CoffSym); //�м������ż�¼
	fwrite(fh, 1, sizeof(IMAGE_FILE_HEADER), fout); // ��COFF�ļ�ͷ����д�룬֮ǰ��Ϊ������δ���ɣ�����COFF�ļ�ͷ����ȷ��
	for (i = 0; i < nsec_obj; i++)
	{
		Section* sec = (Section*)sections.data[i];
		/* ��Ϊ֮ǰ�����ݻ�û���ɣ����Ա���Ƚ��������ɺ����ȷ����ͷ���ָ��ڵ���ʼ��ַ�ͽڴ�С */
		fwrite(sec->sh.Name, 1, sh_size, fout); //����ͷ�����������д�룬���ڽṹ���������洢�ĵ�Ԫ
												//���ֻ��Ҫ����һ����Ա�����ĵ�ַ������
	}

	free(fh);
	fclose(fout);
}
