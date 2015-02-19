#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
//#define extern static
#include <sys/io.h>
//#undef extern

#define CHANNEL0 0x40
#define CHANNEL1 0x41
#define CHANNEL2 0x42
#define CONTROL 0x43

int main(int argc, char *argv[])
{
	int fd, serial;
	char byte;

	if (ioperm(0x40,40,1) < 0) {
		perror("Permissions");
		return(errno);
	}

	outb(0b10110100,CONTROL);		// Configure timer 0 channel 2
	outb(0xA0,CHANNEL2);					// low byte
	outb(0x0F,CHANNEL2);					// high byte

	// outb(0b10000100,CONTROL);
	// byte = inb(CHANNEL2) | inb(CHANNEL2)<<8;
	// printf("%d\n", byte&0xFFFF);

#if 1

	/* Open serial port */
	if ( (fd = open("/dev/ttyUSB0",O_RDWR) )< 0) {
		close(fd);
		perror("TTY open");
		return errno;
	}
	ioctl(fd,TIOCMGET, &serial);		// Read the serial port
	/* Toggle LED forever */
	while(1) {
		outb(0b10000000,CONTROL);		// latch channel 2
		byte = inb(CHANNEL2) | inb(CHANNEL2)<<8;					// read channel 2 counter
		// need to read twice
		printf("%d\n", byte&0xFFFF);
		if (byte & 0xFFFF >= 256) {
			serial |= TIOCM_DTR;
			ioctl(fd,TIOCMSET, &serial);
			// printf("here\n");
		} else {
			serial &= ~TIOCM_DTR;
			ioctl(fd,TIOCMSET, &serial);
		}
	}
	
	close(fd);
#endif

	return 0;
}
