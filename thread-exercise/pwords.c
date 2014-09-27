#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

pthread_mutex_t mm_mutex = PTHREAD_MUTEX_INITIALIZER;
void *mymalloc(size_t size) {
  void *r;
  pthread_mutex_lock(&mm_mutex);
  r = malloc(size);
  pthread_mutex_unlock(&mm_mutex);
  return r;
}

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

typedef struct syncdict {
  pthread_mutex_t lock;
  dict_t *wd;
} syncdict_t;

syncdict_t *new_syncdict() {
  syncdict_t *r = mymalloc(sizeof(syncdict_t));
  pthread_mutex_init(&r->lock, NULL);
  r->wd = NULL;
  return r;
}

typedef struct targ {
  long tid;
  FILE *infile;
  syncdict_t *swd;
} targ_t;

char *make_word(char *word) {
  return strcpy(mymalloc(strlen(word)+1),word);
}

dict_t *make_dict(char *word) {
  dict_t *nd = (dict_t *) mymalloc(sizeof(dict_t));
  nd->word = make_word(word);
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

void insert_word(syncdict_t *sd, char *word) {
  /* 
     Insert word into dict or increment count if already there
     return pointer to the updated dict
 */
  dict_t *nd;
  dict_t *pd = NULL;		/* prior to insertion point */
  dict_t *di;			/* following insertion point */

  pthread_mutex_lock(&sd->lock);
  di = sd->wd;
  /* Search down list to find if present or point of insertion */
  while (di && (strcmp(word, di->word) >= 0)) { 
    if (strcmp(word,di->word) == 0) { /* found word */
      di->count++;		/* increment count */
      pthread_mutex_unlock(&sd->lock);
      return;			/* head remains unchanged */
    }
    pd = di;			/* advance ptr pair */
    di = di->next;
  }
  nd = make_dict(word);		/* not found, make entry */
  nd->next = di;		/* entry bigger than word or tail */
  if (pd) {
    pd->next = nd;
    pthread_mutex_unlock(&sd->lock);
    return;			/* insert beond head */
  }
  sd->wd = nd;			/* update head */
  pthread_mutex_unlock(&sd->lock);
  return;
}

void print_dict(dict_t *d) {
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

/* Lock to protect non-threadsafe file i/o */
pthread_mutex_t gw_mutex = PTHREAD_MUTEX_INITIALIZER;
int get_word(char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  pthread_mutex_lock(&gw_mutex);
  while ((c = fgetc(infile)) != EOF) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	/* terminate the word string */
      pthread_mutex_unlock(&gw_mutex);
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  pthread_mutex_unlock(&gw_mutex);
  return 0;			/* no more words */
}


#define MAXWORD 1024
void *words(void *t) {
  targ_t *tp = (targ_t *) t;
  long tid = tp->tid;
  FILE *infile = tp->infile;
  syncdict_t *sd = tp->swd;
  int *rtnval = mymalloc(sizeof(int));
  char wordbuf[MAXWORD];
  printf("Words %ld\n", tid);
  while (get_word(wordbuf,MAXWORD, infile)) {
    printf("T%ld: got %s\n",tid,wordbuf);
    insert_word(sd, wordbuf); /* add to dict */
  }
  pthread_exit(rtnval);
}

#define NTHREADS 4
int main (int argc, char *argv[]) {
  long tid;
  int nthreads = NTHREADS;
  pthread_t threads[NTHREADS];
  targ_t targs[NTHREADS];
  FILE *infile = stdin;
  int *ret;

  syncdict_t *swd = new_syncdict();

  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if (!infile) {
    printf("Unable to open %s\n",argv[1]);
    exit(1);
  }

  for (tid=0; tid < nthreads; tid++) {
    targs[tid].tid = tid;
    targs[tid].infile = infile;
    targs[tid].swd = swd;
    pthread_create(&threads[tid], NULL, words, (void *) &targs[tid]);
    printf("created thread %ld\n", tid);
  }
  for (tid=0; tid < nthreads; tid++) {
    pthread_join(threads[tid], (void **) &ret);
    printf("joined thread %ld (%d)\n", tid, *ret);
  }
  print_dict(swd->wd);
  fclose(infile);
  pthread_exit(NULL);
  return 0;
}

