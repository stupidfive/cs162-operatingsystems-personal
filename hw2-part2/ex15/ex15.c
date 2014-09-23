#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t signal_cond =PTHREAD_COND_INITIALIZER;
static int data = 0;

static void *consumer(void *o) {
	pthread_mutex_lock(&lock); 
	while (data == 0) {
		pthread_cond_wait(&signal_cond,&lock);
	}
	printf("data: %d", data); 
	pthread_mutex_unlock(&lock); 
	return NULL;
}
static void *producer(void *o) {
	pthread_cond_signal(&signal_cond); 
	data = 1;
	return NULL;
}

int main(int argc, const char **argv)
{
	pthread_t producer_tid , consumer_tid;
	pthread_create(&producer_tid, NULL, &producer, NULL);
	pthread_create(&consumer_tid, NULL, &consumer, NULL);
	pthread_join(consumer_tid, NULL);
}

