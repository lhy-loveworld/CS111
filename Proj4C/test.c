#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"id", 1, NULL, 'i'},
    {"log", 1, NULL, 'l'},
    {0, 0, 0, 0}
  };


  int arg_get;
  char* id;
  char* log;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 'i': {
      	/*id = malloc((strlen(optarg) + 1));
        strcpy(id, optarg);
        id[strlen(optarg)] = '\0';*/
        id = optarg;
        break;
      }
      case 'l': {
      	log = optarg;
      	break;
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
        printf("  --period=# ... specify a sampling interval in seconds\n");
        printf("  --scale=C/F ... temperatures reported in Celsius or Fahrenheit\n");
        printf("  --log=pathname ... append report to a logfile\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
      }
    }
  }

  printf("%s\n", id);
  printf("%s\n", log);
  return 0;
}