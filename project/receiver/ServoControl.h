////////////////////////////////////////////////////////////////////////////
// ServoControl struct and function declarations
//
// This file contains the declarations of the data structure and functions
// for the implementation of the proportional-derivative (PD) controllers.
//
// Author: Jeremiah Simonsen and Scott Edgerly
////////////////////////////////////////////////////////////////////////////

// Necessary for using a C source file
#ifdef __cplusplus
extern "C" {
#endif

// Define the extreme servo positions
#define SERVO_MIN_POS   0L
#define SERVO_MAX_POS   1000L
// Define the servo center position
#define SERVO_CENTER_POS ((SERVO_MAX_POS - SERVO_MIN_POS)/2)

// PD controller variables
typedef struct {
	int pos;			// servo position
	int prevError;		// the previous error
	int kp;       		// proportional gain
	int kd;       		// derivative gain
} ServoControl;

// Function to initialize a pointer to a ServoControl struct
// [In] p and d, the proportional and derivative gains, respectively
// [Return] A pointer to a ServoControl struct
ServoControl* initServoControl(int p, int d);

// Function to update the position and error terms in a ServoControl
// [In] c - A pointer to the ServoControl to be updated
// [In] error - the error value that will be accounted for
// [Return] none
void updateServoControl(ServoControl *c, int error);

#ifdef __cplusplus
}
#endif