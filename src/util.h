#include <elf.h>

typedef struct {
    char *buf;
    long long int size;
} FileStruct;

typedef struct {
    Elf64_Ehdr *ehdr;
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    
} ElfStruct;

typedef struct {
    char data[4];
} LE32;

typedef struct {
    char data[2];
} LE16;

typedef struct {
    LE32 l_checksum;
    LE32 l_magic;
    LE16 l_lsize;
    unsigned char l_version;
    unsigned char l_format;
} l_info;

typedef struct {
    Elf64_Ehdr ehdr;
    Elf64_Phdr phdr[3];
    l_info linfo;
} ElfHdr3;