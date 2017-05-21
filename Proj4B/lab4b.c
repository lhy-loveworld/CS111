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

int log_flag = 0;
int log_fd;
int scale_flag = 0;
int stop_flag = 0;
int period = 1;
int sample = -1;

const int B = 4275;
const int R0 = 100000;

mraa_aio_context tmp;
mraa_gpio_context btn;

void Shutdown() {
	time_t rawtime;
  struct tm *info;
  char time_str[9];

	time(&rawtime);
  info = localtime(&rawtime);
  strftime(time_str, 9, "%H:%M:%S", info);
  if (log_flag) {
    dprintf(log_fd, "%s SHUTDOWN\n", time_str);
  }/* else {
    printf("%s SHUTDOWN\n", time_str);
  }*/
  mraa_aio_close(tmp);
  mraa_gpio_close(btn);
  exit(0);
}

void Check_btn() {
	int btn_status = mraa_gpio_read(btn);
  if (btn_status > 0) {
    Shutdown();
  } else {
    if (btn_status < 0) {
      fprintf(stderr, "mraa_gpio_read() failed: %s\n", strerror(errno));
      mraa_aio_close(tmp);
      mraa_gpio_close(btn);
      exit(1);
    }
  }
}

void Check_tmp() {
	time_t rawtime;
  struct tm *info;
  char time_str[9];

	time(&rawtime);
  info = localtime(&rawtime);

  if ((info->tm_sec == sample) || (sample < 0)){
  	int a = mraa_aio_read(tmp);
  	if (a < 0) {
  		fprintf(stderr, "mraa_aio_read() failed: %s\n", strerror(errno));
      mraa_aio_close(tmp);
      mraa_gpio_close(btn);
      exit(1);
  	}
  	double R = 1023.0 / a - 1.0;
  	R = R0 * R;

  	double tmp_C = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  	strftime(time_str, 9, "%H:%M:%S", info);

  	if (!stop_flag) {
	  	if (!scale_flag) {
	  		printf("%s %f\n", time_str, tmp_C * 1.8 + 32);
	  		if (log_flag)
	  			dprintf(log_fd, "%s %f\n", time_str, tmp_C * 1.8 + 32);
	  	} else {
	  		printf("%s %f\n", time_str, tmp_C);
	  		if (log_flag)
	  			dprintf(log_fd, "%s %f\n", time_str, tmp_C);
	  	}
  	}
  	sample = (info->tm_sec + period) % 60;
  }
}

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"period", 1, NULL, 'p'},
    {"scale", 1, NULL, 's'},
    {"log", 1, NULL, 'l'},
    {0, 0, 0, 0}
  };


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


  tmp = mraa_aio_init(0);
  btn = mraa_gpio_init(3);
  if (tmp == NULL) {
  	fprintf(stderr, "mraa_aio_init() fail\n");
  	exit(1);
  }
  if (btn == NULL) {
  	fprintf(stderr, "mraa_gpio_init() fail\n");
  	mraa_aio_close(tmp);
  	exit(1);
  }

  mraa_gpio_dir(btn, MRAA_GPIO_IN);

  struct pollfd pfd[1];
  pfd[0].fd = 0;
  pfd[0].events = POLLIN | POLLERR;

  char buffer[20];

  while (1) {
    int ret_poll = poll(pfd, 1, 0);
    if (ret_poll == -1) {
      fprintf(stderr, "poll() failed: %s\n", strerror(errno));
      mraa_aio_close(tmp);
      exit(1);
    } else {
      if (ret_poll == 1) {
      	Check_btn();
        if (pfd[0].revents & POLLIN) {
        	bzero(buffer, 20);
          fgets(buffer, 20, stdin);
          if (!strcmp(buffer, "OFF\n")) {
          	dprintf(log_fd, "%s", buffer);
          	Shutdown();
          } else {
          	if (!strcmp(buffer, "STOP\n")) {
          		dprintf(log_fd, "%s", buffer);
          		stop_flag = 1;
          	} else {
          		if (!strcmp(buffer, "START\n")) {
          			dprintf(log_fd, "%s", buffer);
          			stop_flag = 0;
          		} else {
          			if (!strcmp(buffer, "SCALE=F\n")) {
          				dprintf(log_fd, "%s", buffer);
          				scale_flag = 0;
          			} else {
          				if (!strcmp(buffer, "SCALE=C\n")) {
	          				dprintf(log_fd, "%s", buffer);
	          				scale_flag = 1;
	          			} else {
	          				if ((!strncmp(buffer, "PERIOD=", 7)) && (buffer[7] < 58) && (buffer[7] > 47)) {
		          				dprintf(log_fd, "%s", buffer);
		          				period = atoi(buffer + 7);
		          			}
	          			}
          			}
          		}
          	}
          }
        }
        if (pfd[0].revents & POLLERR) {
        	fprintf(stderr, "read() failed: %s\n", strerror(errno));
          mraa_aio_close(tmp);
          mraa_gpio_close(btn);
          exit(1);
        }
      } else {
      	Check_btn();
      	Check_tmp();
      }
    }
  }

  return 0;
}