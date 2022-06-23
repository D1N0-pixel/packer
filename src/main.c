#include <stdio.h>
#include "pack.h"
#include "parse.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [input] [output]\n", argv[0]);
        return -1;
    }

    char *fi = argv[1];
    char *fo = argv[2];

    FileStruct *infile = read_elf(fi);

    if (infile->buf == NULL) {
        return -1;
    }

    ElfStruct *hdr = parse_header(infile);

    if (hdr == NULL) {
        fprintf(stderr, "Invalid file format\n");
        finish(infile, hdr);
        return -1;
    }
    if (!valid_hdr(hdr)) {
        finish(infile, hdr);
        return -1;
    }

    if (pack(fo, hdr)) {
        fprintf(stderr, "Fail to pack the file\n");
        finish(infile, hdr);
        return -1;
    }

    puts("Success!");
    finish(infile, hdr);

    return 0;
}