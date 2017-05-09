#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "SortedList.h"

int iterations = 1;
SortedList_t l = {NULL, NULL, NULL};

void Clock_gettime(clockid_t clk_id, struct timespec *tp){
	if (clock_gettime(clk_id, tp) < 0) {
		fprintf(stderr, "clock_gettime() failed: %s\n", strerror(errno));
		exit(1);	
	}
}

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {
	if (pthread_create(thread, attr, start_routine, arg)) {
		fprintf(stderr, "pthread_create() failed: %s\n", strerror(errno));
		exit(1);
	}
}

void Pthread_join(pthread_t thread, void **retval) {
	if (pthread_join(thread, retval)) {
		fprintf(stderr, "pthread_join() failed: %s\n", strerror(errno));
		exit(1);
	}
}

void random_keys(char *key) {
	int len = 3;
	key = malloc(sizeof(char) * (len + 1));
	int i;
	for (i = 0; i < len; ++i) {
		key[i] = (char) (rand() % 255 + 1);
	}
	key[len] = '\0';
	//printf("%s\n", key);
}

void *worker(void *elem) {
	int i;
	SortedListElement_t **elements = (SortedListElement_t **) elem;
	for (i = 0; i < iterations; ++i) {
		SortedList_insert(&l, elements[i]);
	}
	return NULL;
}


int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 1, NULL, 'y'},
    {0, 0, 0, 0}
  };

  int threads = 1;
  int arg_get;

  int opt_yield = 0;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 't': {
        threads = atoi(optarg);
        break;
      }
      case 'i': {
				iterations = atoi(optarg);
        break;
      }
      case 'y': {
      	int c = 0;
      	while (optarg[c] != '\0') {
	  			if (optarg[c] == 'i') {
	  				opt_yield |= INSERT_YIELD;
	  			} else {
	  				if (optarg[c] == 'd') {
	  					opt_yield |= DELETE_YIELD;
	  				} else {
	  					if (optarg[c] == 'l') {
	  						opt_yield |= LOOKUP_YIELD;
	  					} else {
	  						break;
	  					}
	  				}
	  			}
	  			c++;
	  		}
	  		if (optarg[c] == '\0') break;
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
        printf("  --threads=# ... specify the number of parallel threads\n");
        printf("  --iterations=# ... specify the number of iterations\n");
        printf("  --yield=[idl] ... enable (any combination of) optional critical section yields\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
    	}
    }
  }


  srand(time(NULL));

  SortedListElement_t *elements[threads][iterations];
  char *rand_keys[threads][iterations];

  int i;
  int j;
  for (i = 0; i < threads; ++i) {
  	for (j = 0; j < iterations; ++j) {
  		random_keys(rand_keys[i][j]);
  		elements[i][j] = malloc(sizeof(SortedListElement_t));
  		elements[i][j]->key = rand_keys[i][j];
  		printf("%s\n", rand_keys[i][j]);
  		//free(rand_keys[i][j]);
 			//SortedList_insert(&l, elements[i][j]); 	
  	}
  }
  
  struct timespec *t = malloc(sizeof(struct timespec));
	Clock_gettime(CLOCK_MONOTONIC, t);
	long t_beg1 = t->tv_sec;
	long t_beg2 = t->tv_nsec;

	pthread_t p[threads];

	for (i = 0; i < threads; ++i)
  {
  	//Pthread_create(&(p[i]), NULL, worker, elements[i]);
  }

	return 0;
}