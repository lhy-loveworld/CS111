//Arthor: Hongyang Li
//This is the source module for lab4b of CS111.

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <math.h>
#include "mraa/aio.h"

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"period", 1, NULL, 'p'},
    {"scale", 1, NULL, 's'},
    {"log", 1, NULL, 'l'},
    {0, 0, 0, 0}
  };

  int log_flag = 0;
  int log_fd;
  int scale_flag = 0;
  int period = 1;
  int arg_get;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 'p': {
        period = atoi(optarg);
        break;
      }
      case 'l': {
        log_flag = 1;
        log_fd = open(optarg, O_CREAT | O_NONBLOCK | O_APPEND | O_WRONLY, 0666);
        break;
      }
      case 's': {
        if (!strcmp(optarg, "C")) {
        	scale_flag = 1;
        	break;
        } else {
        	if (!strcmp(optarg, "F"))
        		break;
        }
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
        printf("  --period=# ... specify a sampling interval in seconds\n");
        printf("  --scale=C/F ... temperatures reported in Celsius or Fahrenheit\n");
        printf("  --log=pathname ... append report to a logfile\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
      }
    }
  }

  const int B = 4275;
  const int R0 = 100000;
  mraa_aio_context tmp;
  if (log_flag) dprintf(log_fd, "abcdefg\n");
  return 0;
}