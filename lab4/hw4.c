#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>

// Error checking macros
#define CHECK(x, y) do { \
	int retval = (x); \
	if (retval < 0) { \
		fprintf(stderr, "Runtime error: (%s) returned %d:%s at %s:%d\n", #x, retval, strerror(errno), __FILE__, __LINE__); \
	y; \
	} \
} while (0)

int main(int argc, char * argv[])
{
	if (argc % 2 == 0){	//if there are an even # of arguments then the user
				//did not enter info correctlly on the command line
		printf("error:  Incorrect usage of program. Please send a flag/string pair\n");
		return -1;
	}
	char *serial_port = "/dev/ttyUSB0";
	char *filename = "default.csv";
	char *output;
	int serialfd, filenamefd, outputfd;
	struct termios my_termios;
	int i;
	for(i=2;i<argc;i+=2){	//need to grab the flag as well as the string
		if (strcmp(argv[i-1],"-p")==0){
			serial_port = argv[i];
		}
		else if (strcmp(argv[i-1],"-f")==0){
			filename = argv[i];
		}
		else if (strcmp(argv[i-1],"-o")==0){
			output = argv[i];
		}
	}
	CHECK(serialfd = open(serial_port, O_RDWR), return errno);
	CHECK(filenamefd = open(filename, O_RDWR | O_CREAT,S_IRUSR | S_IWUSR), return errno); //create the file if it doesn't exist
//	CHECK(outputfd = open(output, O_RDWR), return errno);  //open all the files and check for errors

	CHECK(tcgetattr(serialfd, &my_termios), return errno); //initalize and setup
	my_termios.c_oflag = 0;
	my_termios.c_cflag = CS8 | CSTOPB | CREAD | CLOCAL;
	my_termios.c_lflag = 0;

	CHECK(cfsetispeed(&my_termios, B9600), return errno);
	CHECK(cfsetospeed(&my_termios, B9600), return errno);
	CHECK(tcsetattr(serialfd, TCSANOW, &my_termios), return errno);	//set all settings

	int status;
	int pid;
	pid = fork();
	char ifbuf[512];
	if (pid > 0){	//parrent process
		while(1){
			CHECK(read(filenamefd, ifbuf, 512), return errno); //read from input
			CHECK(write(serialfd, ifbuf, 512), return errno);
			wait(&status); //wait for child process to end	
		}
	}

	if (pid ==0){	//child process
		char *str;
		FILE * fd = fdopen(serialfd, "r");
		while(1){
			fscanf(fd, "%s", str);
			fprintf(stdout,"%s", str);
		}
	}
	CHECK(close(filenamefd), return errno);
//	close (outputfd);
	printf("%s, %s, %s\n",serial_port, filename, output);
	return 0;
}
