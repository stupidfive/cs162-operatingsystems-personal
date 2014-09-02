#include <stdio.h>
#include <ctype.h>

void wc(FILE *ofile, FILE *infile, char *inname) {
    char cur_char = fgetc(infile);
    int lines = 0;
    int words = 0;
    int chars = 0;
    int was_word = 0;
    
    while(cur_char != EOF) {
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
        cur_char = fgetc(infile);
    }
    char *final_string;
    sprintf(final_string, "%d %d %d %s", lines, words, chars, inname);
    printf("%s", final_string);
    fprintf(ofile, final_string);
}

int main (int argc, char *argv[]) {
    FILE *ofile = stdout;
    FILE *infile;
    printf("argc: %d", argc);
    switch (argc) {
    case 2:
        printf("Case 1: %d, %s", argc, argv[0]);
        infile = fopen(argv[1], "r");
        wc(ofile, infile, argv[1]);
        fclose(infile);
        break;
    case 3:
        infile = fopen(argv[1], "r");
        ofile = fopen(argv[2], "w");
        wc(ofile, infile, argv[1]);
        fclose(infile);
        break;
    default:
        printf("usage: ");
    }
    return 0;
}
