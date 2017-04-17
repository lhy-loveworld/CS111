//Arthor: Hongyang Li
//This is the source module for lab1a of CS111.

#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

/*
void read_buf(int, char *);
void write_buf(int, char *, int);
*/
struct termios saved_attributes;

void reset_input_mode (void) {
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void) {
  struct termios tattr;
  char *name;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (1);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &saved_attributes);
  atexit (reset_input_mode);

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

int main(int argc, char *argv[]) {
  static struct option args[] = {
    {"shell", 0, NULL, 's'},
    {0, 0, 0, 0}
  };
  int shell_flag = 0;
  int arg_get;
  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    if (arg_get == 's') {
      shell_flag = 1;
    } else {
      printf("Please enter correct commands as shown below!\n");
      printf("  --shell ... pass input/output between the terminal and a shell\n");
      exit(1);
    }
  }

  set_input_mode();

  if (!shell_flag) {
    char buf[1024];
    int read_count = 0;
    int i = 0;
    while (1) {
      read_count = read(0, buf, 1024);
      if (read_count == -1) {
        perror("Can not read from stdin");
        exit(1);
      } else {
        for (i = 0; i < read_count; ++i) {
          switch (buf[i]) {
            case 0x0D:
            case 0x0A:
              write(1, "\r\n", 2);
              break;
            case 0x04:
              exit(0);
            default:
              write(1, buf + i, 1);
          }
        }
      }
    }
   
  } else {
    int pfd1[2], pfd2[2];
    if (pipe(pfd1) < 0) {
      perror("Creating pipe failed");
      exit(1);
    }
    if (pipe(pfd2) < 0) {
      perror("Creating pipe failed");
      exit(1);
    }
    int rc = fork();
    if (rc < 0) { // fork failed; exit
      perror("Fork failed\n");
      exit(1);
    } else {
      if (rc == 0) { // child (new process)
        close(pfd1[1]);
        close(pfd2[0]);
        dup2(pfd1[0], 0);
	      dup2(pfd2[1], 1);
        close(pfd1[0]);
        close(pfd2[1]);
        char *child_args[2];
	      child_args[0] = strdup("/bin/bash");
	      child_args[1] = NULL;
	      if (execvp(child_args[0], child_args) == -1) {
          perror("execvp() failed!\n");
          exit(1);
        }
      } else { // parent goes down this path (main)
        close(pfd1[0]);
        close(pfd2[1]);
        char buf[1024];
        struct pollfd fds[2];
        int ret_poll;
        int i = 0;
        int j = 0;
        int read_count = 0;
        fds[0].fd = 0;
        fds[1].fd = pfd2[0];
        fds[0].events = POLLIN | POLLHUP | POLLERR;
        fds[1].events = POLLIN | POLLHUP | POLLERR;
        while (1) {
          ret_poll = poll(fds, 2, 0);
          if (ret_poll == -1) {
            perror("poll() failed\n");
            exit(1);
          } else {
            if (ret_poll > 0) {
              for (i = 0; i < 2; ++i) {
                if (fds[i].revents & POLLIN) {
                  read_count = read(fds[i].fd, buf, 1024);
                  if (read_count == -1) {
                    perror("read() failed\n");
                    exit(1);
                  } else {
                    for (j = 0; j < read_count; ++j) {
                      switch (buf[j]) {
                        case 0x0D:
                          if (i == 0) {
                            write(1, "\r\n", 2);
                            write(pfd1[1], "\n", 1);
                          } else {
                            write(1, "\r\n", 1);
                          }
                          break;
                        case 0x0A:
                          if (i == 0) {
                            write(1, "\r\n", 2);
                            write(pfd1[1], "\n", 1);
                          } else {
                            write(1, buf + i, 2);
                          }
                          break;
                        case 0x03:
                          kill(rc, SIGINT);
                          exit(0);
                        default:
                          write(1, buf + i, 1);
                          if (i == 0) {
                            write(pfd1[1], buf + i, 1);
                          }
                      }
                    }
                  }
                }
                //if (fds[i].revents & POLLIN) {
              }
            }
          }
        }
	      // printf("%d %d\n\r", pfd1[0], pfd1[1]);
	      // printf("%d %d\n\r", pfd2[0], pfd2[1]);
      }
    }
  }
    
  return 0;
}
/*
void read_buf(int fd, char *buf) {
  
}

void write_buf(int fd, char *buf, int i) {
  if (write(fd, buf, i) == -1) {
    perror("Can not write to stdout");
    exit(1);
  }
}*/