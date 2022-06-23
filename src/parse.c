#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <malloc.h>
#include <string.h>
#include "util.h"
#include "parse.h"

FileStruct *read_elf(char *fi) {
    FILE *fp = fopen(fi, "r");
    if (fp == NULL) {
        fprintf(stderr, "Invalid File\n");
        return NULL;
    }


    FileStruct *infile = malloc(sizeof(FileStruct));
    if (infile == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    infile->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    infile->buf = malloc(infile->size + 1);
    if (infile->buf == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    memset(infile->buf, 0, infile->size + 1);

    fread(infile->buf, infile->size, 1, fp);
    fclose(fp);

    return infile;
}

int valid_hdr(ElfStruct *hdr) {
    if (!check_ehdr(hdr->ehdr)) {
        fprintf(stderr, "Unsupported file format\n");
        return -1;
    }
}

int check_ehdr(Elf64_Ehdr const *ehdr) {
    if (memcmp(ehdr->e_ident, "\x7f\x45\x4c\x46", 4)
    || ehdr->e_ident[EI_CLASS] != ELFCLASS64
    || ehdr->e_ident[EI_DATA] != ELFDATA2LSB
    ) {
        return 0;
    }
    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)
        return 0;
    if (ehdr->e_version != EV_CURRENT)
        return 0;
    if (ehdr->e_machine != EM_X86_64)
        return 0;
    if (ehdr->e_ehsize != sizeof(Elf64_Ehdr))
        return 0;
    if (ehdr->e_phoff == 0)
        return 0;
    if (ehdr->e_phnum < 1)
        return 0;
    if (ehdr->e_phentsize != sizeof(Elf64_Phdr))
        return 0;

    return 1;
}

ElfStruct *parse_header(FileStruct *infile) {
    ElfStruct *hdr = malloc(sizeof(ElfStruct));
    char *buf = infile->buf;
    long long int size = infile->size;
    if (size < 64) {
        return NULL;
    }
    hdr->ehdr = (Elf64_Ehdr *)buf;

    if (size < hdr->ehdr->e_phoff + hdr->ehdr->e_phentsize * hdr->ehdr->e_phnum) {
        return NULL;
    }
    hdr->phdr = (Elf64_Phdr *)(buf + hdr->ehdr->e_phoff);

    if (size < hdr->ehdr->e_shoff + hdr->ehdr->e_shentsize * hdr->ehdr->e_shnum) {
        return NULL;
    }
    hdr->shdr = (Elf64_Shdr *)(buf + hdr->ehdr->e_shoff);

    return hdr;
}