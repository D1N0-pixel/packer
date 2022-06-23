#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

int elf_parse(char *fi, char *buf);
FileStruct *read_elf(char *fi);
int valid_hdr(ElfStruct *hdr);
ElfStruct *parse_header(FileStruct *infile);
int check_ehdr(Elf64_Ehdr const *ehdr);