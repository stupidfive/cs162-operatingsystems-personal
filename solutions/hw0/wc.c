#include <stdio.h>

int wc(FILE *ofile, FILE *infile, char *inname) {
  int bytes = 0;
  int words = 0;
  int lines = 0;
  int inword = 0;
  int buf;
   while ((buf = fgetc(infile)) != EOF) {
    bytes++;			
    if (buf == '\n') lines++;
    if (inword && isspace(buf)) {
      words++;
      inword = 0;
    } 
    if (!isspace(buf)) inword = 1;
  }
  fprintf(ofile, "%8d%8d%8d %s\n",lines,words,bytes,inname);
}

int main (int argc, char *argv[]) {
  FILE *infile = stdin;
  FILE *ofile  = stdout;
  char *name = "";
  if (argc >= 3) ofile  = fopen (argv[2],"w");
  if (argc >= 2) {
    name = argv[1];
    infile = fopen (argv[1],"r");
  }
  wc(ofile, infile, name);
  fclose(infile);
  fclose (ofile);
  return 0;
}

