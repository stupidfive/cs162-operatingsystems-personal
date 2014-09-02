#include <stdio.h>
#include <ctype.h>

void wc(FILE *ofile, FILE *infile, char *inname) {
    char cur_char;
    int lines = 0;
    int words = 0;
    int chars = 0;
    int was_word = 0;
    while(cur_char = fgetc(infile) != EOF) {
        chars++;
        if (was_word && isspace(cur_char)) {
            words++;
            was_word = 0;
        }
        if (!isspace(cur_char)) {
            was_word = 1;
        }
        if (cur_char == "\n") {
            lines++;
        }
    }
    char *final_string;
    sprintf(final_string, "%d %d %d %s", lines, words, chars, inname);
    fprintf(ofile, final_string);
}

int main (int argc, char *argv[]) {
    FILE *ofile = stdout;
    FILE *infile;
    switch (argc) {
    case 1:
        infile = fopen(argv[0], "r");
        wc(ofile, infile, argv[0]);
        fclose(infile);
        break;
    case 2:
        infile = fopen(argv[0], "r");
        ofile = fopen(argv[1], "w");
        wc(ofile, infile, argv[0]);
        fclose(infile);
        break;
    default:
        printf("usage: ");
    }
    return 0;
}
