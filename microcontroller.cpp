/*
 * PROJECT: NavyScope Focus Controller
 * HARDWARE: Arduino/Teensy + A4988/DRV8825 Driver + NEMA 17 Stepper
 * INTERFACE: USB Serial (Baud 9600)
 */

#include <AccelStepper.h>

// --- PIN CONFIGURATION (Adjust for your specific board) ---
#define STEP_PIN 2
#define DIR_PIN  3
#define ENABLE_PIN 4  // Optional: To turn off motor power when idle

// --- MOTOR SETTINGS ---
// MotorInterfaceType 1 means a driver with Step and Direction pins
AccelStepper focusMotor(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// FINE TUNING
// How many "steps" to move per command. 
// Smaller = Slower but more precise focus. Larger = Faster zoom.
const int FOCUS_INCREMENT = 50; 

// Speed settings (Steps per second)
const float MAX_SPEED = 1000.0;
const float ACCELERATION = 500.0;

String inputString = "";      // A String to hold incoming data
bool stringComplete = false;  // Whether the string is complete

void setup() {
  // 1. Initialize Serial USB
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect (Needed for native USB boards like Teensy)
  }

  // 2. Setup Motor
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW); // LOW usually enables the driver
  
  focusMotor.setMaxSpeed(MAX_SPEED);
  focusMotor.setAcceleration(ACCELERATION);
  
  // Initial ready message
  Serial.println("NavyScope Focus System: READY");
}

void loop() {
  // 1. PROCESS MOVEMENTS
  // The run() function must be called as frequently as possible
  focusMotor.run();

  // 2. PROCESS COMMANDS
  if (stringComplete) {
    parseCommand(inputString);
    // Clear for next command
    inputString = "";
    stringComplete = false;
  }
}

/*
 * SerialEvent occurs whenever a new data comes in the hardware serial RX.
 * This handles reading the "F+" or "F-" from the Android phone.
 */
void serialEvent() {
  while (Serial.available()) {
    // Read the new byte:
    char inChar = (char)Serial.read();
    
    // Add it to the inputString:
    inputString += inChar;
    
    // If we detect the '+' or '-' command terminator, we are ready to process
    // Note: The Python script sends 'F+' or 'F-', so we look for these specific endings
    if (inChar == '+' || inChar == '-') {
      stringComplete = true;
    }
  }
}

/*
 * Decodes the command and sets the motor target
 */
void parseCommand(String command) {
  // Check if command contains "F+" (Focus IN/Forward)
  if (command.indexOf("F+") >= 0) {
    // Move relative to current position
    focusMotor.move(FOCUS_INCREMENT);
    // Serial.println("ACK: Focus IN"); // Optional debug
  }
  
  // Check if command contains "F-" (Focus OUT/Backward)
  else if (command.indexOf("F-") >= 0) {
    focusMotor.move(-FOCUS_INCREMENT); 
    // Serial.println("ACK: Focus OUT"); // Optional debug
  }
}
