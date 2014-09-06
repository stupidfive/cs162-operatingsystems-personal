#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

void wc(FILE *ofile, FILE *infile, char *inname, int size) {
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
        if (cur_char == '\n') {
            lines++;
        }
        cur_char = fgetc(infile);
    }
    int num_size = count_digits(lines) + count_digits(chars) + count_digits(words) + 3;
    char *num_string = malloc(num_size);
    sprintf(num_string, "%d %d %d ", lines, words, chars);
    fputs(num_string, ofile);
    fputs(inname, ofile);
    fputs("\n", ofile);
}
int count_digits(int num) {
    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    return count;
}

int main (int argc, char *argv[]) {
    FILE *ofile = stdout;
    FILE *infile = stdin;
    int size = 0;
    switch (argc) {
    case 2:
        size = strlen(argv[1]);
        infile = fopen(argv[1], "r");
        wc(ofile, infile, argv[1], size);
        fclose(infile);
        break;
    case 3:
        size = strlen(argv[1]);
        infile = fopen(argv[1], "r");
        ofile = fopen(argv[2], "w");
        wc(ofile, infile, argv[1], size);
        fclose(infile);
        fclose(ofile);
        break;
    case 1:
        wc(ofile, infile, "", size);
        break;
    default:
        printf("usage: ");
        break;
    }
    return 0;
}
