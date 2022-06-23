#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "amd64-linux.elf-fold.h"
#include "pack.h"

int pack(char *fo, ElfStruct *hdr) {
    FILE *outfile = fopen(fo, "w");
    ElfHdr3 *outhdr;
    if (outfile == NULL) {
        fprintf(stderr, "Invalid File\n");
        return -1;
    }
    if (gen_header(hdr, outfile) == -1) {
        fprintf(stderr, "Fail to generate header\n");
        return -1;
    }

    fclose(outfile);
    return 0;
}

int gen_header(ElfStruct *hdr, FILE *outfile) {
    ElfHdr3 *out = malloc(sizeof(ElfHdr3));
    ElfHdr3 *const proto = (ElfHdr3 *)stub_amd64_linux_elf_fold;
    out->ehdr = proto->ehdr;

    memcpy(&out->phdr[0], &proto->phdr[1], sizeof(Elf64_Phdr)); // base
    memcpy(&out->phdr[1], &proto->phdr[0], sizeof(Elf64_Phdr)); // text
    memset(&out->linfo, 0, sizeof(out->linfo));

    out->ehdr.e_type = hdr->ehdr->e_type;
    out->ehdr.e_ident[EI_OSABI] = hdr->ehdr->e_ident[EI_OSABI];
    out->ehdr.e_flags = hdr->ehdr->e_flags;

    assert(out->ehdr.e_phoff == sizeof(Elf64_Ehdr));
    assert(out->ehdr.e_ehsize == sizeof(Elf64_Ehdr));
    assert(out->ehdr.e_phentsize == sizeof(Elf64_Phdr));
    out->ehdr.e_shentsize = sizeof(Elf64_Shdr);

    out->ehdr.e_phnum = 3;
    out->ehdr.e_shoff = 0;
    out->ehdr.e_shnum = 0;
    out->ehdr.e_shstrndx = 0;

    char *gnu_stack = get_gnu_stack(hdr);
    if (gnu_stack == NULL) {
        return -1;
    }

    memcpy(&out->phdr[2], gnu_stack, hdr->ehdr->e_phentsize);

    out->phdr[1].p_filesz = sizeof(*out);
    out->phdr[1].p_memsz = out->phdr[1].p_filesz;

    for (unsigned int j=0; j < 3; ++j) {
        if (out->phdr[j].p_type == PT_LOAD)
            out->phdr[j].p_align = 1 << 12;
    }

    unsigned long long int brka = getbrk(hdr->phdr, hdr->ehdr->e_phnum);
    if (brka) {
        unsigned long long int lo_va_user = ~0;
        for (int j=hdr->ehdr->e_phnum; --j>=0; ) {
            if (hdr->phdr[j].p_type == PT_LOAD) {
                unsigned long long int vaddr = hdr->phdr[j].p_vaddr;
                lo_va_user = lo_va_user <= vaddr ? lo_va_user : vaddr;
            }
        }
        out->phdr[0].p_vaddr = lo_va_user;
        out->phdr[0].p_paddr = out->phdr[0].p_vaddr;
        out->phdr[1].p_vaddr = out->phdr[0].p_vaddr;
        out->phdr[1].p_paddr = out->phdr[0].p_vaddr;
        out->phdr[0].p_type = PT_LOAD;
        out->phdr[0].p_offset = 0;
        out->phdr[0].p_filesz = 0;
        out->phdr[0].p_memsz = brka - lo_va_user;
        out->phdr[0].p_flags = PF_R | PF_W;
        out->phdr[1].p_flags = ~PF_W & out->phdr[1].p_flags;
    }

    memset(&out->linfo, 0, sizeof(out->linfo));
    fwrite(out, sizeof(*out), 1, outfile);

    return 0;
}

char *get_gnu_stack(ElfStruct *hdr) {
    for (int i=0; i < hdr->ehdr->e_phnum; i++) {
        if (hdr->phdr[i].p_type == 0x6474e551) {
            return (char *)&hdr->phdr[i];
        }
    }
    return NULL;
}

void finish(FileStruct *infile, ElfStruct *hdr) {
    free(infile->buf);
    free(infile);
    if (hdr != NULL)
        free(hdr);
}

unsigned long long int getbrk(Elf64_Phdr *phdr, int e_phnum) {
    unsigned long long int brka = 0;
    for (int j = 0; j < e_phnum; ++j) {
        if (phdr[j].p_type == PT_LOAD) {
            unsigned long long int b = phdr[j].p_vaddr + phdr[j].p_memsz;
            if (b > brka)
                brka = b;
        }
    }
    return brka;
}
