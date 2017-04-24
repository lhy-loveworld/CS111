#include <mcrypt.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	int fd = open("my.key", O_RDONLY);
	char buffer[256] = "";
	printf("%d\n",read(fd,buffer, 32));
	/*for (int i = 0; i < 16; ++i)
	{
		printf("%c", buffer[i]);
		//if (buffer[i] == '\0') printf("%d\n", i);	
	}
	printf("%d\n", strlen(buffer));
	char *buffer2 = "asd";
	printf("%s\n", strcat(buffer, "5"));*/
	MCRYPT td1, td2;
	char key[16];
	strcpy(key, buffer);
	char *IV = "AAAAAAAAAAAAAAAA";
	int keysize = 16;
	td1 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
	td2 = mcrypt_module_open("twofish", NULL, "cfb", NULL);
	printf("blocksize: %d, keysize: %d, ivsize: %d\n", mcrypt_enc_get_block_size(td1), mcrypt_enc_get_key_size(td1), mcrypt_enc_get_iv_size(td1));
	mcrypt_generic_init(td1, key, keysize, IV);
	mcrypt_generic_init(td2, key, keysize, IV);
	mcrypt_generic(td1,buffer, 255);
	printf("%s\n", buffer);
	printf("string length: %d\n", strlen(buffer));
	mdecrypt_generic(td2, buffer, 255);
printf("%s\n", buffer);
	
	mcrypt_generic_deinit(td1);
	  mcrypt_module_close(td1);

  mcrypt_generic_deinit(td2);
  mcrypt_module_close(td2);
	return 0;
}