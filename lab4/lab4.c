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

#define BUFFSIZE 512

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
	CHECK(serialfd = open(portFileName, O_RDWR | O_NOCTTY | O_SYNC), return errno);

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
int parent(pid_t child, int serialfd, char *inputFile) {
	char ifbuff[BUFFSIZE];
	int inputfd;
	int status;

	/* Open input file */
	CHECK(inputfd = open(inputFile, O_RDONLY), return errno);

	while(1){
		CHECK(read(inputfd, ifbuff, BUFFSIZE), return errno); //read from input
		CHECK(write(serialfd, ifbuff, BUFFSIZE), return errno);
		// CHECK(waitpid(-1,&status,0), return errno); //wait for child process to end	
	}

	return 0;
}

//todo function for child
int child(int serialfd, char *outputFile) {
	FILE* output = stdout;
	FILE* serialIN = NULL;
	char *inBuff[BUFFSIZE];
	int n1, n2, n3, n4, n5 = 0;
	int retVal;

	if (outputFile) {
		CHECK(output = fopen(outputFile, "w"), return errno);
	}

	if (serialIN = fdopen(serialfd,"r") == NULL) {
		perror("Child had problem opening serial");
		return errno;
	}

	while (1) {
		//todo actual output stuff
		retVal = fscanf(serialIN, "%d,%d,%d,%d,%d", &n1, &n2, &n3, &n4, &n5);
		if (retVal == EOF) {
			return errno;
		}

		switch (retVal) {
			case 1:
				fprintf(output, "%d\n", n1);
				break;
			case 2:
				fprintf(output, "%d,%d\n", n1, n2);
				break;
			case 3:
				fprintf(output, "%d,%d,%d\n", n1, n2, n3);
				break;
			case 4:
				fprintf(output, "%d,%d,%d,%d\n", n1, n2, n3, n4);
				break;
			case 5:
				fprintf(output, "%d,%d,%d,%d,%d\n", n1, n2, n3, n4, n5);
				break;
		}
	}

	return 0;
}

int main(int argc, char *argv[]) {
	static char defaultSerial[] = "/dev/ttyUSB0";
	static char defaultInputf[] = "./default.csv";
	char *portFile, *inputFile, *outputFile;
	int serialfd;
	pid_t pid;

	//todo parseArgs shits
	parseArgs(argc, argv, &portFile, &inputFile, &outputFile);

	if (portFile == NULL) {
		portFile = defaultSerial;
	}

	if (inputFile == NULL) {
		inputFile = defaultInputf;
	}

	serialfd = setupPort(defaultSerial);

	//todo split with fork
	CHECK(pid = fork(), CHECK(close(serialfd),return errno));

	if (pid == 0) {
		return child(serialfd, outputFile);
	} else if (pid > 0) {
		return parent(pid, serialfd, defaultInputf);
	}

	return 0;
}