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

#define NOTE_A 440
#define NOTE_B 494
#define NOTE_D 294
#define NOTE_G 392

// B A G A B B B
// A A A B D D
// B A G A B B B
// B A A B A G
// B A	G	A	B	B	B
// A A	A	B	D	D
// B A G A B B B
// B A A B A G
// D B A G A B B B
// A A A B D D
// D B A G A B B B
// B A A B A G
// D B A G A B B B
// A A A B D D
// D B A G A B B B
// B A A B A G
static const mary_lamb[108] = {NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_A, NOTE_A, NOTE_A, NOTE_B, NOTE_D, NOTE_D,
	NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_B, NOTE_A, NOTE_A, NOTE_B, NOTE_A, NOTE_G,
	NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_A, NOTE_A, NOTE_A, NOTE_B, NOTE_D, NOTE_D,
	NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B,NOTE_B,
	NOTE_B, NOTE_A, NOTE_A, NOTE_B, NOTE_A, NOTE_G,
	NOTE_D, NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_A, NOTE_A, NOTE_A, NOTE_B, NOTE_D, NOTE_D,
	NOTE_D, NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_B, NOTE_A, NOTE_A, NOTE_B, NOTE_A, NOTE_G,
	NOTE_D, NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_A, NOTE_A, NOTE_A, NOTE_B, NOTE_D, NOTE_D,
	NOTE_D, NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
	NOTE_B, NOTE_A, NOTE_A, NOTE_B, NOTE_A, NOTE_G};


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
	char new,old;
	outb(0xb6,0x43);
	outb(0x9A,0x42);	//LSB
	outb(0x04,0x42);	//MSB
	new = inb(0x61);
	old = new;
	new |= 3;
	outb(new,0x61);
	sleep(1);
	outb(old,0x61);
}

int note(unsigned int frequency, float delay)
{
	char new,old;
	// if (frequency > 5000) return 1;		// can't generate high freq well
	outb(0xb6,0x43);
	unsigned int timerval = 0x120000L / frequency;
	outb(timerval & 0xFF, 0x42);			// LSB
	outb((timerval & 0xFF00)>>8,0x42);			// MSB
	new = inb(0x61);
	old = new;
	new |= 3;
	outb(new,0x61);
	usleep(delay*1000);
	outb(old,0x61);
}

int main(int argc, char *argv[])
{
	if (ioperm(CHANNEL2,8,1) || ioperm(CONTROL,8,1) || ioperm(SPEAKER,8,1) < 0) {
		perror("Permissions");
		return errno;
	}

	// int i;
	// for(i=100;i<5000;i+=100) {
	// 	note(i,1);
	// }

	int i;
	for (i=0; i<104; i++) {
		note(mary_lamb[i],500);
	}

	return 0;
}