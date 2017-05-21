#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main ()
{
	char buf[20];
	int i;
	//for (i = 0; i < 10; ++i) {
	while (1) {
		bzero(buf, 20);
		gets(buf);
		if (!strcmp(buf, "OFF\n")) printf("1321\n");
		if (strlen(buf)) printf("%ld %s", strlen(buf), buf);
	}
  
   //printf("Current local time and date: %s", asctime(info));
   return(0);
}