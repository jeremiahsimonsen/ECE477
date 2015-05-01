////////////////////////////////////////////////////////////////////////////
// Functions associated with receiving and parsing commands
//
// The fetchCMD function returns the newest command string
// The parseCMD function updates the members of the passed joystick
//
// Author: Jeremiah Simonsen and Scott Edgerly
////////////////////////////////////////////////////////////////////////////

#include "joystick.h"
#include "Arduino.h"
#include "string.h"

// Function to fetch the command string from the XBee
String fetchCMD(void) {
	String buff = "";
	String cmd = "";

	// Read a command string from the XBee
	while (Serial1.available()) {	// Byte available on the serial port
		char c = Serial1.read();	// Read a byte
		buff += c;					// Append to the buffer string
		
		// If we have reached a newline
		if (c == '\n') {
			cmd = buff;              // Copy to command string
			buff = "";               // Empty the buffer string
		}
	}
	
	// Send the command string to the USB port for testing purposes
	// if (cmd.length() > 0) {
	// Serial.print(cmd);
	// }

	return cmd;
}

// Function to parse the command string into usable numbers
void parseCMD(joystick *j, String cmd) {
	// Parse the command string into usable numbers
	// The command string is comma-delimited:
	// 		x,y,buttons

	// Find the indices of the two commas (x and y are not fixed length)
	int comma1 = cmd.indexOf(',');
	int comma2 = cmd.indexOf(',', comma1+1);
	
	// Cut out each piece and convert it to an integer
	j->x = cmd.substring(0, comma1).toInt();
	j->y = cmd.substring(comma1+1, comma2).toInt();
	j->sel = cmd[comma2+1] - '0';
	j->d3 = cmd[comma2+2] - '0';
	j->d4 = cmd[comma2+3] - '0';
	j->d5 = cmd[comma2+4] - '0';
	j->d6 = cmd[comma2+5] - '0';

	// Print everything to the USB port for verification
	// char buf[40];
	// sprintf(buf, "x = %d, y = %d\n",x,y);
	// Serial.print(buf);
	// sprintf(buf, "Select Button = %d\nButton D3 = %d\n",button_sel,button_d3);
	// Serial.print(buf);
	// sprintf(buf, "Button D4 = %d\nButtonD5 = %d\n",button_d4,button_d5);
	// Serial.print(buf);
	// sprintf(buf, "Button D6 = %d\n",button_d6);
	// Serial.print(buf);
}