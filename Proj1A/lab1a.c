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

//void attr_get(int fd, struct termios attr);
void set_attr(int, struct termios *);
void read_buf(int, char *, struct termios *);
void write_buf(int, char *, int, struct termios *);

int main(int argc, char *argv[]) {
  static struct option args[] = {
    {"shell", 0, NULL, 's'},
    {0, 0, 0, 0}
  };
  int shell_flag = 0;
  int i;
  while ((i = getopt_long(argc, argv, "s", args, NULL)) != -1) {
    if (i == 's') {
      shell_flag = 1;
    } else {
      printf("Please enter correct commands as shown below!\n");
      printf("  --shell ... pass input/output between the terminal and a shell\n");
      exit(1);
    }
  }
  struct termios old_attributes;
  struct termios new_attributes;
  if (tcgetattr(0, &old_attributes) != 0) {
    perror("Can not get attribute");
    exit(1);
  }
  new_attributes = old_attributes;
  new_attributes.c_iflag = ISTRIP;
  new_attributes.c_oflag = 0;
  new_attributes.c_lflag = 0;
  set_attr(0, &new_attributes);
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
    
  set_attr(0, &old_attributes);
  exit(0);
  return 0;
}

void set_attr(int fd, struct termios *attr_p) {
  if (tcsetattr(fd, TCSANOW, attr_p) != 0) {
    perror("Can not set attributes!");
    exit(1);
  }
}

void read_buf(int fd, char *buf, struct termios *attr_p) {
  if (read(fd, buf, 1) == -1) {
    perror("Can not read from stdin");
    set_attr(fd, attr_p);
    exit(1);
  }
}

void write_buf(int fd, char *buf, int i, struct termios *attr_p) {
  if (write(fd, buf, i) == -1) {
    perror("Can not write to stdout");
    set_attr(0, attr_p);
    exit(1);
  }
}