#include <AccelStepper.h>

// --- PIN DEFINITIONS ---
// BASE (Azimuth)
#define AZ_STEP_PIN 5
#define AZ_DIR_PIN 6
// TILT (Altitude)
#define ALT_STEP_PIN 7
#define ALT_DIR_PIN 8

// --- MOTOR SETUP ---
AccelStepper azMotor(AccelStepper::DRIVER, AZ_STEP_PIN, AZ_DIR_PIN);
AccelStepper altMotor(AccelStepper::DRIVER, ALT_STEP_PIN, ALT_DIR_PIN);

// --- CONFIGURATION ---
const float MAX_SPEED = 3000.0;
const float ACCELERATION = 1000.0;
const long SLEW_DISTANCE = 500000; // A generic "far away" point to trigger movement

// --- SAFETY LIMITS (Soft Stops) ---
// Set these based on your specific mount geometry to prevent crashes
const long ALT_MAX_UP = 5000;    // Max steps UP (e.g., 90 degrees)
const long ALT_MAX_DOWN = -2000; // Max steps DOWN (e.g., -20 degrees)

// --- COMMAND BUFFER ---
String inputString = "";         // A String to hold incoming data
bool stringComplete = false;     // Whether the string is complete

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);

  // Azimuth Settings
  azMotor.setMaxSpeed(MAX_SPEED);
  azMotor.setAcceleration(ACCELERATION);

  // Altitude (Tilt) Settings
  altMotor.setMaxSpeed(MAX_SPEED);
  altMotor.setAcceleration(ACCELERATION);
  
  Serial.println("System Ready: Turret & Tilt Online");
}

void loop() {
  // 1. PROCESS MOTOR MOVEMENT
  azMotor.run();
  altMotor.run();

  // 2. CHECK SAFETY LIMITS (The "Virtual Bumper")
  // If we go past the limit, force the motor to stop immediately
  if (altMotor.currentPosition() > ALT_MAX_UP && altMotor.speed() > 0) {
    altMotor.stop(); // Decelerate to stop
    Serial.println("Error: MAX UP LIMIT REACHED");
  }
  if (altMotor.currentPosition() < ALT_MAX_DOWN && altMotor.speed() < 0) {
    altMotor.stop();
    Serial.println("Error: MAX DOWN LIMIT REACHED");
  }

  // 3. PROCESS COMMANDS
  if (stringComplete) {
    parseCommand(inputString);
    // Clear the string:
    inputString = "";
    stringComplete = false;
  }
}

// --- SERIAL EVENT (Reads data automatically) ---
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    // If the incoming character is a hash # (LX200 standard terminator)
    if (inChar == '#') {
      stringComplete = true;
    }
  }
}

// --- COMMAND PARSER ---
void parseCommand(String command) {
  
  // --- TILT CONTROL (Altitude) ---
  
  // MOVE NORTH (Tilt Up) -> :Mn#
  if (command.indexOf(":Mn#") >= 0) {
    // Check limit BEFORE moving
    if (altMotor.currentPosition() < ALT_MAX_UP) {
      altMotor.move(SLEW_DISTANCE); // Move to a point excessively far away
    }
  }

  // MOVE SOUTH (Tilt Down) -> :Ms#
  if (command.indexOf(":Ms#") >= 0) {
    if (altMotor.currentPosition() > ALT_MAX_DOWN) {
      altMotor.move(-SLEW_DISTANCE); // Move to a point excessively far negative
    }
  }

  // --- BASE CONTROL (Azimuth) ---

  // MOVE EAST (Rotate Right) -> :Me#
  if (command.indexOf(":Me#") >= 0) {
    azMotor.move(SLEW_DISTANCE);
  }

  // MOVE WEST (Rotate Left) -> :Mw#
  if (command.indexOf(":Mw#") >= 0) {
    azMotor.move(-SLEW_DISTANCE);
  }

  // --- STOP COMMAND ---
  // QUIT MOVEMENT -> :Q#
  if (command.indexOf(":Q#") >= 0) {
    // This stops the motor smoothly using the set acceleration
    azMotor.stop();
    altMotor.stop();
    
    // Note: AccelStepper.stop() sets a new target position just ahead of the 
    // current position to allow for deceleration. 
    // You must keep calling run() in the loop!
  }
}
