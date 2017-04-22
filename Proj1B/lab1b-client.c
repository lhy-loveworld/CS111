#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <strings.h>

struct termios saved_attributes;

void exit_handler (void) {
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

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
  atexit (exit_handler);

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

int main(int argc, char *argv[])
{
  int sockfd, portno, n;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];

  static struct option args[] = {
    {"port", 1, NULL, 'p'},
    {"log", 1, NULL, 'l'},
    {0, 0, 0, 0}
  };

  int log_flag = 0;
  int arg_get;
  int i;
  char *log_path = NULL;
  const char host[] = "localhost";
  
  while ((i = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(i) {
      case 'p': {
        portno = atoi(optarg);
				break;
      }
      case 'l': {
				log_flag = 1;
				log_path = optarg;
				break;
      }
      default: {
      printf("Please enter correct commands as shown below!\n");
      printf("  --port=portnumber ... specify a port\n");
      printf("  --log=filename ... maintain a record of data sent over the socket\n");
      fprintf(stderr, "unrecognized argument\n");
      exit(1);
    	}
    }
  }

  set_input_mode();

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
  bcopy((char *)server->h_addr, 
    (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
    fprintf(stderr, "ERROR connecting: %s\n", strerror(errno));
    exit(1);
  }

  return 0;
}
