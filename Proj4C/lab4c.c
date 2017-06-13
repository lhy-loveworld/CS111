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
#include <netinet/in.h>
#include <netdb.h>
#include "mraa/aio.h"


int log_flag = 0;
int log_fd;
int scale_flag = 0;
int stop_flag = 0;
int sample = -1;
int period = 1;
char* id;

const int B = 4275;
const int R0 = 100000;

mraa_aio_context tmp;

int tcp_build(char* host, int port) {
	struct sockaddr_in serv_addr;
  struct hostent *server;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) {
    fprintf(stderr, "ERROR opening socket: %s\n", strerror(errno));
    exit(1);
  }

  server = gethostbyname(host);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(port);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
    fprintf(stderr, "ERROR connecting: %s\n", strerror(errno));
    exit(1);
  }

  dprintf(sockfd, "ID=%s\n", id);
  return sockfd;
}

void Shutdown() {
	time_t rawtime;
  struct tm *info;
  char time_str[9];

	time(&rawtime);
  info = localtime(&rawtime);
  strftime(time_str, 9, "%H:%M:%S", info);
  if (log_flag) {
    dprintf(log_fd, "%s SHUTDOWN\n", time_str);
  }

  mraa_aio_close(tmp);
  exit(0);
}

void Check_tmp(int sockfd) {
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
      exit(1);
  	}
  	double R = 1023.0 / a - 1.0;
  	R = R0 * R;

  	double tmp_C = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  	strftime(time_str, 9, "%H:%M:%S", info);

  	if (!stop_flag) {
	  	if (!scale_flag) {
	  		printf("%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
	  		dprintf(sockfd, "%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
	  		if (log_flag)
	  			dprintf(log_fd, "%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
	  	} else {
	  		printf("%s %04.1f\n", time_str, tmp_C);
	  		dprintf(sockfd, "%s %04.1f\n", time_str, tmp_C);
	  		if (log_flag)
	  			dprintf(log_fd, "%s %04.1f\n", time_str, tmp_C);
	  	}
  	}
  	sample = (info->tm_sec + period) % 60;
  }
}

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"id", 1, NULL, 'i'},
    {"host", 1, NULL, 'h'},
    {"log", 1, NULL, 'l'},
    {0, 0, 0, 0}
  };


  int arg_get;
  char* host;
  int portno = atoi(argv[argc - 1]);
  int tls_flag = 0;

  if (argv[0][7] == 'l')
  	tls_flag = 1;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 'i': {
        id = optarg;
        break;
      }
      case 'l': {
        log_flag = 1;
        log_fd = open(optarg, O_CREAT | O_NONBLOCK | O_APPEND | O_WRONLY, 0666);
        break;
      }
      case 'h': {
        host = optarg;
        break;
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

  int sockfd;
  if (!tls_flag)
  	sockfd = tcp_build(host, portno);
  FILE *sock_str = fdopen(sockfd, "r");
  
  tmp = mraa_aio_init(0);
  if (tmp == NULL) {
  	fprintf(stderr, "mraa_aio_init() fail\n");
  	exit(1);
  }

  struct pollfd pfd[1];
  pfd[0].fd = sockfd;
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
        Check_tmp(sockfd);
        if (pfd[0].revents & POLLIN) {
        	bzero(buffer, 20);
          fgets(buffer, 20, sock_str);
          if (!strcmp(buffer, "OFF\n")) {
          	if (log_flag) dprintf(log_fd, "%s", buffer);
          	Shutdown();
          } else {
          	if (!strcmp(buffer, "STOP\n")) {
          		if (log_flag) dprintf(log_fd, "%s", buffer);
          		stop_flag = 1;
          	} else {
          		if (!strcmp(buffer, "START\n")) {
          			if (log_flag) dprintf(log_fd, "%s", buffer);
          			stop_flag = 0;
          		} else {
          			if (!strcmp(buffer, "SCALE=F\n")) {
          				if (log_flag) dprintf(log_fd, "%s", buffer);
          				scale_flag = 0;
          			} else {
          				if (!strcmp(buffer, "SCALE=C\n")) {
	          				if (log_flag) dprintf(log_fd, "%s", buffer);
	          				scale_flag = 1;
	          			} else {
	          				if ((!strncmp(buffer, "PERIOD=", 7)) && (buffer[7] < 58) && (buffer[7] > 47)) {
		          				if (log_flag) dprintf(log_fd, "%s", buffer);
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
          exit(1);
        }
      } else {
      	Check_tmp(sockfd);
      }
    }
  }

  return 0;
}