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

// Define button states
#define BUTTON_PRESSED 0
#define BUTTON_UNPRESSED 1

// Deadband on the vertical axis to try to get rid of shaking
#define STICK_V_DEADBAND 20

// Libraries for interfacing with the Pixy camera
#include <SPI.h>
#include <Pixy.h>

// Define the camera center in pixels
#define X_CENTER        160L
#define Y_CENTER        100L

// Define the extreme servo positions
#define SERVO_MIN_POS   0L
#define SERVO_MAX_POS   1000L
// Define the servo center position
#define SERVO_CENTER_POS ((SERVO_MAX_POS - SERVO_MIN_POS)/2)

// Define a PD servo controller class
class ServoControl {
public:
	// Constructor that takes proportional and derivative gains
	ServoControl(int kp, int kd);
	
	// Member function that will update the position based on the error
	void update(int error);
	
	// Controller variables
	int pos;          // servo position
	int prevError;    // the previous error
	int kp;           // proportional gain
	int kd;           // derivative gain
};

// PD servo controller constructor
ServoControl::ServoControl(int p, int d) {
	pos = SERVO_CENTER_POS;    // the servo will start in the center
	kp = p;                    // store the gain variables
	kd = d;
	prevError = 0x80000000L;
}

// Function to update the servo position based on the error
void ServoControl::update(int error) {
	long int vel;        // velocity term
	
	// If there is error
	if (prevError != 0x80000000) {
		vel = (error*kp + (error - prevError)*kd)>>10;    // calculate the velocity
		pos += vel;                                       // update position

		// Handle positions out of bounds
		if (pos > SERVO_MAX_POS) {
			pos = SERVO_MAX_POS;
		} else if (pos < SERVO_MIN_POS) {
			pos = SERVO_MIN_POS;
		}
	}
	// Update the error term
	prevError = error;
}

// Initialize pan and tilt PD servo controllers
ServoControl pan(350, 600);
ServoControl tilt(500, 700);

// For communicating with the Pixy camera
Pixy pixy;

// XBee input buffer and command string
String buff, cmd;
char c;
int bytesAvailable = 0;
// Controller state variables
int x, y;
int button_sel, button_d3, button_d4, button_d5, button_d6;

// Function to fetch the command string from the XBee
void fetchCMD() {
	// Read a command string from the XBee
	while (Serial1.available()) {// Byte available on the serial port
		c = Serial1.read();        // Read a byte
		buff += c;                 // Append to the buffer string
		
		// If we have reached a newline
		if (c == '\n') {
			cmd = buff;              // Copy to command string
			buff = "";               // Empty the buffer string
		}
	}
	
	// Send the command string to the USB port for testing purposes
//  if (cmd.length() > 0) {
//    Serial.print(cmd);
//  }
}

// Function to parse the command string to usable numbers
void parseCMD() {
	// Parse the command string into usable numbers
	int comma1 = cmd.indexOf(',');
	int comma2 = cmd.indexOf(',', comma1+1);
	x = cmd.substring(0, comma1).toInt();
	y = cmd.substring(comma1+1, comma2).toInt();
	button_sel = cmd[comma2+1] - '0';
	button_d3 = cmd[comma2+2] - '0';
	button_d4 = cmd[comma2+3] - '0';
	button_d5 = cmd[comma2+4] - '0';
	button_d6 = cmd[comma2+5] - '0';

	// Print everything to the USB port for verification
//  char buf[40];
//  sprintf(buf, "x = %d, y = %d\n",x,y);
//  Serial.print(buf);
//  sprintf(buf, "Select Button = %d\nButton D3 = %d\n",button_sel,button_d3);
//  Serial.print(buf);
//  sprintf(buf, "Button D4 = %d\nButtonD5 = %d\n",button_d4,button_d5);
//  Serial.print(buf);
//  sprintf(buf, "Button D6 = %d\n",button_d6);
//  Serial.print(buf);
}

// Function to configure pins and ports
void setup() {
	// Configure the laser control pin as an output
	pinMode(LASER, OUTPUT);
	
	// Set the laser control pin low to start
	digitalWrite(LASER, LOW);
	
	// Start the serial port that is connected to the XBee with 9600 baud
	Serial1.begin(9600);
//  Serial.begin(9600);
	
	// Initialize the pixy cam
	pixy.init();
}

// Function executed continuously that controls the turret
void loop() {
	// Fetch commands from the XBee
	fetchCMD();
	
	// Parse commands into usable numbers
	parseCMD();
	
	// Control the laser
	if (button_d5 == BUTTON_PRESSED) {    // If the fire button is pressed
		digitalWrite(LASER, HIGH);          // Turn laser on
//    Serial.println("FIRING LASER");   // Verification over serial
	} else {                              // Fire button not pressed
		digitalWrite(LASER, LOW);           // Turn laser off
	}
	
	// Use the Pixy for image based tracking
	if (button_d6 == BUTTON_PRESSED) {    // If the Pixy control button is pressed
		int blocks;                         // number of detected objects matching a signature
		int panError, tiltError;            // Error terms for servo controllers
		
		blocks = pixy.getBlocks();          // Fetch the image recognition data from Pixy
		
		// If any objects have been recognized
		if (blocks) {
			// Calculate error terms as distance from center
			panError = X_CENTER - pixy.blocks[0].x;
			tiltError = pixy.blocks[0].y - Y_CENTER;
			
			// Update the controller object error terms
			pan.update(panError);
			tilt.update(tiltError);
			
			// Set the new servo positions
			pixy.setServos(pan.pos, tilt.pos);
		}
	} 
	
	// Manual joystick control
	if (button_d3 == BUTTON_PRESSED) {    // If the manual control button is pressed
		// Handle out of bounds positions
		if (x > SERVO_MAX_POS) {
			x = SERVO_MAX_POS;
		} else if (x < SERVO_MIN_POS) {
			x = SERVO_MIN_POS;
		}
		
		if (y > SERVO_MAX_POS) {
			y = SERVO_MAX_POS;
		} else if ( (y > 512 - STICK_V_DEADBAND) && (y < 512 + STICK_V_DEADBAND) ) {
			y = SERVO_CENTER_POS;
		} else if (y < SERVO_MIN_POS) {
			y = SERVO_MIN_POS;
		}
		
		// Serial verification
//    char buf[32];
//    sprintf(buf, "x = %d, y = %d",x,y);
//    Serial.println(buf);

		// Update the servo positions
		pixy.getBlocks();     // For whatever reason, this is necessary before setServos()
		pixy.setServos(x, y); // Set the new servo positions
//    pan.pos = x;        // Doesn't seem needed - was intended to update the position
//    tilt.pos = y;       // variables in the controller objects for error correction
	}
}



