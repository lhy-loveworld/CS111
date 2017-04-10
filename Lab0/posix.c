#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]){
  int fd0 = open("./foo.txt", O_RDONLY);
    if (fd0 >= 0) {
      close(0);
      dup(fd0);
      close(fd0);
    }
    printf("%d", fd0);
    return 0;
}