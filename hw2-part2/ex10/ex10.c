#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>


#define N_THREADS 4

static void *worker(void *o) {
	int mytid = *((int*)o);
	printf("Worker %d started\n", mytid);
	return NULL;
}

int main(int argc, const char **argv) {
	pthread_t tids[N_THREADS];
	for (int i = 0; i < N_THREADS; ++i) {
		int *ip = malloc(sizeof(*ip));
		*ip = i;
		pthread_create(&tids[i], NULL, &worker, ip);
	}
	for (int i = 0; i < N_THREADS; ++i) {
		pthread_join(tids[i], NULL);
	}
}


