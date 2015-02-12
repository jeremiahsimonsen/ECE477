#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	char file_path[] = "/proc/meminfo";
	FILE *fp;

	fp = fopen(file_path,"r");
	if (fp == NULL) {
		perror("fopen");
		return errno;
	}

	while(1) {
		// do stuff
	}

	fclose(fp);

	return 0;
}