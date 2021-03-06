////////////////////////////////////////////////////////////////////////////
// ServoControl function definitions
//
// This file contains the definitions of the functions that operate on
// ServoControl structs. There is an initializer function and an update
// function that accounts for the error.
//
// Author: Jeremiah Simonsen and Scott Edgerly
////////////////////////////////////////////////////////////////////////////

#include "ServoControl.h"

// Function to initialize a ServoControl
ServoControl* initServoControl(int p, int d) {
	// Allocate memory
	ServoControl *c = (ServoControl *)malloc(sizeof(ServoControl));
	c->pos = SERVO_CENTER_POS;		// starting position is the center
	c->kp = p;						// store the input gains
	c->kd = d;
	c->prevError = 0x80000000L;		// starting previous error (none)

	return c;
}

// Function to update the position and error terms of a ServoControl
void updateServoControl(ServoControl *c, int error) {
	long int vel;        // velocity term
	
	// If there is error
	if (c->prevError != 0x80000000) {
		vel = (error*c->kp + (error - c->prevError)*c->kd)>>10;    // calculate the velocity
		c->pos += vel;                                       // update position

		// Handle positions out of bounds
		if (c->pos > SERVO_MAX_POS) {
			c->pos = SERVO_MAX_POS;
		} else if (c->pos < SERVO_MIN_POS) {
			c->pos = SERVO_MIN_POS;
		}
	}
	// Update the error term
	c->prevError = error;
}