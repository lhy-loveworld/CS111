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


void read_buf(int, char *);
void write_buf(int, char *, int);

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
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

int main(int argc, char *argv[]) {
  static struct option args[] = {
    {"shell", 0, NULL, 's'},
    {0, 0, 0, 0}
  };
  int shell_flag = 0;
  int i;
  while ((i = getopt_long(argc, argv, "", args, NULL)) != -1) {
    if (i == 's') {
      shell_flag = 1;
    } else {
      printf("Please enter correct commands as shown below!\n");
      printf("  --shell ... pass input/output between the terminal and a shell\n");
      exit(1);
    }
  }
  
  if (!shell_flag) {
    char buf[1];
    read_buf(0, buf, &old_attributes);
    while (buf[0] != 0x04) {
      if ((buf[0] == 0x0D)||(buf[0] == 0x0A)) {
        write_buf(1, "\r\n", 2, &old_attributes);
      } else { 
        write_buf(1, buf, 1, &old_attributes);
      }
      read_buf(0, buf, &old_attributes);
    }
  } else {
    int pfd1[2], pfd2[2];
    if (pipe(pfd1) < 0) {
      perror("Creating pipe failed");
      set_attr(0, &old_attributes);
      exit(1);
    }
    if (pipe(pfd2) < 0) {
      perror("Creating pipe failed");
      set_attr(0, &old_attributes);
      exit(1);
    }
    int rc = fork();
    if (rc < 0) { // fork failed; exit
      perror("Fork failed\n");
      set_attr(0, &old_attributes);
      exit(1);
    } else {
      if (rc == 0) { // child (new process)
        dup2(0, pfd1[0]);
	dup2(1, pfd2[1]);
        char *child_args[2];
	child_args[0] = strdup("/bin/bash");
	child_args[1] = NULL;
	execvp(child_args[0], child_args);
      } else { // parent goes down this path (main)
	printf("%d %d\n\r", pfd1[0], pfd1[1]);
	printf("%d %d\n\r", pfd2[0], pfd2[1]);
      }
    }
  }
    
  exit(0);
  return 0;
}

void read_buf(int fd, char *buf) {
  if (read(fd, buf, 1) == -1) {
    perror("Can not read from stdin");
    exit(1);
  }
}

void write_buf(int fd, char *buf, int i) {
  if (write(fd, buf, i) == -1) {
    perror("Can not write to stdout");
    exit(1);
  }
}