// Transmitter program
//
// This program will run on the Arduino Uno as the "transmitter"/
// wireless controller for this project. It will read the button
// states and joystick positions and sends the data via the XBee
// radio to the other XBee on the Arduino Due. The Due will take
// care of the rest.
//
// Data will be sent in the following format:
//   x,y,buttons
// where
//   x is the joystick x position (0-1023)
//   y is the joystick y position (0-1023)
//   button is the binary representation of all the buttons
//     <select><D3><D4><D5><D6>
//
// Example output to illustrate controller states:
//   507,507,11111	joystick centered, no buttons pressed
//   507,507,01111	select button pressed
//   507,507,10111	D3 button pressed
//   507,507,11011	D4 button pressed
//   507,507,11101	D5 button pressed
//   507,507,11110	D6 button pressed
//   507,1023,11111	joystick up
//   507,0,11111        joystick down
//   0,507,11111	joystick left
//   1023,507,11111	joystick right
//
// Author: Jeremiah Simonsen and Scott Edgerly
// Created on April 27, 2015

// Define button pin numbers
#define BUTTON_SEL 2    // Joystick "select" button
#define BUTTON_D3 3
#define BUTTON_D4 4
#define BUTTON_D5 5
#define BUTTON_D6 6

// Function to setup the input pins and serial port
// The serial port is configured for 9600 baud, 8 data bit, 2 stop bit, no parity
void setup() {
  // Set all buttons as inputs
  pinMode(BUTTON_SEL, INPUT);
  pinMode(BUTTON_D3, INPUT);
  pinMode(BUTTON_D4, INPUT);
  pinMode(BUTTON_D5, INPUT);
  pinMode(BUTTON_D6, INPUT);
  
  // Enable pull-up resistors on all the buttons
  digitalWrite(BUTTON_SEL, HIGH);
  digitalWrite(BUTTON_D3, HIGH);
  digitalWrite(BUTTON_D4, HIGH);
  digitalWrite(BUTTON_D5, HIGH);
  digitalWrite(BUTTON_D6, HIGH);  
  
  // Setup the serial port for the XBee at 9600 baud
  Serial.begin(9600);
}

// Function that runs continuously and sends the data via the XBee
void loop() {
  Serial.print(1023 - analogRead(0));            // Send joystick x position
  Serial.print(",");
  Serial.print(1023 - analogRead(1));            // Send joystick y position
  Serial.print(",");
  Serial.print(digitalRead(BUTTON_SEL));  // Send the select buton state
  Serial.print(digitalRead(BUTTON_D3));   // Send the D3 button state
  Serial.print(digitalRead(BUTTON_D4));   // Send the D4 button state
  Serial.print(digitalRead(BUTTON_D5));   // Send the D5 button state
  Serial.println(digitalRead(BUTTON_D6)); // Send the D6 button state
  
  // Wait 100ms
  delay(100);
}
