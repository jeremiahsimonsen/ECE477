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

	char buff[100];https://github.com/jeremiahsimonsen/ECE477.git
	while(1) {
		buff = fgets()
	}

	fclose(fp);

	return 0;
}