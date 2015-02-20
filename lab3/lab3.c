/******************************************************************************
 * ECE477 Lab 3, Part A
 * File: play_song.c
 * Author: Jeremiah Simonsen and Scott Edgerly
 * 
 * This is a program to control the brightness of an LED over a serial port by
 * pulse-width-modulation, implemented using the 8253 timer.
 *****************************************************************************/

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

// Definitions of used io ports
#define TMR_CH2 0x42
#define TMR_CNTRL 0x43

// Error checking macro
#define CHECK(x, y) do { \
	int retval = (x); \
	if (retval < 0) { \
		fprintf(stderr, "Runtime error: (%s) returned %d:%s at %s:%d\n", #x, retval, strerror(errno), __FILE__, __LINE__); \
	y; \
	} \
} while (0)

/* Do everything at once because it's short */
int main(int argc, char *argv[])
{
	int fd, serial,threshold;	// file descriptor, serial port status, and
								// threshold to control brightness
	unsigned int byte;			// used to store timer values

	/* First we must ask for permission to use the io ports */
	CHECK(ioperm(TMR_CH2,8,1) || ioperm(TMR_CNTRL,8,1), return errno);

	outb(0b10110100,TMR_CNTRL);		// Configure timer 0 channel 2 to count
									// down in mode 2 and take LSB then MSB
	outb(0xA0,TMR_CH2);				// Write LSB
	outb(0x0F,TMR_CH2);				// MSB

	/* Open serial port */
	CHECK(fd = open("/dev/ttyUSB0",O_RDWR), return errno);
	/* Read the serial port so we have the initial state */
	CHECK(ioctl(fd,TIOCMGET, &serial), return errno);

	/* Toggle LED forever */
	while(1) {
		CHECK(ioctl(fd, TIOCMGET, &serial), return errno);
		threshold = (serial & TIOCM_RNG) ? 2000 : 256;
		outb(0b10000000,TMR_CNTRL);	// latch channel 2
		byte = inb(TMR_CH2) | inb(TMR_CH2)<<8;	// read channel 2 counter
		if (byte & 0xFFFF >= threshold) {
			serial |= TIOCM_DTR;
			CHECK(ioctl(fd,TIOCMSET, &serial),return errno);
		} else {
			serial &= ~TIOCM_DTR;
			CHECK(ioctl(fd,TIOCMSET, &serial),return errno);
		}
	}
	
	close(fd);

	return 0;
}
