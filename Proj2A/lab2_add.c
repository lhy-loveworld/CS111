#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <sched.h>

long long counter;
int opt_yield = 0;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
int lock2 = 0;
int opt_lock = 0;

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

void add(long long *pointer, long long value) {
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
}

void add1(long long *pointer, long long value) {
	Pthread_mutex_lock(&lock1);
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
	Pthread_mutex_unlock(&lock1);
}

void add2(long long *pointer, long long value) {
	while (__sync_lock_test_and_set(&lock2, 1));
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
	__sync_lock_release(&lock2);
}

void add3(long long *pointer, long long value) {
	long long old, new;
	do {
		old = *pointer;
		new = old + value;
		if (opt_yield)
			sched_yield();
	} while (__sync_val_compare_and_swap(pointer, old, new) != old);
}

void *addminus(void *it) {
	int i, j;
	int *ite = (int *)it;
	for (i = 0; i < *ite; ++i)
	{
		switch(opt_lock) {
			case 0: {
				add(&counter, 1);
				break;
			}
			case 1: {
				add1(&counter, 1);
				break;
			}
			case 2: {
				add2(&counter, 1);
				break;
			}
			case 3: {
				add3(&counter, 1);
				break;
			}
		}
	}
	for (j = 0; j < *ite; ++j)
	{
		switch(opt_lock) {
			case 0: {
				add(&counter, -1);
				break;
			}
			case 1: {
				add1(&counter, -1);
				break;
			}
			case 2: {
				add2(&counter, -1);
				break;
			}
			case 3: {
				add3(&counter, -1);
				break;
			}
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	counter = 0;

	static struct option args[] = {
    {"threads", 1, NULL, 't'},
    {"iterations", 1, NULL, 'i'},
    {"yield", 0, NULL, 'y'},
    {"sync", 1, NULL, 's'},
    {0, 0, 0, 0}
  };

  int threads = 1;
  int iterations = 1;
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
      case 'y': {
  			opt_yield = 1;      
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
  					if (optarg[0] == 'c') {
  						opt_lock = 3;
  						break;
  					} else;
  				}
  			}
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
        printf("  --threads=# ... specify the number of parallel threads\n");
        printf("  --iterations=# ... specify the number of iterations\n");
        printf("  --yield ... cause the thread to a immediate yield\n");
        printf("  --sync=m/c/s ... specify the way to protect a thread\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
    	}
    }
  }

  int i;
  pthread_t p[threads];

	struct timespec *t = malloc(sizeof(struct timespec));
	Clock_gettime(CLOCK_MONOTONIC, t);
	long t_beg1 = t->tv_sec;
	long t_beg2 = t->tv_nsec;

  for (i = 0; i < threads; ++i)
  {
  	Pthread_create(&(p[i]), NULL, addminus, &iterations);
  }

  int j;
  for (j = 0; j < threads; ++j)
  {
  	Pthread_join(p[j], NULL);
  }

  Clock_gettime(CLOCK_MONOTONIC, t);
	long t_end1 = t->tv_sec;
	long t_end2 = t->tv_nsec;
	long t_run = (t_end1 - t_beg1) * 1e9 + t_end2 - t_beg2;
	int oprts = threads*iterations*2;
	char test1[2][7] = {"", "yield-"}, test2[4][5] = {"none", "m", "s", "c"};
	
	printf("add-%s%s,%d,%d,%d,%ld,%ld,%lld\n", test1[opt_yield], test2[opt_lock], threads, iterations, oprts, t_run, t_run / oprts, counter);
  
  free(t);
  exit(0);
  return 0;
}