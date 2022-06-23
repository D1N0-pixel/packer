#include "util.h"

int pack(char *fo, ElfStruct *hdr);
int gen_header(ElfStruct *hdr, FILE *outfile);
char *get_gnu_stack(ElfStruct *hdr);
void finish(FileStruct *infile, ElfStruct *hdr);
unsigned long long int getbrk(Elf64_Phdr *phdr, int e_phnum);