#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

void segfault();

void sighandler(int);

int main (int argc, char *argv[]){
  static struct option args[] = {
    {"input", 1, NULL, 'i'},
    {"output", 1, NULL, 'o'},
    {"segfault", 0, NULL, 's'},
    {"catch", 0, NULL, 'c'},
    {0, 0, 0, 0}
  };
  int i;
  char *in_path = NULL;
  char *out_path = NULL;
  int segfault_flag = 0;
  int catch_flag = 0;
  
  while ((i = getopt_long(argc, argv, "i:o:sc", args, NULL)) != -1) {
    switch(i) {
      case 'i': {
        in_path = optarg;
	break;
      }
      case 'o': {
        out_path = optarg;
	break;
      }
      case 's': {
	segfault_flag = 1;
	break;
      }
      case 'c': {
        catch_flag = 1;
	break;
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
	printf("  --input=filename ... use the specified file as standard input\n");
	printf("  --output=filename ... create the specified file and use it as standard output\n");
	printf("  --segfault ... force a segmentation fault\n");
	printf("  --catch ... catch the segmentation fault\n");
	exit(1);
	break;
      }
    }
  }
  if (catch_flag)
    if (signal(SIGSEGV, sighandler) == SIG_ERR)
      printf("Can not catch segmentation fault!\n");
  if (segfault_flag) segfault();
  if (in_path != NULL) {
    int fd0 = open(in_path, O_RDONLY);
    if (fd0 >= 0) {
      close(0);
      dup(fd0);
      close(fd0);
    }
    else {
      fprintf(stderr, "Error opening input file: %s", strerror(errno));
      exit(2);
    }
  }
  if (out_path != NULL) {
    int fd1 = creat(out_path, 0666);
    if (fd1 >= 0) {
      close(1);
      dup(fd1);
      close(fd1);
    }
    else {
      fprintf(stderr, "Error opening output file: %s", strerror(errno));
      exit(3);
    }
  }
  
  char buf[10];
  int read_bytes = read(0, buf, 10);
  while (read_bytes != 0) {
    write(1, buf, read_bytes);
    read_bytes = read(0, buf, 10);
  }
  exit(0);
  return 0;
}

void segfault() {
  int *i = NULL;
  *i = 0;
}

void sighandler(int signo) {
  fprintf(stderr, "Error caught: %s", signo);
  exit(4);
}