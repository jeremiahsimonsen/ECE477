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

ServoControl* initServoControl(int p, int d);

void updateServoControl(ServoControl *c, int error);

#ifdef __cplusplus
}
#endif