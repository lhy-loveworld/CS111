//Arthor: Hongyang Li
//This is the source code for project 1B of CS111.

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <poll.h>
#include <mcrypt.h>
#include <fcntl.h>

int rc = 0;
MCRYPT td1, td2;
int enc_flag = 0;

void harvest(void) {
  int status;
  if (waitpid(rc, &status, 0) == -1) {
    fprintf(stderr, "waitpid() failed: %s\n", strerror(errno));
  }
  mcrypt_generic_deinit(td1);
  mcrypt_module_close(td1);
  mcrypt_generic_deinit(td2);
  mcrypt_module_close(td2);  
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
}

void pipe_handler(int signo) {
  harvest();
  exit(0);
}



int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t clilen;

  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    fprintf(stderr, "ERROR opening socket: %s\n", strerror(errno));
    exit(1);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));

  static struct option args[] = {
    {"port", 1, NULL, 'p'},
    {"encrypt", 1, NULL, 'e'},
    {0, 0, 0, 0}
  };
  
  int arg_get;
  int key_fd = -1;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 'p': {
        portno = atoi(optarg);
        break;
      }
      case 'e': {
        enc_flag = 1;
        key_fd = open(optarg, O_RDONLY);
        break;
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
        printf("  --port=portnumber ... specify a port\n");
        printf("  --encrypt=keyfile ... maintain a record of data sent over the socket\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
      }
    }
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "ERROR on binding: %s\n", strerror(errno));
    exit(1);
  }
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) {
    fprintf(stderr, "ERROR on accept: %s\n", strerror(errno));
    exit(1);
  }
  bzero(buffer,256);

  int ptoc_fd[2], ctop_fd[2];
  if ((pipe(ptoc_fd) < 0) || (pipe(ctop_fd) < 0)) {
    fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
    exit(1);
  }

  rc = fork();
  if (rc < 0) { // fork failed; exit
    fprintf(stderr, "fork() failed: %s\n", strerror(errno));
    exit(1);
  } else {
    if (rc == 0) { // child (new process)
      close(ptoc_fd[1]);
      close(ctop_fd[0]);
      dup2(ptoc_fd[0], 0);
      dup2(ctop_fd[1], 1);
      dup2(ctop_fd[1], 2);
      close(ptoc_fd[0]);
      close(ctop_fd[1]);
      char *child_args[2];
      child_args[0] = strdup("/bin/bash");
      child_args[1] = NULL;
      if (execvp(child_args[0], child_args) == -1) {
        fprintf(stderr, "execvp() failed: %s\n", strerror(errno));
        exit(1);
      }
    } else { // parent goes down this path (main)
      struct sigaction sa;

      char *IV = "ABCDEFGHIJKLMNOP";
      char key[16];
      int keysize = 16;
      bzero(key, keysize);
        
      if (enc_flag) {
        td1 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        td2 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        read(key_fd, key, 16);
        mcrypt_generic_init(td1, key, keysize, IV);
        mcrypt_generic_init(td2, key, keysize, IV);
      }      

      sa.sa_handler = pipe_handler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = 0;
      if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        fprintf(stderr, "sigaction() failed: %s\n", strerror(errno));
        harvest();
        exit(1);
      }
      close(ptoc_fd[0]);
      close(ctop_fd[1]);
      struct pollfd fds[2];
      int ret_poll;
      int i = 0;
      int j = 0;
      int read_count = 0;
      fds[0].fd = newsockfd;
      fds[1].fd = ctop_fd[0];
      fds[0].events = POLLIN | POLLHUP | POLLERR;
      fds[1].events = POLLIN | POLLHUP | POLLERR;
      while (1) {
        ret_poll = poll(fds, 2, 0);
        if (ret_poll == -1) {
          fprintf(stderr, "poll() failed: %s\n", strerror(errno));
          harvest();
          exit(1);
        } else {
          if (ret_poll > 0) {
            for (i = 0; i < 2; ++i) {
              if (fds[i].revents & POLLIN) {
                bzero(buffer, 256);
                read_count = read(fds[i].fd, buffer, 256);
                if (read_count == -1) {
                  fprintf(stderr, "read() failed: %s\n", strerror(errno));
                  harvest();
                  exit(1);
                } else {
                  if ((i == 0) && (read_count == 0)) {
                    kill(rc, SIGTERM);
                  } else {
                    if (i == 1) {
                      if (enc_flag) {
                        mcrypt_generic(td1, buffer, 256);
                      }
                      write(newsockfd, buffer, 256);
                    } else {
                      if (enc_flag) {
                        mdecrypt_generic(td2, buffer, 256);
                      }
                      for (j = 0; j < strlen(buffer); ++j) {
                        switch (buffer[j]) {
                          case 0x04:
                            close(ptoc_fd[1]);
                            break;
                          case 0x03:
                            kill(rc, SIGINT);
                            break;
                          default:
                            write(ptoc_fd[1], buffer + j, 1);
                        }
                      }
                    }
                  }
                }
              }
              if (fds[i].revents & POLLHUP) {
                if (i == 1) {
                  close(ctop_fd[0]);
                  harvest();
                  exit(0);
                }
              }
              if (fds[i].revents & POLLERR) {
                harvest();
                exit(0);
              }
            }
          }
        }
      }
    }
  }
  return 0;
}