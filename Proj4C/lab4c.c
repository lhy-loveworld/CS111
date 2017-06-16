//Arthor: Hongyang Li
//This is the source module for lab4b of CS111.

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <poll.h>
#include <resolv.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mraa/aio.h"


int log_flag = 0;
int log_fd;
int scale_flag = 0;
int stop_flag = 0;
int sample = -1;
int period = 1;
int tls_flag = 0;
SSL *ssl;
int sockfd;
char* id = "304759850";

const int B = 4275;
const int R0 = 100000;

mraa_aio_context tmp;

SSL_CTX* InitCTX(void)
{   SSL_METHOD const *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();   /* Load cryptos, et.al. */
    SSL_load_error_strings();     /* Bring in and register error messages */
    SSL_library_init();
    method = SSLv23_client_method();    /* Create new client-method instance */
    ctx = SSL_CTX_new(method);      /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void connect_build(char* host, int port) {
	struct sockaddr_in serv_addr;
  struct hostent *server;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) {
    fprintf(stderr, "ERROR opening socket: %s\n", strerror(errno));
    exit(2);
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
    exit(2);
  }

  if (tls_flag) {
    SSL_CTX *ctx;
    ctx = InitCTX();
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) == -1) {
      fprintf(stderr, "SSL_connect() failed: %s\n", strerror(errno));
      exit(2);
    }
    char id_msg[14];
    sprintf(id_msg, "ID=%s\n", id);
    SSL_write(ssl, id_msg, strlen(id_msg));
	} else 
    dprintf(sockfd, "ID=%s\n", id);
}

void Shutdown() {
	time_t rawtime;
  struct tm *info;
  char time_str[9];
  char buffer[20];

	time(&rawtime);
  info = localtime(&rawtime);
  strftime(time_str, 9, "%H:%M:%S", info);
  if (tls_flag) {
    bzero(buffer, 20);
    sprintf(buffer, "%s SHUTDOWN\n", time_str);
    SSL_write(ssl, buffer, strlen(buffer));
  } else
    dprintf(sockfd, "%s SHUTDOWN\n", time_str);
  if (log_flag) {
    dprintf(log_fd, "%s SHUTDOWN\n", time_str);
  }

  mraa_aio_close(tmp);
  exit(0);
}

void Check_tmp() {
	time_t rawtime;
  struct tm *info;
  char time_str[9];
  char buffer[20];

	time(&rawtime);
  info = localtime(&rawtime);

  if ((info->tm_sec == sample) || (sample < 0)){
  	int a = mraa_aio_read(tmp);
  	if (a < 0) {
  		fprintf(stderr, "mraa_aio_read() failed: %s\n", strerror(errno));
      mraa_aio_close(tmp);
      exit(2);
  	}
  	double R = 1023.0 / a - 1.0;
  	R = R0 * R;

  	double tmp_C = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  	strftime(time_str, 9, "%H:%M:%S", info);

  	if (!stop_flag) {
	  	if (!scale_flag) {
	  		printf("%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
        if (tls_flag) {
          bzero(buffer, 20);
          sprintf(buffer, "%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
          SSL_write(ssl, buffer, strlen(buffer));
        } else
          dprintf(sockfd, "%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
	  		if (log_flag)
	  			dprintf(log_fd, "%s %04.1f\n", time_str, tmp_C * 1.8 + 32);
	  	} else {
	  		printf("%s %04.1f\n", time_str, tmp_C);
        if (tls_flag) {
          bzero(buffer, 20);
          sprintf(buffer, "%s %04.1f\n", time_str, tmp_C);
          SSL_write(ssl, buffer, strlen(buffer));
	  		} else
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
  char* host = "lever.cs.ucla.edu";
  int portno = atoi(argv[argc - 1]);

  if (strstr(argv[0], "tls") != NULL)
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
  
  connect_build(host, portno);
    
  tmp = mraa_aio_init(0);
  if (tmp == NULL) {
  	fprintf(stderr, "mraa_aio_init() fail\n");
  	exit(2);
  }

  struct pollfd pfd[1];
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN | POLLERR;

  char buffer[101];
  char *cmd;
  while (1) {
    int ret_poll = poll(pfd, 1, 0);
    if (ret_poll == -1) {
      fprintf(stderr, "poll() failed: %s\n", strerror(errno));
      mraa_aio_close(tmp);
      exit(2);
    } else {
      if (ret_poll == 1) {
        if (pfd[0].revents & POLLIN) {
        	bzero(buffer, 101);
          if (tls_flag)
            SSL_read(ssl, buffer, 100);
          else 
            read(sockfd, buffer, 100);
          cmd = strtok(buffer, "\n");
          while (cmd) {
            if (!strcmp(cmd, "OFF")) {
              if (log_flag) dprintf(log_fd, "%s\n", cmd);
              Shutdown();
            } else {
              if (!strcmp(cmd, "STOP")) {
                if (log_flag) dprintf(log_fd, "%s\n", cmd);
                stop_flag = 1;
              } else {
                if (!strcmp(cmd, "START")) {
                  if (log_flag) dprintf(log_fd, "%s\n", cmd);
                  stop_flag = 0;
                } else {
                  if (!strcmp(cmd, "SCALE=F")) {
                    if (log_flag) dprintf(log_fd, "%s\n", cmd);
                    scale_flag = 0;
                  } else {
                    if (!strcmp(cmd, "SCALE=C")) {
                      if (log_flag) dprintf(log_fd, "%s\n", cmd);
                      scale_flag = 1;
                    } else {
                      if ((!strncmp(cmd, "PERIOD=", 7)) && (cmd[7] < 58) && (cmd[7] > 47)) {
                        if (log_flag) dprintf(log_fd, "%s\n", cmd);
                        period = atoi(cmd + 7);
                      }
                    }
                  }
                }
              }
            }
            cmd = strtok(NULL, "\n");
            Check_tmp();
          }
        }
        if (pfd[0].revents & POLLERR) {
        	fprintf(stderr, "read() failed: %s\n", strerror(errno));
          mraa_aio_close(tmp);
          exit(2);
        }
      } else {
      	Check_tmp();
      }
    }
  }

  return 0;
}