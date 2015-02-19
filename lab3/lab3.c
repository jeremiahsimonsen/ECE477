#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
#define extern static
#include <sys/io.h>
#undef extern

#define CONTROL 0x43

int main(int argc, char *argv[])
{
	int fd, serial;

	if (ioperm(0x40,5,1) < 0) {
		perror("Permissions");
		return(errno);
	}

	outb(0b10110100,0x43);
	outb(0xA0,0x42);
	outb(0x0F,0x42);

#if 0
	char byte = inb(0x43);
	printf("First: %x\n", byte);
	byte = 0b10110100;
	outb(byte,0x43);
	byte = inb(0x43);
	printf("Second:%x\n",byte);
#endif
	char byte[4]={0};
	byte[0] = inb(0x40);
	byte[1] = inb(0x41);
	byte[2] = inb(0x42);
	byte[3] = inb(0x43);
	int i;
	for (i=0;i<4;i++) {
		printf("0x4%d: %x\n",i,byte[i]);
	}
#if 0
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
#endif

	return 0;
}
