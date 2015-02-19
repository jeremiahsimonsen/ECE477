#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int fd, serial;

	if ( (fd = open("/dev/ttyUSB0",O_RDWR) )< 0) {
		close(fd);
		perror("TTY open");
		return errno;
	}
	ioctl(fd,TIOCMGET, &serial);
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