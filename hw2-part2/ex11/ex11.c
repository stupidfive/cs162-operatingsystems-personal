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

struct args {
	pthread_t tid;
	int tindex; 
};

static void *worker(void *o){
	int mytid = ((struct args *)o)->tindex; 
	printf("Worker %d started\n", mytid); 
	return NULL;
}

int main(int argc, const char **argv) {
	struct args args[N_THREADS];
	for (int i = 0; i < N_THREADS; ++i) {
		args[i].tindex = i;
		pthread_create(&args[i].tid, NULL, &worker, &args[i]); 
	}
	for (int i = 0; i < N_THREADS; ++i) {
		pthread_join(args[i].tid, NULL);
	}
}