#include <stdio.h>
#include <ctype.h>

void wc(FILE *ofile, FILE *infile, char *inname) {
    int bytes = 0;
    int words = 0;
    int lines = 0;
    int midWord = 0;
    int buf = fgetc(infile);
    while (buf != EOF) {
        bytes++;
        if (buf == '\n') {
            lines++;
        }
        if (midWord && isspace(buf)) {
            words++;
            midWord = 0;
        }
        if (!isspace(buf)) {
            midWord = 1;
        }
        buf = fgetc(infile);
    }
    fprintf(ofile, "%8d%8d%8d %s\n", lines, words, bytes, inname);
}

int main (int argc, char *argv[]) {
    FILE *infile = stdin;
    FILE *ofile = stdout;
    char *name = "";
    if (argc >= 2) {
        name = argv[1];
        infile = fopen(argv[1], "r");
    }
    if (argc >= 3) {
        fopen(argv[2], "w");
    }
    wc(ofile, infile, name);
    fclose(infile);
    fclose(ofile);
    return 0;
}
