#include "so0.h"

so_t *new_so(FILE *rfile) {
  so_t *share = malloc(sizeof(so_t));
  so_init(share, rfile);
  return share;
}

int so_init(so_t *so, FILE *rfile) {
  so->rfile = rfile;
  so->line = NULL;
  so->flag = SO_EMPTY;		/* initially empty */
  pthread_mutex_init(&so->solock, NULL); /* unlocked */
  pthread_cond_init(&so->flag_cv, NULL); /* nobody waiting */
  return 1;
}

int so_waittill(so_t *so, int val) {
  /* 
     Obtain exclusive access to shared object once it obtains the specified state.

     The intended use of the object is provide a concurrent buffer with an
     alternating protocol:
     * Threads fill the buffer by waittill EMPTY, setting the fields, and release FULL.
     * Threads empty the buffer by waittill FULL, reading the fields, and release EMPTY.

     Invariant: each fill will be emptied exactly once until termination.

     On return, the calling thread holds the object lock and can access the fields
     to effect the protocol.
*/
  pthread_mutex_lock(&so->solock); /* take the lock before attempting to enter state */
  while (so->flag != val) {
    pthread_cond_wait(&so->flag_cv, &so->solock); /* release lock and regain upon signal */
  }
  return 1;
}

int so_release(so_t *so, int val) {
  /* Release exclusive access previously obtained and transition to next protocol state */
  so->flag = val;		/* update the state */
  pthread_cond_signal(&so->flag_cv); /* wake up those waiting on it */
  return pthread_mutex_unlock(&so->solock); /* release lcok */
}

int so_release_exit(so_t *so) {
  /* Release lock and wake up waiters, but do not advance state of the object

     The exception to the alternating protocol is termination in which all consumers need
     to see that the object if FULL of a NULL line signifiying end of the file.
 */
  pthread_cond_signal(&so->flag_cv);
  return pthread_mutex_unlock(&so->solock);
}

int so_close(so_t *so) {
  /* 
     When it can be guaranteed that all threads accessing the object have taken final action:
     * Producer as filled it with the termination condition
     * All Consumers have release_exit after observing the termination condition.
     Clear all resources associated with the object.
   */
  so->flag = 0;
  pthread_mutex_destroy(&so->solock);
  pthread_cond_destroy(&so->flag_cv);
  return 1;
}
