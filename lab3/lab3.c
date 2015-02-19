#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
#include <sys/io.h>

#define CHANNEL0 0x40
#define CHANNEL1 0x41
#define CHANNEL2 0x42
#define CONTROL 0x43

int main(int argc, char *argv[])
{
	int fd, serial,threshold;
	unsigned int byte;

	if (ioperm(CHANNEL2,8,1) || ioperm(CONTROL,8,1)< 0) {
		perror("Permissions");
		return(errno);
	}

	outb(0b10110100,CONTROL);		// Configure timer 0 channel 2
	outb(0xA0,CHANNEL2);		// low byte
	outb(0x0F,CHANNEL2);		// high byte

	/* Open serial port */
	if ( (fd = open("/dev/ttyUSB0",O_RDWR) )< 0) {
		close(fd);
		perror("TTY open");
		return errno;
	}
	ioctl(fd,TIOCMGET, &serial);		// Read the serial port
	/* Toggle LED forever */
	while(1) {
		ioctl(fd, TIOCMGET, &serial);
		if (serial & TIOCM_RNG) {
			threshold = 2000;
		} else {
			threshold = 256;
		}
		outb(0b10000000,CONTROL);	// latch channel 2
		byte = inb(CHANNEL2) | inb(CHANNEL2)<<8;	// read channel 2 counter
		if (byte & 0xFFFF >= threshold) {
			serial |= TIOCM_DTR;
			ioctl(fd,TIOCMSET, &serial);
		} else {
			serial &= ~TIOCM_DTR;
			ioctl(fd,TIOCMSET, &serial);
		}
	}
	
	close(fd);

	return 0;
}
