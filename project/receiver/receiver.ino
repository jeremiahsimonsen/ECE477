// Receiver program
//
// This program will run on the Arduino Due as the "receiver" for this
// project. It will read wirelessly transmitted controller data via
// the XBee radio and control the laser turret based on user input.
//
// See the transmitter program for details on controller data format.
//
// When no buttons are pressed the joystick will be used to control the
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
// Created on April 28, 2015

// Define the laser control pin number
#define LASER 13

// Define button states
#define BUTTON_PRESSED 0
#define BUTTON_UNPRESSED 1

String buff, cmd;
char c;
int bytesAvailable = 0;
int x, y;
int button_sel, button_d3, button_d4, button_d5, button_d6;

void setup() {
  // Configure the laser control pin as an output
  pinMode(LASER, OUTPUT);
  
  // Set the laser control pin low to start
  digitalWrite(LASER, LOW);
  
  // Start the serial port that is connected to the XBee with 9600 baud
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // Byte available on the serial port
  while (Serial1.available()) {
    c = Serial1.read();        // Read a byte
    buff += c;                 // Append to the buffer string
    
    // If we have reached a newline
    if (c == '\n') {
      cmd = buff;              // Copy to command string
      buff = "";               // Empty the buffer string
    }
  }
  
  // Send the command string to the USB port for testing purposes
  if (cmd.length() > 0) {
    Serial.print(cmd);
  }
  
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
  Serial.print("x = ");
  Serial.println(x);
  Serial.print("y = ");
  Serial.println(y);
  Serial.print("Select Button = ");
  Serial.println(button_sel);
  Serial.print("Button D3 = ");
  Serial.println(button_d3);
  Serial.print("Button D4 = ");
  Serial.println(button_d4);
  Serial.print("Button D5 = ");
  Serial.println(button_d5);
  Serial.print("Button D6 = ");
  Serial.println(button_d6);
  
  // Control the laser
  if (button_d5 == BUTTON_PRESSED) {
    digitalWrite(LASER, HIGH);
    Serial.println("FIRING LASER");
  } else {
    digitalWrite(LASER, LOW);
  }
}



