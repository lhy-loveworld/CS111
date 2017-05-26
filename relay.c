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

mraa_gpio_context rel;


int main(int argc, char *argv[]) {



  rel = mraa_gpio_init(3);

  if (rel == NULL) {
  	fprintf(stderr, "mraa_gpio_init() fail\n");
  	exit(1);
  }

  mraa_gpio_dir(rel, MRAA_GPIO_OUT);

  struct pollfd pfd[1];
  pfd[0].fd = 0;
  pfd[0].events = POLLIN | POLLERR;

  char buffer[20];

  while (1) {
    int ret_poll = poll(pfd, 1, 0);
    if (ret_poll == -1) {
      fprintf(stderr, "poll() failed: %s\n", strerror(errno));
      mraa_gpio_close(rel);
      exit(1);
    } else {
      if (ret_poll == 1) {
      	if (pfd[0].revents & POLLIN) {
        	bzero(buffer, 20);
          fgets(buffer, 20, stdin);
          if (!strcmp(buffer, "OFF\n")) {
          	mraa_gpio_write(rel, 0);
          } else {
            mraa_gpio_write(rel, 1);
          }
        }
        if (pfd[0].revents & POLLERR) {
        	fprintf(stderr, "read() failed: %s\n", strerror(errno));
          mraa_gpio_close(rel);
          exit(1);
        }
      }
    }
  }

  return 0;
}