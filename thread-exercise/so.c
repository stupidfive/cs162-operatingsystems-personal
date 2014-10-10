#include "so.h"

so_t *new_so(FILE *rfile) {
  so_t *share = malloc(sizeof(so_t));
  so_init(share, rfile);
  return share;
}

int so_init(so_t *so, FILE *rfile) {
  int i;
  so->rfile = rfile;
  so->front = so->nextempty = 0;
  for (i = 0; i<SO_DEPTH; i++) {
    so->queue[i].line = NULL;
    so->queue[i].linenum = -1;
  }
  pthread_mutex_init(&so->solock, NULL); /* initially unlocked and empty */
  return 1;
}

void so_insert(so_t *so, so_val_t *val) {
  while (1) {
    pthread_mutex_lock(&so->solock); /* take the lock before attempting to update state */
#ifdef DEBUG
    printf("insert [%d] %s\n", val->tid, val->line);
#endif
    if ((so->nextempty - so->front) < SO_DEPTH) {
#ifdef DEBUG
      printf("Ins: [%d:%d] %d\n",  so->front,so->nextempty, so->nextempty % SO_DEPTH);
#endif
      so->queue[so->nextempty % SO_DEPTH] = *val; /* fill next empty */
      so->nextempty++;		/* advance the empty pointer */
      pthread_mutex_unlock(&so->solock); 
      return;
    }
    pthread_mutex_unlock(&so->solock); /* release a try again */
  }
}

char *so_remove(so_t *so, so_val_t *val) {
  while (1) {
    pthread_mutex_lock(&so->solock); /* take the lock before attempting to enter state */
#ifdef DEBUG
    printf("Remove [%d:%d] %d\n",  so->front,so->nextempty, so->front % SO_DEPTH);
#endif
    if (so->nextempty > so->front) {
      *val = so->queue[so->front % SO_DEPTH]; /* copy object from the queue */
#ifdef DEBUG
      printf("Rem: [%d:%d]%d %s\n",so->front,so->nextempty, so->front % SO_DEPTH, val->line);
#endif
      if (val->line) so->front++;	      /* remove only if not terminal NULL */
      pthread_mutex_unlock(&so->solock); 
      return val->line;
    }
    pthread_mutex_unlock(&so->solock); 
  }
}

int so_close(so_t *so) {
  /* 
     When it can be guaranteed that all threads accessing the object have taken final action:
     * Producer as filled it with the termination condition
     * All Consumers have release_exit after observing the termination condition.
     Clear all resources associated with the object.
   */
  pthread_mutex_destroy(&so->solock);
  return 1;
}
