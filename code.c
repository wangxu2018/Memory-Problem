#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 512

int main (int argc, char **argv)
{
	int fd = open("readText.txt", O_CREAT | O_RDWR | O_DIRECT | O_EXCL);

	// char buf[1024];
	
	char *buf;
	(void)posix_memalign((void **)&buf, BUF_SIZE, BUF_SIZE);
	int readSize = read(fd, buf, BUF_SIZE);
	if (readSize == -1) {
		printf("errno:%d, errnoStr:%s\n", errno, strerror(errno));
		return -1;
	}

	printf("exec end!\n");


	close(fd);
    return 0;
}
