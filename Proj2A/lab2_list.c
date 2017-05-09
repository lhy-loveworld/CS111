#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include "SortedList.h"

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
		*(key + i) = (unsigned char) (rand() % 255 + 1);
	}
	*(key + len) = '\0';
}

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 1, NULL, 'y'},
    {0, 0, 0, 0}
  };

  int threads = 1;
  int iterations = 1;
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

  SortedList_t l = {NULL, NULL, NULL};

  srand(time(NULL));

  SortedListElement_t *elements[iterations * threads];
  char *rand_keys[iterations * threads];

  int i;
  for (i = 0; i < (threads * iterations); ++i) {
  	random_keys(rand_keys[i]);
  	elements[i] = malloc(sizeof(SortedListElement_t));
  	elements[i]->key = rand_keys[i];
  	//SortedList_insert(&l, elements[i]);
  }
  
  struct timespec *t = malloc(sizeof(struct timespec));
	Clock_gettime(CLOCK_MONOTONIC, t);
	long t_beg1 = t->tv_sec;
	long t_beg2 = t->tv_nsec;

	return 0;
}