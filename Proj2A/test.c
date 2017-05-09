#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
	struct timespec *t = malloc(sizeof(struct timespec));
	int rc = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
	printf("%ld, %ld\n", /*2017-t->tv_sec/(86400*365)*/t->tv_sec, t->tv_nsec);
	free(t);
	return 0; 
}