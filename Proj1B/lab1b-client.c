//Arthor: Hongyang Li
//This is the source code for project 1B of CS111.
#include <mcrypt.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>

struct termios saved_attributes;
MCRYPT td1, td2;
int log_flag = 0;
int enc_flag = 0;
int log_fd = -1;
int sockfd;

void set_input_mode (void) {
  struct termios tattr;
 
  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (1);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &saved_attributes);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tattr.c_lflag = ISTRIP;
  tattr.c_oflag = 0;
  tattr.c_lflag = 0;
  tcsetattr (STDIN_FILENO, TCSANOW, &tattr);
}

void exithandler(void) {
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
  mcrypt_generic_deinit(td1);
  mcrypt_module_close(td1);
  mcrypt_generic_deinit(td2);
  mcrypt_module_close(td2);
}

void sendtoserver(char *buffer) {
	int len = strlen(buffer);
  if (enc_flag) {
    //len++;
    mcrypt_generic(td1, buffer, len);
  }
  if (log_flag) {
    dprintf(log_fd, "SENT %d bytes: %s\n", len, buffer);
  }
  write(sockfd, buffer, len);
  bzero(buffer, 256);
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  static struct option args[] = {
    {"port", 1, NULL, 'p'},
    {"log", 1, NULL, 'l'},
    {"encrypt", 1, NULL, 'e'},
    {0, 0, 0, 0}
  };

  int key_fd = -1;
  int arg_get;
  const char *host = "localhost";
  
  int portno;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 'p': {
        portno = atoi(optarg);
        break;
      }
      case 'l': {
				log_flag = 1;
        log_fd = creat(optarg, 0666);
        if (log_fd < 0) {
          fprintf(stderr, "creat() error: %s\n", strerror(errno));
          exit(1);
        }
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
        printf("  --log=filename ... maintain a record of data sent over the socket\n");
        printf("  --encrypt=keyfile ... maintain a record of data sent over the socket\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
    	}
    }
  }


  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
  char send_buffer[256];
  bzero(send_buffer, 256);

  set_input_mode();


  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR opening socket: %s\n", strerror(errno));
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
    exit(1);
  }
  server = gethostbyname(host);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
    exit(1);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
    fprintf(stderr, "ERROR connecting: %s\n", strerror(errno));
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
    exit(1);
  }


  char *IV = "ABCDEFGHIJKLMNOP";
  char key[40];
  int keysize = 0;
  bzero(key, 40);

  if (enc_flag) {
    td1 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
    td2 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
    read(key_fd, key, 32);
    keysize = strlen(key);
    mcrypt_generic_init(td1, key, keysize, IV);
    mcrypt_generic_init(td2, key, keysize, IV);
  }
  

  struct pollfd fds[2];
  fds[0].fd = 0;
  fds[1].fd = sockfd;
  fds[0].events = POLLIN | POLLHUP | POLLERR;
  fds[1].events = POLLIN | POLLHUP | POLLERR;
  
  int i = 0;
  int j = 0;
  int read_count = 0;
  
  int ret_poll;
  while (1) {
    ret_poll = poll(fds, 2, 0);
    if (ret_poll == -1) {
      fprintf(stderr, "poll() failed: %s\n", strerror(errno));
      exithandler();
      exit(1);
    } else {
      if (ret_poll > 0) {
        for (i = 0; i < 2; ++i) {
          if (fds[i].revents & POLLIN) {
            bzero(buffer,256);
            read_count = read(fds[i].fd, buffer, 256);
            if (read_count == -1) {
              fprintf(stderr, "read() failed: %s\n", strerror(errno));
              exithandler();
              exit(1);
            } else {
              if (read_count == 0) {
                exithandler();
                exit(0);
              }
              if (i==0) {
                for (j = 0; j < read_count; ++j) {
                  if ((buffer[j] == 0x0D) || (buffer[j] == 0x0A)) {
                    strcat(send_buffer, "\n");
                    write(1, "\r\n", 2);
                    sendtoserver(send_buffer);
                  } else {
                    write(1, buffer + j, 1);
                    strncat(send_buffer, buffer + j, 1);
                    if (buffer[j] < 32) {
                      sendtoserver(send_buffer);
                    }
                  }
                  if (strlen(send_buffer) == 255) {
                    sendtoserver(send_buffer);
                  }
                }
              } else {
                if (log_flag) {
                  dprintf(log_fd, "RECEIVED %d bytes: %s\n", read_count, buffer);
                }
                if (enc_flag) {
                  mdecrypt_generic(td2, buffer, read_count);
                }
                for (j = 0; j < read_count; ++j) {
                  if (buffer[j] == 0x0A) {
                    write(1, "\r\n", 2);
                  } else {
                    write(1, buffer + j, 1);
                  }
                }
              }
            }
          }
          if (fds[i].revents & POLLHUP) {
            exithandler();
            exit(0);
          }
          if (fds[i].revents & POLLERR) {
            exithandler();
            exit(0);
          }
        }
      }
    }
  }
  return 0;
}
