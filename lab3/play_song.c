/******************************************************************************
 * ECE477 Lab 3, Part B
 * File: play_song.c
 * Author: Jeremiah Simonsen and Scott Edgerly
 * 
 * This is a program to play "Mary had a little lamb" using the built-in
 * speaker and channel 2 of the 8253 programmable interval timer
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/io.h>
#include <string.h>

/* Definitions for the necessary io ports*/
#define TMR_CH2 0x42
#define TMR_CNTRL 0x43
#define SPKR_CNTRL 0x61

/* Musical notes necessary for the song */
#define NOTE_A 440
#define NOTE_B 494
#define NOTE_D 294
#define NOTE_G 392

// Error checking macro
#define CHECK(x, y) do { \
	int retval = (x); \
	if (retval != 0) { \
		fprintf(stderr, "Runtime error: (%s) returned %d:%s at %s:%d\n", #x, retval, strerror(errno), __FILE__, __LINE__); \
	y; \
	} \
} while (0)

/* Array of note frequencies that comprise Mary had a little lamb */
static const mary_lamb[108] = {
	NOTE_B, NOTE_A, NOTE_G, NOTE_A, NOTE_B, NOTE_B, NOTE_B,
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

/* Quick-test function to see if the speaker works.
 * Outputs a 512 Hz tone */
void tone_512() {
	char byte;				// To store the speaker status byte
	outb(0xb6,TMR_CNTRL);	// Configure channel 2 of timer0 to count down in 
							// mode 3 and take LSB then MSB
	outb(0,TMR_CH2);		// Send LSB to channel 2 counter
	outb(6,TMR_CH2);		// Send MSB
	byte = inb(SPKR_CNTRL);	// Read status of the speaker
	byte |= 3;				// Set enable bits
	outb(byte,SPKR_CNTRL);	// Write status of speaker, turns on speaker
}

/* Function to make the speaker beep.The speaker counter value is set to 0x49A 
 * for roughly 1 kHz tone */
void beep() {
	char new,old;			// To store the state of the speaker
	outb(0xb6,TMR_CNTRL);	// Configure channel 2 of timer0 to count down in
							// mode 3 and take LSB then MSB
	outb(0x9A,TMR_CH2);		// LSB
	outb(0x04,TMR_CH2);		// MSB
	new = inb(SPKR_CNTRL);	// Read status of the speaker
	old = new;				// Store previous state of the speaker
	new |= 3;				// Set the enable bits
	outb(new,SPKR_CNTRL);	// Write status of speaker, turns on speaker
	sleep(1);				// Delay; beep for 1 second
	outb(old,SPKR_CNTRL);	// Write old status of speaker, turns off speaker
}

/* Function to play a note of a given frequency (Hz) for a given delay (ms) */
int note(unsigned int frequency, float delay)
{
	char new,old;			// to store state of speaker
	 if (frequency > 5000) return 1;		// can't generate high freq well
	outb(0xb6,TMR_CNTRL);	// Configure timer
	unsigned int timerval = 0x120000L / frequency;	// Calculate timer value 
										// require to generate desired freq.
	outb(timerval & 0xFF, TMR_CH2);			// Write LSB
	outb((timerval & 0xFF00)>>8,TMR_CH2);	// MSB
	new = inb(SPKR_CNTRL);					// Read state of speaker
	old = new;								// Store it
	new |= 3;								// Set enable bits
	outb(new,SPKR_CNTRL);					// Write status of speaker (on)
	usleep(delay*1000);						// Delay for given time in ms
	outb(old,SPKR_CNTRL);					// Turn off speaker

	return 0;	// success
}

/* Main function to play the "Mary had a little lamb" song */
int main(int argc, char *argv[])
{
	/* Must ask for root permissions in order to access the io ports */
	CHECK(ioperm(TMR_CH2,8,1) || ioperm(TMR_CNTRL,8,1) || ioperm(SPKR_CNTRL,8,1), return errno);

	/* Play each note in the song */
	int i;
	for (i=0; i<108; i++) {
		note(mary_lamb[i],500);
	}

	return 0;
}