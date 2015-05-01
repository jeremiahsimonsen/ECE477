#include "ServoControl.h"

// Function to initialize a ServoControl
ServoControl* initServoControl(int p, int d) {
	ServoControl *c = (ServoControl *)malloc(sizeof(ServoControl));
	c->pos = SERVO_CENTER_POS;
	c->kp = p;
	c->kd = d;
	c->prevError = 0x80000000L;

	return c;
}

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