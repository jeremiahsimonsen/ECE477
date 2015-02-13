#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	char file_path[] = "/proc/meminfo";
	FILE *fp;
	char buff[100];
	char str[100];
	char value[100]; 
	while(1) {
		fp = fopen(file_path,"r");
		if (fp == NULL) {
			perror("fopen");
			return errno;
		}
		while(fgets(buff, 100, fp)!=NULL){ // scan until EoF
			sscanf(buff, "%s %s", str); // extract the first word in the string
			if(strcmp(str,"MemFree:") == 0){ // if the first word is MemFree , print the line
				printf("%s\n",buff); // print the value of MemFree
			}
		}
		fclose(fp); //close the file and restart
	}
	return 0;
}
