#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
void sig_handler(int signo) {
  fprintf(stderr, "segmentation fault detected%d\n", pthread_self());
 // exit(2);
}

void *worker(void *elem) {
	printf("child%d\n", pthread_self());
	int *i = NULL;
	*i = 99;
	return NULL;
}
int main (int argc, char *argv[]){
	  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGSEGV, &sa, NULL);
	struct timespec *t = malloc(sizeof(struct timespec));
	int rc = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
//	printf("%ld, %ld\n", /*2017-t->tv_sec/(86400*365)*/t->tv_sec, t->tv_nsec);
	printf("%d\n", pthread_self());

	pthread_t p;
	pthread_create(&p, NULL, worker, 1);
	pthread_join(p, NULL);
	free(t);

	printf("%d\n", pthread_self());
	return 0; 
}