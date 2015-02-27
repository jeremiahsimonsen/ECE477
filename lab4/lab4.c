/*********************************************************************
 * ECE477 Lab 4, Part A
 * File: lab4.c
 * Author: Jeremiah Simonsen and Scott Edgerly
 * 
 * This is a program that splits into two processes using fork()
 * The parent process will read from an input file and output to
 * the serial port. The child process will read from the serial
 * port and output to an output csv file
 *********************************************************************/
 
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>
#include <stdlib.h>

/* Error checking macro */
#define CHECK(x, y) do { \
	int retval = (x); \
	if (retval < 0) { \
		fprintf(stderr, "Runtime error: (%s) returned %d:%s at %s:%d\n", #x, retval, strerror(errno), __FILE__, __LINE__); \
	y; \
	} \
} while (0)

#define BUFFSIZE 512

/* Function for parsing command line args 
 * -p flag denotes the serial port to be opened
 * -f flag denotes the input file
 * -o flag denotes the output file 
 * If an unexpected argument is passed, exit with usage message */
void getArgs(int argc, char* argv[], char** serialPort, char** infName, char** outfName)
{
	int i;
	/* For all of the command line arguments except argv[0] (program name) */
	for (i = 1; i < argc; i++) {

		/* Check for flags and grab parameters */
		if ((strcmp("-p", argv[i]) == 0) && (argc > ++i)) {
			*serialPort = argv[i];		// the serial port to open
		} else if ((strcmp("-f", argv[i]) == 0) && (argc > ++i)) {
			*infName = argv[i];			// the input file to open
		} else if ((strcmp("-o", argv[i]) == 0) && (argc > ++i)) {
			*outfName = argv[i];		// the output file
		} else if (argv[i][0] == '-') {	// error, give usage
			fprintf(stderr, "Unknown argument: %s\n", argv[i]);
			fprintf(stderr, "usage: %s [-p port_name] [-f input_file] [-o output_file]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
}

/* Function for setting up the serial port for:
 * canonical processing, no echo, no parity 9600 baud, 2 stop bits
 * no handshake, no special character translation or delays */
int setupPort(char *portFileName) {
	int serialfd;
	struct termios serial;

	/* Open the serial port file descriptor, checking errors */
	CHECK(serialfd = open(portFileName, O_RDWR | O_NOCTTY | O_SYNC), return errno);

	/* Fetch current serial port attributes to make setup easier */
	CHECK(tcgetattr(serialfd, &serial), return errno);

	/* Adjust serial port attributes */
	serial.c_iflag = IGNPAR;						// ignore parity
	serial.c_iflag &= ~IGNBRK;						// ignore breaks
	serial.c_oflag = 0;								// don't need anything special
	serial.c_cflag = CS8 | CSTOPB | CREAD | CLOCAL;	// byte size, stop bits, enable, no statuses
	serial.c_lflag = ICANON;						// canonical mode
	serial.c_lflag &= ~ECHO;						// no echo

	/* Set input/output baud rate */
	CHECK(cfsetispeed(&serial, B9600), return errno);	// input baud rate
	CHECK(cfsetospeed(&serial, B9600), return errno);	// output baud rate

	/* Apply settings */
	CHECK(tcsetattr(serialfd, TCSANOW, &serial), return errno);

	/* Flush the toilet */
	CHECK(tcflush(serialfd, TCIOFLUSH), return errno);

	/* Return the file descriptor */
	return serialfd;
}

/* Function for the parent process to execute 
 * Reads from input file, writes to serial port */
int parent(pid_t child, int serialfd, char *inputFile) {
	char ifbuff[BUFFSIZE];
	int inputfd;
	int status;

	/* Open input file */
	CHECK(inputfd = open(inputFile, O_RDONLY), return errno);

	/* Forever */
	while(1){
		CHECK(read(inputfd, ifbuff, BUFFSIZE), return errno); //read from input
		CHECK(write(serialfd, ifbuff, BUFFSIZE), return errno);
		// CHECK(waitpid(-1,&status,0), return errno); //wait for child process to end	
	}

	return 0;
}

/* Function for the child process to execute
 * Reads from the serial port, writes to output file */
int child(int serialfd, char *outputFile) {
	/* Variables */
	FILE* output = stdout;
	FILE* serialIN = NULL;
	char *inBuff[BUFFSIZE];
	int n1, n2, n3, n4, n5 = 0;
	int retVal;

	/* If output file specified, open it */
	if (outputFile != NULL) {
		output = fopen(outputFile, "w");
		if (output == NULL) {
			perror("Error child opening output file");
			return errno;
		}
	}

	/* Open serial port */
	if ((serialIN = fdopen(serialfd,"r")) == NULL) {
		perror("Child had problem opening serial");
		return errno;
	}

	/* Forever */
	while (1) {
		/* Read from serial port */
		retVal = fscanf(serialIN, "%d,%d,%d,%d,%d", &n1, &n2, &n3, &n4, &n5);
		if (retVal == EOF) {
			return errno;
		}

		/* Output to file */
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

/* Main program */
int main(int argc, char *argv[]) {
	/* Variables */
	static char *defaultSerial = "/dev/ttyUSB0";
	static char *defaultInputf = "./default.csv";
	char *portFile, *inputFile, *outputFile;
	int serialfd;
	pid_t pid;

	// Process command line arguments
	getArgs(argc, argv, &portFile, &inputFile, &outputFile);

	// Defaults
	if (portFile == NULL) {
		portFile = defaultSerial;
	}

	if (inputFile == NULL) {
		inputFile = defaultInputf;
	}

	// Setup the serial port
	CHECK(serialfd = setupPort(portFile),return errno);

	// split with fork
	CHECK(pid = fork(), CHECK(close(serialfd),return errno));

	if (pid == 0) { // child process
		return child(serialfd, outputFile);
	} else if (pid > 0) { // parent process
		return parent(pid, serialfd, inputFile);
	}

	return 0;
}