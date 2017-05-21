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
mraa_aio_context snd;
int status = 0;

int main(int argc, char *argv[]) {


  snd = mraa_aio_init(0);
  rel = mraa_gpio_init(3);
  

  if (snd == NULL) {
    fprintf(stderr, "mraa_aio_init() fail\n");
    exit(1);
  }
  if (rel == NULL) {
    fprintf(stderr, "mraa_gpio_init() fail\n");
    mraa_aio_close(snd);
    exit(1);
  }

  mraa_gpio_dir(rel, MRAA_GPIO_OUT);

  struct pollfd pfd[1];
  pfd[0].fd = 0;
  pfd[0].events = POLLIN | POLLERR;

  char buffer[20];
  mraa_gpio_write(rel, 0);
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
            status = 0;
          } else {
            mraa_gpio_write(rel, 1);
            status = 1;
          }
        }
        if (pfd[0].revents & POLLERR) {
        	fprintf(stderr, "read() failed: %s\n", strerror(errno));
          mraa_gpio_close(rel);
          mraa_aio_close(snd);
          exit(1);
        }
      } else {
        int a = mraa_aio_read(snd);
        if (a < 0) {
          fprintf(stderr, "mraa_aio_read() failed: %s\n", strerror(errno));
          mraa_aio_close(snd);
          mraa_gpio_close(rel);
          exit(1);
        }
        if (a > 400) {
          status = 1 - status;
          mraa_gpio_write(rel, status);
	  sleep(2);
        }
      }
    }
  }

  return 0;
}
