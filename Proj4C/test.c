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
#include <openssl/applink.c>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

char* id = "123456777";

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

int main(int argc, char *argv[]) {

	char* host = "lever.cs.ucla.edu";
	int portno = 19000;

	SSL_CTX *ctx;
 	SSL *ssl;
 	int sockfd = tcp_build(host, portno);

  return 0;
}