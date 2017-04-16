#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  printf("hello world (pid:%d)\n", (int) getpid());
  int i = 0;
  int fd[2];
  pipe(fd);
  printf("%d %d", fd[0], fd[1]);
  int rc = fork();
  if (rc < 0) { // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else {
    if (rc == 0) { // child (new process)
     printf("%d %d", fd[0], fd[1]);
     printf("hello, I am child (pid:%d)\n", (int) getpid());
    } else { // parent goes down this path (main)
      printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
      //close(fd[1]);
    }
  }
 return 0;
 }