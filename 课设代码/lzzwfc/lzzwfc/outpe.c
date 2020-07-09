#include "outpe.h"
#include "DynArray.h"
#include "coff.h"
#include "DynArray.h"
#include <stdio.h>
char* entry_symbol = "_entry";
DynArray array_dll;
DynArray array_lib;
char* lib_path;
short subsystem;
int nsec_images;
char* outfile;
int output_type;
float scc_version = 1.00;
//MS-DOSͷ
IMAGE_DOS_HEADER dos_header = {
    /* IMAGE_DOS_HEADER doshdr */
    0x5A4D, /*WORD e_magic;         DOS��ִ���ļ����,Ϊ'MZ'  */
    0x0090, /*WORD e_cblp;          Bytes on last page of file */
    0x0003, /*WORD e_cp;            Pages in file */
    0x0000, /*WORD e_crlc;          Relocations */

    0x0004, /*WORD e_cparhdr;       Size of header in paragraphs */
    0x0000, /*WORD e_minalloc;      Minimum extra paragraphs needed */
    0xFFFF, /*WORD e_maxalloc;      Maximum extra paragraphs needed */
    0x0000, /*WORD e_ss;            DOS����ĳ�ʼ����ջ�� */

    0x00B8, /*WORD e_sp;            DOS����ĳ�ʼ����ջָ�� */
    0x0000, /*WORD e_csum;          Checksum */
    0x0000, /*WORD e_ip;            DOS��������IP */
    0x0000, /*WORD e_cs;            DOS��������CS */
    0x0040, /*WORD e_lfarlc;        File address of relocation table */
    0x0000, /*WORD e_ovno;          Overlay number */
    {0,0,0,0}, /*WORD e_res[4];     Reserved words */
    0x0000, /*WORD e_oemid;         OEM identifier (for e_oeminfo) */
    0x0000, /*WORD e_oeminfo;       OEM information; e_oemid specific */
    {0,0,0,0,0,0,0,0,0,0}, /*WORD e_res2[10];      Reserved words */
    0x00000080  /*DWORD   e_lfanew;        ָ��PE�ļ�ͷ */
};

//MS-DOSռλ����
BYTE dos_stub[0x40] = {
    0x0e,0x1f,0xba,0x0e,0x00,0xb4,0x09,0xcd,0x21,0xb8,0x01,0x4c,0xcd,0x21,0x54,0x68,
    0x69,0x73,0x20,0x70,0x72,0x6f,0x67,0x72,0x61,0x6d,0x20,0x63,0x61,0x6e,0x6e,0x6f,
    0x74,0x20,0x62,0x65,0x20,0x72,0x75,0x6e,0x20,0x69,0x6e,0x20,0x44,0x4f,0x53,0x20,
    0x6d,0x6f,0x64,0x65,0x2e,0x0d,0x0d,0x0a,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

//NTͷ
IMAGE_NT_HEADERS32 nt_header = {
    0x00004550, /* DWORD Signature = IMAGE_NT_SIGNATURE PE�ļ���ʶ*/
    {
        /* IMAGE_FILE_HEADER FileHeader */
        0x014C,		/*WORD    Machine; ����ƽ̨*/
        0x0003,		/*WORD    NumberOfSections; �ļ��Ľ���Ŀ */
        0x00000000, /*DWORD   TimeDateStamp; �ļ��Ĵ������ں�ʱ��*/
        0x00000000, /*DWORD   PointerToSymbolTable; ָ����ű�(���ڵ���)*/
        0x00000000, /*DWORD   NumberOfSymbols; ���ű��еķ������������ڵ��ԣ�*/
        0x00E0,		/*WORD    SizeOfOptionalHeader; IMAGE_OPTIONAL_HEADER32�ṹ�ĳ���*/
        0x030F		/*WORD    Characteristics; �ļ�����*/
    },
    {
        /* IMAGE_OPTIONAL_HEADER OptionalHeader */
        /* ��׼��. */
        0x010B, /*WORD    Magic; */
        0x06, /*BYTE    MajorLinkerVersion; ���������汾��*/
        0x00, /*BYTE    MinorLinkerVersion; �������ΰ汾��*/
        0x00000000, /*DWORD   SizeOfCode; ���к�����ε��ܴ�С*/
        0x00000000, /*DWORD   SizeOfInitializedData; �����ѳ�ʼ�����ݶε��ܴ�С*/
        0x00000000, /*DWORD   SizeOfUninitializedData; ���к���δ��ʼ�����ݶεĴ�С*/
        0x00000000, /*DWORD   AddressOfEntryPoint; ����ִ����ڵ���������ַ*/
        0x00000000, /*DWORD   BaseOfCode; ����ε���ʼRVA*/
        0x00000000, /*DWORD   BaseOfData; ����ε���ʼRVA*/

        /* NT������ */
        0x00400000, /*DWORD   ImageBase; ����Ľ���װ�ص�ַ*/
        0x00001000, /*DWORD   SectionAlignment; �ڴ��жεĶ�������*/
        0x00000200, /*DWORD   FileAlignment; �ļ��жεĶ�������*/
        0x0004, /*WORD    MajorOperatingSystemVersion; ����ϵͳ�����汾��*/
        0x0000, /*WORD    MinorOperatingSystemVersion; ����ϵͳ�Ĵΰ汾��*/
        0x0000, /*WORD    MajorImageVersion; ��������汾��*/
        0x0000, /*WORD    MinorImageVersion; ����Ĵΰ汾��*/
        0x0004, /*WORD    MajorSubsystemVersion; ��ϵͳ�����汾��*/
        0x0000, /*WORD    MinorSubsystemVersion; ��ϵͳ�Ĵΰ汾��*/
        0x00000000, /*DWORD   Win32VersionValue; ��������Ϊ0*/
        0x00000000, /*DWORD   SizeOfImage; �ڴ�������PEӳ��ߴ�*/
        0x00000200, /*DWORD   SizeOfHeaders; ����ͷ+�ڱ�Ĵ�С*/
        0x00000000, /*DWORD   CheckSum; У���*/
        0x0003, /*WORD    Subsystem; �ļ�����ϵͳ*/
        0x0000, /*WORD    DllCharacteristics; */
        0x00100000, /*DWORD   SizeOfStackReserve; ��ʼ��ʱ��ջ��С*/
        0x00001000, /*DWORD   SizeOfStackCommit; ��ʼ��ʱʵ���ύ�Ķ�ջ��С*/
        0x00100000, /*DWORD   SizeOfHeapReserve; ��ʼ��ʱ�����ĶѴ�С*/
        0x00001000, /*DWORD   SizeOfHeapCommit; ��ʼ��ʱʵ���ύ�ĶѴ�С*/
        0x00000000, /*DWORD   LoaderFlags; ��������Ϊ0*/
        0x00000010, /*DWORD   NumberOfRvaAndSizes; ���������Ŀ¼�ṹ������*/

                /* IMAGE_DATA_DIRECTORY DataDirectory[16]; ����Ŀ¼*/
        {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}}
    } };


/* ------------------------------------------------------------- */

/***********************************************************
 * ����:	�����ļ�����λ��
 * n:		δ����ǰλ��
 **********************************************************/
DWORD pe_file_align(DWORD n)
{
    DWORD FileAlignment = nt_header.OptionalHeader.FileAlignment; //�ļ��жεĶ�������
    return size_align(n, FileAlignment);
}

/***********************************************************
 * ����:	�����ڴ����λ��
 * n:		δ����ǰλ��
 **********************************************************/
DWORD pe_virtual_align(DWORD n)
{
    DWORD SectionAlignment = nt_header.OptionalHeader.SectionAlignment; //�ڴ��жεĶ�������
    return size_align(n, SectionAlignment);
}

/***********************************************************
 * ����:	��������Ŀ¼
 * dir:		Ŀ¼����
 * addr:	���RVA
 * size:	��Ĵ�С(���ֽڼ�)
 **********************************************************/
void pe_set_datadir(int dir, DWORD addr, DWORD size)
{
    nt_header.OptionalHeader.DataDirectory[dir].VirtualAddress = addr;
    nt_header.OptionalHeader.DataDirectory[dir].Size = size;
}

/***********************************************************
 * ����:	����Ŀ���ļ�
 * fname:	Ŀ���ļ���
 **********************************************************/
int load_obj_file(char* fname)
{
    IMAGE_FILE_HEADER fh;
    Section** secs;
    int i, sh_size, nsec_obj = 0, nsym;
    FILE* fobj;
    CoffSym* cfsyms; //ȫ���ķ���
    CoffSym* cfsym; //����������Ŀ
    char* strs, * csname;
    void* ptr;
    int cur_text_offset;
    int cur_rel_offset;
    int* old_to_new_syms;
    int cfsym_index;
    CoffReloc* rel, * rel_end;
    sh_size = sizeof(IMAGE_SECTION_HEADER);
    fobj = fopen(fname, "rb");
    fread(&fh, 1, sizeof(IMAGE_FILE_HEADER), fobj);
    nsec_obj = fh.NumberOfSections;
    secs = (Section**)sections.data;
    for (i = 0; i < nsec_obj; i++)
    {
        fread(secs[i]->sh.Name, 1, sh_size, fobj);
    }

    cur_text_offset = sec_text->data_offset;
    cur_rel_offset = sec_rel->data_offset;
    for (i = 0; i < nsec_obj; i++)
    {
        if (!strcmp(secs[i]->sh.Name, ".symtab"))
        {
            cfsyms = mallocz(secs[i]->sh.SizeOfRawData);
            fseek(fobj, secs[i]->sh.PointerToRawData, SEEK_SET);
            fread(cfsyms, 1, secs[i]->sh.SizeOfRawData, fobj);
            nsym = secs[i]->sh.SizeOfRawData / sizeof(CoffSym);
            continue;
        }
        if (!strcmp(secs[i]->sh.Name, ".strtab"))
        {
            strs = mallocz(secs[i]->sh.SizeOfRawData);
            fseek(fobj, secs[i]->sh.PointerToRawData, SEEK_SET);
            fread(strs, 1, secs[i]->sh.SizeOfRawData, fobj);
            continue;
        }
        if (!strcmp(secs[i]->sh.Name, ".dynsym") || !strcmp(secs[i]->sh.Name, ".dynstr"))
            continue;
        fseek(fobj, secs[i]->sh.PointerToRawData, SEEK_SET); //poinitertorawdataΪ�ýڵ�λ�þ��ļ���ʼλ�õ�ƫ��
        ptr = section_ptr_add(secs[i], secs[i]->sh.SizeOfRawData);
        fread(ptr, 1, secs[i]->sh.SizeOfRawData, fobj);
    }

    /* ���������ҵ�ÿ�������Ƿ��ű�ĵڼ������� */
    old_to_new_syms = mallocz(sizeof(int) * nsym);
    for (i = 1; i < nsym; i++) //������һ���հ׷���
    {
        cfsym = &cfsyms[i];
        csname = strs + cfsym->Name;
        cfsym_index = add_coffsym(sec_symtab, csname, cfsym->Value, cfsym->Section, cfsym->Type, cfsym->StorageClass);
        old_to_new_syms[i] = cfsym_index;
    }

    /* �����ض�λ�ڵ����� */
    rel = (CoffReloc*)(sec_rel->data + cur_rel_offset);
    rel_end = (CoffReloc*)(sec_rel->data + sec_rel->data_offset);
    for (; rel < rel_end; rel++)
    {
        cfsym_index = rel->cfsym;
        rel->cfsym = old_to_new_syms[cfsym_index];
        rel->offset += cur_text_offset;
    }

    free(cfsyms);
    free(strs);
    free(old_to_new_syms);
    fclose(fobj);
    return 1;
}

/***********************************************************
 * ����:	�Ӿ�̬���ļ��ж�ȡһ��,���������Ч�ַ�
 * line:	���ݴ洢λ��
 * size:	��ȡ������ַ���
 * fp:		�Ѵ򿪵ľ�̬���ļ�ָ��
 **********************************************************/
char* get_line(char* line, int size, FILE* fp)
{
    char* p, * p1;
    if (NULL == fgets(line, size, fp))
        return NULL;

    //ȥ����ո�
    p = line;
    while (*p && isspace(*p))
        p++;

    //ȥ���ҿո񼰻س���
    p1 = strchr(p, '\0');
    while (p1 > p && p1[-1] <= ' ')
        p1--;
    *p1 = '\0';

    return p;
}

/***********************************************************
 * ����:	��libfile�õ���Ӧdll��
 * libfile: ��̬���ļ���
 **********************************************************/
char* get_dllname(char* libfile)
{
    int n1, n2;
    char* libname, * dllname, * p;
    n1 = strlen(libfile);
    libname = libfile;
    for (p = libfile + n1; n1 > 0; p--)
    {
        if (*p == '\\')
        {
            libname = p + 1;
            break;
        }
    }

    n2 = strlen(libname);
    dllname = mallocz(sizeof(char) * n2);
    memcpy(dllname, libname, n2 - 4);
    memcpy(dllname + n2 - 4, "dll", 3);
    return dllname;
}

/***********************************************************
 * ����:	���ؾ�̬��
 * name:    ��̬���ļ���,������·���ͺ�׺
 **********************************************************/
int pe_load_lib_file(char* name)
{
    char libfile[MAX_PATH];
    int ret = -1;
    char line[400], * dllname, * p;
    FILE* fp;
    sprintf(libfile, "%s%s.slib", lib_path, name);
    fp = fopen(libfile, "rb");
    if (fp)
    {
        dllname = get_dllname(libfile);
        array_push(&array_dll, dllname);
        for (;;)
        {
            p = get_line(line, sizeof line, fp);
            if (NULL == p)
                break;
            if (0 == *p || ';' == *p)
                continue;
            add_coffsym(sec_dynsymtab, p, array_dll.count, sec_text->idx, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL); //�˴������⣬i>180ʱ����û����Ӧ��
        }
        ret = 0;
        if (fp)
            fclose(fp);
    }
    else
    {
        link_error("\"%s\" �ļ���ʧ��", libfile);
    }
    return ret;
}

/***********************************************************
 * ����:	���������ڵ�
 * pe:		PE��Ϣ�洢�ṹָ��
 **********************************************************/
void get_entry_addr(struct PEInfo* pe)
{
    unsigned long addr = 0;
    int cs;
    CoffSym* cfsym_entry;
    cs = search_coffsym(sec_symtab, entry_symbol);
    cfsym_entry = (CoffSym*)sec_symtab->data + cs;
    addr = cfsym_entry->Value;
    pe->entry_addr = addr;
}


char* get_lib_path()
{
    char path[MAX_PATH];
    char* p;
    GetModuleFileNameA(NULL, path, sizeof(path)); //��ȡ��ǰִ���ļ��ľ���·��
    p = strrchr(path, '\\'); //ָ���ַ������һ�γ����ַ��ĵط�
    *p = '\0';
    strcat(path, "\\lib\\");
    return strdup(path);
}

/***********************************************************
 * ����:	������Ҫ���ӵľ�̬��
 **********************************************************/
void add_runtime_libs()
{
    int i;

    int pe_type = 0;

    //for (i = 0; i < array_lib.count; i++)
    //{
    //    pe_load_lib_file(array_lib.data[i]);
    //}
    pe_load_lib_file("msvcrt");
}

/***********************************************************
 * ����:	���ҵ��뺯��
 * symbol:  ������
 **********************************************************/
int pe_find_import(char* symbol)
{
    int sym_index;
    sym_index = search_coffsym(sec_dynsymtab, symbol);
    return sym_index;
}

/***********************************************************
 * ����:		���ӵ��뺯��
 * pe:			PE��Ϣ�洢�ṹָ��
 * sym_index:	���ű������
 * name:		������
 **********************************************************/
struct ImportSym* pe_add_import(struct PEInfo* pe, int sym_index, char* name)
{
    int i;
    int dll_index;
    struct ImportInfo* p;
    struct ImportSym* s;

    dll_index = ((CoffSym *)sec_dynsymtab->data + sym_index)->Value;
    if (0 == dll_index)
        return NULL;

    i = array_search(&pe->imps, dll_index);
    if (-1 != i) {
        p = pe->imps.data[i];
    }
    else
    {
        p = mallocz(sizeof * p);
        array_init(&p->imp_syms, 8);
        p->dll_index = dll_index;
        array_push(&pe->imps, p);
    }

    i = array_search(&p->imp_syms, sym_index);
    if (-1 != i)//�ҵ�ֱ�ӷ��أ��Ҳ��������
    {
        return (struct ImportSym*)p->imp_syms.data[i];
    }
    else
    {
        s = mallocz(sizeof(struct ImportSym) + strlen(name));
        array_push(&p->imp_syms, s);
        strcpy((char*)&s->imp_sym.Name, name);
        return s;
    }
}

/***********************************************************
 * ����:	�����������õ����ⲿ����
 * pe:		PE��Ϣ�洢�ṹָ��
 **********************************************************/
int resolve_coffsyms(struct PEInfo* pe)
{
    CoffSym* sym;
    int sym_index, sym_end;
    int ret = 0;
    int found = 1;

    sym_end = sec_symtab->data_offset / sizeof(CoffSym);
    for (sym_index = 1; sym_index < sym_end; sym_index++)
    {
        sym = (CoffSym*)sec_symtab->data + sym_index;

        if (sym->Section == IMAGE_SYM_UNDEFINED)
        {
            char* name = sec_symtab->link->data + sym->Name;

            unsigned type = sym->Type;
            int imp_sym = pe_find_import(name);
            struct ImportSym* is;
            if (0 == imp_sym)
                found = 0;
            is = pe_add_import(pe, imp_sym, name);
            if (!is)
                found = 0;

            if (found && type == CST_FUNC)
            {
                int offset = is->thk_offset;

                char buffer[100];

                offset = sec_text->data_offset;
                /*FF /4		JMP r/m32	Jump near,absolute indirect,address given in r/m32*/
                *(WORD*)section_ptr_add(sec_text, 6) = 0x25FF;


                sprintf(buffer, "IAT.%s", name);
                is->iat_index = add_coffsym(sec_symtab, buffer, 0, sec_idata->idx, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL);

                direct_add_coffreloc(offset + 2, is->iat_index, sec_text->idx, IMAGE_REL_I386_DIR32);
                is->thk_offset = offset;

                sym = (CoffSym*)sec_symtab->data + sym_index;
                sym->Value = offset;
                sym->Section = sec_text->idx;
            }
            else
            {
                link_error("'%s'δ����", name);
                ret = 1;
            }
        }
    }
    return ret;
}

/***********************************************************
 * ����:	������ű���д������ַ���(dll���ƻ������)
 * sec:		����������ڽ�
 * sym:		��������
 **********************************************************/
int put_import_str(Section* sec, char* sym)
{
    int offset, len;
    char* ptr;
    len = strlen(sym) + 1;
    offset = sec->data_offset;
    ptr = section_ptr_add(sec, len);
    memcpy(ptr, sym, len);
    return offset;
}

/***********************************************************
 * ����:	����������Ϣ������Ŀ¼��������ű�
 * pe:		pe:		PE��Ϣ�洢�ṹָ��
 **********************************************************/
void pe_build_imports(struct PEInfo* pe)
{
    int thk_ptr, ent_ptr, dll_ptr, sym_cnt, i;
    DWORD rva_base = pe->thunk->sh.VirtualAddress - nt_header.OptionalHeader.ImageBase;
    int ndlls = pe->imps.count;

    for (sym_cnt = i = 0; i < ndlls; ++i)
        sym_cnt += ((struct ImportInfo*)pe->imps.data[i])->imp_syms.count;

    if (0 == sym_cnt)
        return;

    pe->imp_offs = dll_ptr = pe->thunk->data_offset;
    pe->imp_size = (ndlls + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
    pe->iat_offs = dll_ptr + pe->imp_size;
    pe->iat_size = (sym_cnt + ndlls) * sizeof(DWORD);
    section_ptr_add(pe->thunk, pe->imp_size + 2 * pe->iat_size);//Ԥ����dll������iat��ռ�

    thk_ptr = pe->iat_offs;
    ent_ptr = pe->iat_offs + pe->iat_size;

    for (i = 0; i < pe->imps.count; ++i)
    {
        int k, n, v;
        struct ImportInfo* p = pe->imps.data[i];
        char* name = array_dll.data[p->dll_index - 1];

        /* д��dll���� */
        v = put_import_str(pe->thunk, name);

        p->imphdr.FirstThunk = thk_ptr + rva_base;
        p->imphdr.OriginalFirstThunk = ent_ptr + rva_base;
        p->imphdr.Name = v + rva_base;
        memcpy(pe->thunk->data + dll_ptr, &p->imphdr, sizeof(IMAGE_IMPORT_DESCRIPTOR)); //���ӵ������Ŀ

        for (k = 0, n = p->imp_syms.count; k <= n; ++k)
        {
            if (k < n)
            {
                struct ImportSym* is = (struct ImportSym*)p->imp_syms.data[k];
                DWORD iat_index = is->iat_index;
                CoffSym* org_sym = (CoffSym*)sec_symtab->data + iat_index;

                org_sym->Value = thk_ptr;
                org_sym->Section = pe->thunk->idx;
                v = pe->thunk->data_offset + rva_base;

                section_ptr_add(pe->thunk, sizeof(is->imp_sym.Hint));
                put_import_str(pe->thunk, is->imp_sym.Name);

            }
            else
            {
                v = 0;
            }
            *(DWORD*)(pe->thunk->data + thk_ptr) =
                *(DWORD*)(pe->thunk->data + ent_ptr) = v;
            thk_ptr += sizeof(DWORD);
            ent_ptr += sizeof(DWORD);
        }
        dll_ptr += sizeof(IMAGE_IMPORT_DESCRIPTOR);
        //dynarray_free(&p->imp_syms);
    }
    //dynarray_free(&pe->imps);
}


int pe_assign_addresses(struct PEInfo* pe)
{
    printf("\nnsec_images:%d", nsec_images);
    int i;
    DWORD addr;
    Section* sec, ** ps;

    pe->thunk = sec_idata;

    pe->secs = (Section**)mallocz(nsec_images * sizeof(Section*));
    addr = nt_header.OptionalHeader.ImageBase + 1;

    for (i = 0; i < nsec_images; ++i)
    {
        sec = (Section*)sections.data[i];
        ps = &pe->secs[pe->sec_size];
        *ps = sec;

        sec->sh.VirtualAddress = addr = pe_virtual_align(addr);

        if (sec == pe->thunk)
        {
            pe_build_imports(pe);
        }

        if (sec->data_offset)
        {
            addr += sec->data_offset;
            ++pe->sec_size;
        }
    }
    return 0;
}


void relocate_syms()
{
    CoffSym* sym, * sym_end;
    Section* sec;

    sym_end = (CoffSym*)(sec_symtab->data + sec_symtab->data_offset);
    for (sym = (CoffSym*)sec_symtab->data + 1; sym < sym_end; sym++)
    {
        sec = (Section*)sections.data[sym->Section - 1];
        sym->Value += sec->sh.VirtualAddress; // ���Ͻ�����RVA��ַ
    }
}


void coffrelocs_fixup()
{
    Section* sec, * sr;
    CoffReloc* rel, * rel_end, * qrel;
    CoffSym* sym;
    int type, sym_index;
    char* ptr;
    unsigned long val, addr;
    char* name;


    sr = sec_rel;
    rel_end = (CoffReloc*)(sr->data + sr->data_offset);
    qrel = (CoffReloc*)sr->data;
    for (rel = qrel; rel < rel_end; rel++)
    {
        sec = (Section*)sections.data[rel->section - 1];

        sym_index = rel->cfsym;
        sym = &((CoffSym*)sec_symtab->data)[sym_index];
        name = sec_symtab->link->data + sym->Name;
        val = sym->Value;
        type = rel->type;
        addr = sec->sh.VirtualAddress + rel->offset;

        ptr = sec->data + rel->offset;
        switch (type)
        {
        case IMAGE_REL_I386_DIR32:  //ȫ�ֱ���
            *(int*)ptr += val;
            break;
        case IMAGE_REL_I386_REL32:  //��������
            *(int*)ptr += val - addr;
            break;
        }
    }
}


int pe_write(struct PEInfo* pe)
{
    int i;
    FILE* op;
    DWORD file_offset, r;
    int sizeofheaders;

    op = fopen(pe->filename, "wb");
    if (NULL == op)
    {
        link_error("'%s'����ʧ��", pe->filename);
        return 1;
    }

    sizeofheaders = pe_file_align(
        sizeof(dos_header)
        + sizeof(dos_stub)
        + sizeof(nt_header)
        + pe->sec_size * sizeof(IMAGE_SECTION_HEADER)
        );

    file_offset = sizeofheaders;
    fpad(op, file_offset);


    for (i = 0; i < pe->sec_size; ++i)
    {
        Section* sec = pe->secs[i];
        char* sh_name = sec->sh.Name;
        unsigned long addr = sec->sh.VirtualAddress - nt_header.OptionalHeader.ImageBase;
        unsigned long size = sec->data_offset;
        IMAGE_SECTION_HEADER* psh = &sec->sh;


        if (!strcmp(sec->sh.Name, ".text"))
        {
            nt_header.OptionalHeader.BaseOfCode = addr;
            nt_header.OptionalHeader.AddressOfEntryPoint = addr + pe->entry_addr;
        }
        else if (!strcmp(sec->sh.Name, ".data"))
            nt_header.OptionalHeader.BaseOfData = addr;
        else if (!strcmp(sec->sh.Name, ".idata"))
        {
            if (pe->imp_size)
            {
                pe_set_datadir(IMAGE_DIRECTORY_ENTRY_IMPORT,
                    pe->imp_offs + addr, pe->imp_size);
                pe_set_datadir(IMAGE_DIRECTORY_ENTRY_IAT,
                    pe->iat_offs + addr, pe->iat_size);
            }
        }

        strcpy((char*)psh->Name, sh_name);

        psh->VirtualAddress = addr;
        psh->Misc.VirtualSize = size;
        nt_header.OptionalHeader.SizeOfImage = pe_virtual_align(
            size + addr
            );

        if (sec->data_offset)
        {
            psh->PointerToRawData = r = file_offset;
            if (!strcmp(sec->sh.Name, ".bss"))
            {
                sec->sh.SizeOfRawData = 0;
                continue;
            }
            fwrite(sec->data, 1, sec->data_offset, op);
            file_offset = pe_file_align(file_offset + sec->data_offset);
            psh->SizeOfRawData = file_offset - r;
            fpad(op, file_offset);
        }
    }

    nt_header.FileHeader.NumberOfSections = pe->sec_size;
    nt_header.OptionalHeader.SizeOfHeaders = sizeofheaders;

    nt_header.OptionalHeader.Subsystem = subsystem;

    fseek(op, 0, SEEK_SET);
    fwrite(&dos_header, 1, sizeof dos_header, op);
    fwrite(&dos_stub, 1, sizeof dos_stub, op);
    fwrite(&nt_header, 1, sizeof nt_header, op);
    for (i = 0; i < pe->sec_size; ++i)
        fwrite(&pe->secs[i]->sh, 1, sizeof(IMAGE_SECTION_HEADER), op);
    fclose(op);
    return 0;
}


int pe_output_file(char* filename)
{
    int ret;
    struct PEInfo pe;

    memset(&pe, 0, sizeof(pe));
    array_init(&pe.imps, 8);
    pe.filename = filename;
    /* ������Ҫ���ӵľ�̬�� */
    add_runtime_libs();
    /* ���������ڵ� */
    get_entry_addr(&pe);
    /* �����������õ����ⲿ���� */
    ret = resolve_coffsyms(&pe);
    if (0 == ret)
    {

        /* ���������RVA��ַ */
        pe_assign_addresses(&pe);
        /* �ض�λ���ŵ�ַ */
        relocate_syms();
        /* ������Ҫ�����ض�λ�Ĵ�������ݵĵ�ַ */
        coffrelocs_fixup();
        /* дPE�ļ� */
        ret = pe_write(&pe);
        free(pe.secs);
    }

    return ret;
}

