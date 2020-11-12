#include "common.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;

void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}



unsigned Mark_Value(char *str,bool *success){
        int i;
        bool judge=false;
        for(i=0;i<nr_symtab_entry;i++){
                judge=true;
		int trans_info=(int)symtab[i].st_info;
		trans_info=trans_info&0xf;
//		printf("000%d\n",trans_info);
                if(trans_info==STT_OBJECT){
                int str_len=strlen(str);
                char* cmp_str=(char*)malloc(str_len+1);
                int j;
//	printf("222%d\n",str_len);
                for(j=0;j<str_len;j++){
                cmp_str[j]=strtab[symtab[i].st_name+j];
  //             printf("111%c%c\n",cmp_str[j],str[j]);
		 }
		cmp_str[str_len]='\0';
                for(j=0;j<str_len;j++){
                if(cmp_str[j]!=str[j])judge=false;
                }
		 if(judge)break;
                }
        }
//	printf("%d%u\n",i,symtab[i].st_value);
       if(judge) return symtab[i].st_value;
       else *success=false;
	return 0;
};



void Function_Addr(swaddr_t curr_addr,char* name,bool* con_or_not){
	int i;
	for (i = 0; i < nr_symtab_entry; i++){
		int trans_info=(int)symtab[i].st_info;
                trans_info=trans_info&0xf;
		if (trans_info == STT_FUNC){
			// printf("0x%08x\n",symtab[i].st_value);
			int str_len=strlen(strtab+symtab[i].st_name);
			name=(char*)malloc(str_len+1);
			if ((curr_addr >= symtab[i].st_value) && (symtab[i].st_value + symtab[i].st_size >= curr_addr)){
				int j;
				for(j=0;j<str_len;j++)name[j]=strtab[symtab[i].st_name+j];
				printf("666 %c\n",name[j]);
				name[str_len]='\0';
				*con_or_not=true;
				return;
			}else *con_or_not=false;
		}
	}
//	name[0]='\0';
	return;
}
