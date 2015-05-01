////////////////////////////////////////////////////////////////////////////
// Function declarations associated with dealing with commands
// 
// This part was written in C++ for the convenience of using String
//
// Author: Jeremiah Simonsen and Scott Edgerly
////////////////////////////////////////////////////////////////////////////

// Necessary for using String
#include "WString.h"


// Define button states
#define BUTTON_PRESSED 0
#define BUTTON_UNPRESSED 1

// Deadband on the vertical axis to try to get rid of shaking
#define DEADBAND 20

// Define the joystick velocity scale factor
#define JOYSTICK_VEL_SCALE 1

// Struct to encapsulate all the transmitter variables
typedef struct {
	int x, y;					// Joystick x and y values
	char sel, d3, d4, d5, d6;	// Joystick button states
} joystick;

// Function to fetch the command string from the XBee
// [In] none
// [Return] The command string
String fetchCMD(void);

// Function to parse the command string into usable numbers
// [In] j - pointer to a joystick
// [In] cmd - the command String
// [Return] none
void parseCMD(joystick *j, String cmd);