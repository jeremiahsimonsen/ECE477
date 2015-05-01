////////////////////////////////////////////////////////////////////////////
// Receiver program
//
// This program will run on the Arduino Due as the "receiver" for this
// project. It will read wirelessly transmitted controller data via
// the XBee radio and control the laser turret based on user input.
//
// See the transmitter program for details on controller data format.
//
// When the D3 button is pressed the joystick will be used to control the
// turret directly.
//
// The laser will be fired by pressing the D5 button on the controller.
//
// If the D6 button is held, the laser turret will use the Pixy camera
// to track a target balloon.
//
// Automatic firing of the laser may be added in a later version.
//
// Author: Jeremiah Simonsen and Scott Edgerly
////////////////////////////////////////////////////////////////////////////

// Define the laser control pin number
#define LASER 13

// Libraries for interfacing with the Pixy camera
#include <SPI.h>
#include <Pixy.h>

// Define the camera center in pixels
#define X_CENTER        160L
#define Y_CENTER        100L

#include "ServoControl.h"
#include "joystick.h"

// Initialize pan and tilt PD servo controllers
ServoControl *pan;
ServoControl *tilt;

// For communicating with the Pixy camera
Pixy pixy;

// XBee input buffer and command string
String cmd;
int bytesAvailable = 0;
joystick j;

// Function to configure pins and ports
void setup() {
	// Configure the laser control pin as an output
	pinMode(LASER, OUTPUT);
	
	// Set the laser control pin low to start
	digitalWrite(LASER, LOW);
	
	// Start the serial port that is connected to the XBee with 9600 baud
	Serial1.begin(9600);
 	Serial.begin(9600);
	
	pan = initServoControl(350, 600);
	tilt = initServoControl(500,700);

	// Initialize the pixy cam
	pixy.init();
}

// Function executed continuously that controls the turret
void loop() {
	// Fetch commands from the XBee
	// fetchCMD();
	cmd = fetchCMD();
	
	// Parse commands into usable numbers
	parseCMD(&j, cmd);
	
	// Control the laser
	if (j.d5 == BUTTON_PRESSED) {    // If the fire button is pressed
		digitalWrite(LASER, HIGH);          // Turn laser on
//    Serial.println("FIRING LASER");   // Verification over serial
	} else {                              // Fire button not pressed
		digitalWrite(LASER, LOW);           // Turn laser off
	}
	
	// Use the Pixy for image based tracking
	if (j.d6 == BUTTON_PRESSED) {    // If the Pixy control button is pressed
		int blocks;                         // number of detected objects matching a signature
		int panError, tiltError;            // Error terms for servo controllers
		
		blocks = pixy.getBlocks();          // Fetch the image recognition data from Pixy
		
		// If any objects have been recognized
		if (blocks) {
			// Calculate error terms as distance from center
			panError = X_CENTER - pixy.blocks[0].x;
			tiltError = pixy.blocks[0].y - Y_CENTER;
			
			// Update the controller object error terms
			updateServoControl(pan, panError);
			updateServoControl(tilt, tiltError);
			
			// Set the new servo positions
			pixy.setServos(pan->pos, tilt->pos);
		}
	} 
	
	// Manual joystick control
	if (j.d3 == BUTTON_PRESSED) {    // If the manual control button is pressed
		// Handle out of bounds positions and implement a deadband
		// Negative positions make no sense and the servo cannot rotate beyond
		// a maximum position. A deadband is used to eliminate shakiness when 
		// the joystick is centered.

		// Do it for the joystick x axis
		if (j.x > SERVO_MAX_POS) {
			j.x = SERVO_MAX_POS;
		} else if ( (j.x > 512 - DEADBAND) && (j.x < 512 + DEADBAND)) {
			j.x = SERVO_CENTER_POS;
		} else if (j.x < SERVO_MIN_POS) {
			j.x = SERVO_MIN_POS;
		}
		
		// And the y axis
		if (j.y > SERVO_MAX_POS) {
			j.y = SERVO_MAX_POS;
		} else if ( (j.y > 512 - DEADBAND) && (j.y < 512 + DEADBAND) ) {
			j.y = SERVO_CENTER_POS;
		} else if (j.y < SERVO_MIN_POS) {
			j.y = SERVO_MIN_POS;
		}
		
		// Convert joystick position to a "velocity" between +/- 1.0
		float dx = ((float)j.x / (float)SERVO_CENTER_POS) - 1.0;
		float dy = ((float)j.y / (float)SERVO_CENTER_POS) - 1.0;

		// Scale the values so the speed is adjustable
		dx *= JOYSTICK_VEL_SCALE;
		dy *= JOYSTICK_VEL_SCALE;

		// Serial verification
		char buf[32];
		sprintf(buf, "dx = %f, dy = %f",dx,dy);
		Serial.println(buf);

		// Update the servo positions
		pixy.getBlocks();		// For whatever reason, this is necessary before setServos()
		pan->pos += dx;			// Update the positions stored in the controllers
		tilt->pos += dy;
		dx = dy = 0.0;			// Zero the velocity terms just in case
		pixy.setServos(pan->pos, tilt->pos);	// Send servo positions
	}
}



