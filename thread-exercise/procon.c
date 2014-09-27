#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "so.h"

#define MAXLINE 1000
char *readline(FILE *rfile) {
  /* Read a line from a file and return a new line string object */
  char buf[MAXLINE];
  int len;
  char *result = NULL;
  char *line = fgets(buf, MAXLINE, rfile);
  if (line) {
    len = strnlen(buf, MAXLINE);
    result = strncpy(malloc(len+1),buf,len+1);
  }
  return result;
}

typedef struct targ {
  long tid;
  so_t *soptr;
} targ_t;

void *producer(void *arg) {
  so_t *so = arg;
  so_val_t sv;
  int *ret = malloc(sizeof(int));
  FILE *rfile = so->rfile;
  int i;
  int w = 0;
  char *line;
  for (i = 0; (line = readline(rfile)); i++) {
    sv.linenum = i;		/* Build share object valuer */
    sv.line = line;
    sv.tid = 0;
    so_insert(so, &sv);		/* insert it into bounded queue */
    fprintf(stdout, "Prod: [%d] %s", i, line);
  }
  sv.linenum = -1;		/* insert terminal object */
  sv.line = NULL;
  sv.tid = 0;
  so_insert(so, &sv);
  printf("Producer done: %d lines\n", i);
  *ret = i;
  pthread_exit(ret);
}

void *consumer(void *arg) {
  targ_t *targ = (targ_t *) arg;
  long tid = targ->tid;
  so_t *so = targ->soptr;
  int *ret = malloc(sizeof(int));
  int i = 0;;
  int len;
  char *line;
  int w = 0;
  so_val_t sv;
  printf("Con %ld starting\n",tid);
  while (so_remove(so, &sv)) {
    printf("Cons %ld: [%d:%d] %s", tid, i, sv.linenum, sv.line);
    i++;
  }
  printf("Consumer %ld done: %d lines\n", tid, i);
  *ret = i;
  pthread_exit(ret);
}

#define CONSUMERS 4

int main (int argc, char *argv[])
{
  pthread_t prod;
  pthread_t cons[CONSUMERS];
  targ_t carg[CONSUMERS];
  int rc;
  long t;
  int *ret;
  FILE *rfile;
  int i;
  so_t *share;
  
  if (argc < 2) {
    fprintf(stderr,"Usage %s filename",argv[0]);
    exit(0);
  }

  rfile = fopen((char *) argv[1], "r");
  if (!rfile) {
    printf("error opening %s\n",argv[0]);
    exit(0);
  } 
  
  share = new_so(rfile);
  
  pthread_create(&prod, NULL, producer, share);

  for (i=0; i<CONSUMERS; i++) {
    carg[i].tid = i+1;
    carg[i].soptr = share;
    pthread_create(&cons[i], NULL, consumer, &carg[i]);
  }

  printf("main continuing\n");

  rc = pthread_join(prod, (void **) &ret);
  printf("main: producer joined with %d\n", *ret);
  for (i=0; i<CONSUMERS; i++) {
    rc = pthread_join(cons[i], (void **) &ret);
    printf("main: consumer %d joined with %d\n", i, *ret);
  }
  so_close(share);
  
  pthread_exit(NULL);
  exit(0);
}
