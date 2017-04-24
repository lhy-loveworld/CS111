//Arthor: Hongyang Li
//This is the source module for lab1a of CS111.

#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>


struct termios saved_attributes;
int rc = 0;

void exit_handler (void) {
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
  if (rc > 0) {
    int status;
    if (waitpid(rc, &status, 0) == -1) {
      fprintf(stderr, "waitpid() failed: %s\n", strerror(errno));
    }
    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
  }
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

void pipe_handler(int signo) {
  exit(0);
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
      fprintf(stderr, "unrecognized argument\n");
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
        fprintf(stderr, "read() failed: %s\n", strerror(errno));
        exit(1);
      } else {
        for (i = 0; i < read_count; ++i) {
          switch (buf[i]) {
            case 0x0D:
            case 0x0A:
              if (write(1, "\r\n", 2) == -1) {
              	fprintf(stderr, "write() failed: %s\n", strerror(errno));
              }
              break;
            case 0x04:
              exit(0);
            default:
              if (write(1, buf + i, 1) == -1) {
              	fprintf(stderr, "write() failed: %s\n", strerror(errno));
              }
          }
        }
      }
    }
   
  } else {
    int pfd1[2], pfd2[2];
    if (pipe(pfd1) < 0) {
      fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
      exit(1);
    }
    if (pipe(pfd2) < 0) {
      fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
      exit(1);
    }
    rc = fork();
    if (rc < 0) { // fork failed; exit
      fprintf(stderr, "fork() failed: %s\n", strerror(errno));
      exit(1);
    } else {
      if (rc == 0) { // child (new process)
        close(pfd1[1]);
        close(pfd2[0]);
        dup2(pfd1[0], 0);
	    	dup2(pfd2[1], 1);
	    	dup2(pfd2[1], 2);
        close(pfd1[0]);
        close(pfd2[1]);
        char *child_args[2];
	      child_args[0] = strdup("/bin/bash");
	      child_args[1] = NULL;
	      if (execvp(child_args[0], child_args) == -1) {
          fprintf(stderr, "execvp() failed: %s\n", strerror(errno));
          exit(1);
        }
      } else { // parent goes down this path (main)
        signal(SIGPIPE, pipe_handler);
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
            fprintf(stderr, "poll() failed: %s\n", strerror(errno));
            exit(1);
          } else {
            if (ret_poll > 0) {
              for (i = 0; i < 2; ++i) {
                if (fds[i].revents & POLLIN) {
                  read_count = read(fds[i].fd, buf, 1024);
                  if (read_count == -1) {
                    fprintf(stderr, "read() failed: %s\n", strerror(errno));
                    exit(1);
                  } else {
                    for (j = 0; j < read_count; ++j) {
                      switch (buf[j]) {
                      	case 0x0A:
                      		if (i == 1) {
                      			write(1, "\r\n", 2);
                      		}
                        case 0x0D:
                          if (i == 0) {
                            write(1, "\r\n", 2);
                            write(pfd1[1], "\n", 1);
                          }
                          break;
                        case 0x04:
                        	if (i == 0) {
                            close(pfd1[1]);
                        	}
                        	break;
                        case 0x03:
                          kill(rc, SIGINT);
                          //status & 0x7F, (status & 0xFF) >> 8);
                          //exit(0);
                        default:
                          write(1, buf + j, 1);
                          if (i == 0) {
                            write(pfd1[1], buf + j, 1);
                          }
                      }
                    }
                  }
                }
                if (fds[i].revents & POLLHUP) {
                	if (i == 1) {
                		close(pfd2[0]);
                		//status & 0x7F, (status & 0xFF) >> 8);
                		exit(0);
                	}
                }
                if (fds[i].revents & POLLERR) {
                	//status & 0x7F, (status & 0xFF) >> 8);
                	exit(0);
                }
              }
            }
          }
        }
      }
    }
  }
    
  return 0;
}
