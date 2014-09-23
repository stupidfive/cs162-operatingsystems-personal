#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

char *make_word(char *word) {
  return strcpy(malloc(strlen(word)+1),word);
}

dict_t *make_dict(char *word) {
  dict_t *nd = (dict_t *) malloc(sizeof(dict_t));
  nd->word = make_word(word);
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

dict_t *insert_word(dict_t *d, char *word) {
  /* 
     Insert word into dict or increment count if already there
     return pointer to the updated dict
 */
  dict_t *nd;
  dict_t *pd = NULL;		/* prior to insertion point */
  dict_t *di = d;		/* following insertion point */
  /* Search down list to find if present or point of insertion */
  while (di && (strcmp(word, di->word) >= 0)) { 
    if (strcmp(word,di->word) == 0) { /* found word */
      di->count++;		/* increment count */
      return d;			/* return head */
    }
    pd = di;			/* advance ptr pair */
    di = di->next;
  }
  nd = make_dict(word);		/* not found, make entry */
  nd->next = di;		/* entry bigger than word or tail */
  if (pd) {
    pd->next = nd;
    return d;			/* insert beond head */
  }
  return nd;
}

void print_dict(dict_t *d) {
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int get_word(char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  while ((c = fgetc(infile)) != EOF) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	/* terminate the word string */
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			/* no more words */
}


#define MAXWORD 1024
dict_t *words(FILE *infile) {
  dict_t *wd = NULL;
  char wordbuf[MAXWORD];
  while (get_word(wordbuf,MAXWORD, infile)) {
    wd = insert_word(wd, wordbuf); /* add to dict */
  }
  return wd;
}

int main (int argc, char *argv[]) {
  dict_t *d = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if (!infile) {
    printf("Unable to open %s\n",argv[1]);
    exit(1);
  }
  d = words(infile);
  print_dict(d);
  fclose(infile);
  return 0;
}

