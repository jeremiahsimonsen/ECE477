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

// function for parsing command line args
void parseArgs(int argc, char *argv[], char **serialPort, char **inputFile,
	char **outputFile) {
	int i;
	for(i=2;i<argc;i+=2){	//need to grab the flag as well as the string
		if (strcmp(argv[i-1],"-p")==0){
			*serialPort = argv[i];
		} else if (strcmp(argv[i-1],"-f")==0){
			*inputFile = argv[i];
		} else if (strcmp(argv[i-1],"-o")==0){
			*outputFile = argv[i];
		} else {
			fprintf(stderr, "Unknown argument: %s\n", argv[i]);
		}
	}
}

// function for setting up the serial port
int setupPort(char *portFileName) {
	int serialfd;
	struct termios serial;

	/* Open the serial port file descriptor, checking errors */
	CHECK(serialfd = open(portFileName, O_RDWR | O_NOCTTY | O_SYNC), 
		return errno);

	/* Fetch current serial port attributes to make setup easier */
	CHECK(tcgetattr(serialfd, &serial), return errno);

	/* Adjust serial port attributes */
	serial.c_oflag = 0;
	serial.c_cflag = CS8 | CSTOPB | CREAD | CLOCAL;
	serial.c_lflag = 0;

	/* Set input/output baud rate */
	CHECK(cfsetispeed(&serial, B9600), return errno);
	CHECK(cfsetospeed(&serial, B9600), return errno);

	/* Apply settings */
	CHECK(tcsetattr(serialfd, TCSANOW, &serial), return errno);

	/* Flush the toilet */
	CHECK(tcflush(serialfd, TCIOFLUSH), return errno);

	return serialfd;
}

//todo function for parent

//todo function for child

int main(int argc, char *argv[]) {
	static char *defaultSerial = "/dev/tty/USB0";
	static char *defaultInputf = "default.csv";
	char *portFile, *inputFile, *outputFile;
	int serialfd;
	pid_t pid;

	//todo parseArgs shits

	//todo split with fork

	return 0;
}