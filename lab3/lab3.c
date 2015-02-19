#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
#include <sys/io.h>

#define CONTROL 0x43

int main(int argc, char *argv[])
{
	int fd, serial;

	if (ioperm(0x40,5,1) < 0) {
		perror("Permissions");
		return(errno);
	}

	char byte = inb(CONTROL);
	printf("%x\n", byte);

	/* Open serial port */
	if ( (fd = open("/dev/ttyUSB0",O_RDWR) )< 0) {
		close(fd);
		perror("TTY open");
		return errno;
	}
	ioctl(fd,TIOCMGET, &serial);		// Read the serial port
	/* Toggle LED forever */
	while(1) {
		serial |= TIOCM_DTR;
		ioctl(fd,TIOCMSET, &serial);
		sleep(1);
		serial &= ~TIOCM_DTR;
		ioctl(fd,TIOCMSET, &serial);
		sleep(1);
	}
	
	close(fd);

	return 0;
}