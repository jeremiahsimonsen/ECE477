#include <stdio.h>
#include <errno.h>
#include <time.h>
void timestamp();
int main(int argc, char *argv[]) {
	char file_path[] = "/proc/meminfo";
	FILE *fp;
	FILE *fpout;
	char buff[100];
	char str[100];
	char val[100];
	int max, ram;
	double ratio;
	time_t ltime;
	fp = fopen(file_path,"r");
	if (fp == NULL) {
		perror("fopen");
		return errno;
	}
	fpout = fopen("datalog","a");
	if (fpout == NULL) {
		perror("fopen");
		return errno;
	}
	while(fgets(buff, 100, fp)!=NULL){ // scan until EoF
		sscanf(buff,"%s %s",str,val); // extract the word and the value in the string
		if(strcmp(str,"MemTotal:") == 0){ // if the first word is MemTotal,
			ltime = time(NULL);
			max = atoi(val); 	  //get the max value of the ram
			fprintf(fpout,"%s\n%s",asctime(localtime(&time)),buff);	  // print the value of MemTotal
		}
		if(strcmp(str,"MemFree:") == 0){ // same thing here just with MemFree
			ram = atoi(val);
			fprintf(fpout,"%s",buff); // print the value of MemFree
		}
	}
	ratio =(double)ram/max *100.0; //compute ratio
	fprintf(fpout,"Total RAM usage: %.2f%%\n\n\n",ratio); //print the ratio
	fclose(fp); //close the files and restart
	fclose(fpout);
	return 0;
}
