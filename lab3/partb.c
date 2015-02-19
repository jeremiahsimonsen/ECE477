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
#define SPEAKER 0x61

void tone_512() {
	char byte;
	outb(0xb6,0x43);
	outb(0,0x42);
	outb(6,0x42);
	byte = inb(0x61);
	byte |= 3;
	outb(byte,0x61);
}

void beep() {
	int delay=0;
	char new,old;
	outb(0xb6,0x43);
	outb(0x9A,0x42);	//LSB
	outb(0x04,0x42);	//MSB
	new = inb(0x61);
	old = new;
	new &= ~3;
	outb(new,0x61);
	while(delay<20000) {delay++;}
	outb(old,0x61);
}

int main(int argc, char *argv[])
{
	if (ioperm(CHANNEL2,8,1) || ioperm(CONTROL,8,1) || ioperm(SPEAKER,8,1) < 0) {
		perror("Permissions");
		return errno;
	}
	// while(1) {
		beep();
	// }

	return 0;
}