#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "SortedList.h"

int iterations = 1;
SortedList_t l = {NULL, NULL, NULL};
int len_max = 10;
int opt_lock = 0;
int opt_yield = 0;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
int lock2 = 0;

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

void Pthread_mutex_lock(pthread_mutex_t *lock) {
	if (pthread_mutex_lock(lock)) {
		fprintf(stderr, "pthread_mutex_lock() failed: %s\n", strerror(errno));
		exit(1);
	}
}

void Pthread_mutex_unlock(pthread_mutex_t *lock) {
	if (pthread_mutex_unlock(lock)) {
		fprintf(stderr, "pthread_mutex_unlock() failed: %s\n", strerror(errno));
		exit(1);
	}
}

void random_keys(char **key) {
	int len = rand() % len_max + 1;
	*key = malloc(sizeof(unsigned char) * (len + 1));
	int i;
	for (i = 0; i < len; ++i) {
		(*key)[i] = 'A' + rand() % 26;
	}
	(*key)[len] = '\0';
}

void *worker(void *elem) {
	int i;
	SortedListElement_t *tmp;
	SortedListElement_t **element = (SortedListElement_t **) elem;
	if (opt_lock) {
		if (opt_lock == 1) {
			pthread_mutex_lock(&lock1);
		} else {
			while (__sync_lock_test_and_set(&lock2, 1));
		}
	}
	for (i = 0; i < iterations; ++i) {
		SortedList_insert(&l, element[i]);
	}
	SortedList_length(&l);
	for (i = 0; i < iterations; ++i) {
		//printf("%s\n", element[i]->key);
		tmp = SortedList_lookup(&l, element[i]->key);
		if ((tmp == NULL) || (SortedList_delete(tmp))) {
			fprintf(stderr, "corrupted list\n");
			exit(2);
		}
		//SortedList_delete(tmp);
		//printf("%d\n", SortedList_length(&l));
	}
	if (opt_lock) {
		if (opt_lock == 1) {
			pthread_mutex_unlock(&lock1);
		} else {
			__sync_lock_release(&lock2);
		}
	}
	return NULL;
}

void sig_handler(int signo) {
  fprintf(stderr, "segmentation fault detected\n");
  exit(2);
}

void Sigaction(int sig, const struct sigaction *act, struct sigaction *oact) {
	if (sigaction(sig, act, oact) == -1) {
    fprintf(stderr, "sigaction() failed: %s\n", strerror(errno));
    exit(1);
  }
}

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 1, NULL, 'y'},
    {"sync", 1, NULL, 's'},
    {0, 0, 0, 0}
  };

  int threads = 1;
  int arg_get;


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
      case 's': {
  			if (optarg[0] == 'm') {
  				opt_lock = 1;
  				break;
  			} else {
  				if (optarg[0] == 's') {
  					opt_lock = 2;
  					break;
  				} else {
  					printf("Please enter correct commands as shown below!\n");
		        printf("  --threads=# ... specify the number of parallel threads\n");
		        printf("  --iterations=# ... specify the number of iterations\n");
		        printf("  --yield=[idl] ... enable (any combination of) optional critical section yields\n");
		        printf("  --sync=m/s ... specify the way to protect a thread\n");
		        fprintf(stderr, "unrecognized argument\n");
		        exit(1);
  				}
  			}
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
        printf("  --sync=m/s ... specify the way to protect a thread\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
    	}
    }
  }


  srand(time(NULL));

  SortedListElement_t *elements[threads][iterations];
  char *rand_keys[threads][iterations];

  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  Sigaction(SIGSEGV, &sa, NULL);

  int i;
  int j;
  for (i = 0; i < threads; ++i) {
  	for (j = 0; j < iterations; ++j) {
  		elements[i][j] = malloc(sizeof(SortedListElement_t));
  		random_keys(&rand_keys[i][j]);
  		elements[i][j]->key = rand_keys[i][j];
  		//free(rand_keys[i][j]);
  		//printf("%s\n", elements[i][j]->key);
  		//
 			//SortedList_insert(&l, elements[i][j]); 	
  	}
  }
  
	pthread_t p[threads];

  struct timespec *t = malloc(sizeof(struct timespec));
	Clock_gettime(CLOCK_MONOTONIC, t);
	long t_beg1 = t->tv_sec;
	long t_beg2 = t->tv_nsec;


	for (i = 0; i < threads; ++i)
  {
  	//random_keys(&(elements[i]));
  	Pthread_create(&(p[i]), NULL, worker, elements[i]);
  }

  for (i = 0; i < threads; ++i)
  {
  	Pthread_join(p[i], NULL);
  }

  Clock_gettime(CLOCK_MONOTONIC, t);
	long t_end1 = t->tv_sec;
	long t_end2 = t->tv_nsec;

	if (SortedList_length(&l)) {
		fprintf(stderr, "not 0 length\n");
		exit(2);
	}
	long t_run = (t_end1 - t_beg1) * 1e9 + t_end2 - t_beg2;
	int oprts = threads * iterations * 3;
	int lists=1;
	char yieldopts[8][5] = {"none", "i", "d", "id", "l", "il", "dl", "idl"}, syncopts[3][5] = {"none", "m", "s"};
	printf("list-%s-%s,%d,%d,%d,%d,%ld,%ld\n", yieldopts[opt_yield], syncopts[opt_lock], threads, iterations, lists, oprts, t_run, t_run / oprts);
	return 0;
}