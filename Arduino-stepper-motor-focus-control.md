**Arduino C++ firmware** for your microcontroller (Teensy 4.1, Arduino Nano, or ESP32).  
This code acts as the "Muscle" to the Android's "Brain." It listens for the F+ and F- commands sent by our Python script and drives the stepper motor with precise, micro-stepped movements to achieve the high-pressure focus you need.

### **The Arduino Firmware (C++)**

WE recommend using the **AccelStepper** library (standard in the Arduino Library Manager) because it handles acceleration/deceleration automatically, preventing your heavy optical assembly from jerking or losing steps.

C++

/\*  
 \* PROJECT: NavyScope Focus Controller  
 \* HARDWARE: Arduino/Teensy \+ A4988/DRV8825 Driver \+ NEMA 17 Stepper  
 \* INTERFACE: USB Serial (Baud 9600\)  
 \*/

\#include \<AccelStepper.h\>

// \--- PIN CONFIGURATION (Adjust for your specific board) \---  
\#define STEP\_PIN 2  
\#define DIR\_PIN  3  
\#define ENABLE\_PIN 4  // Optional: To turn off motor power when idle

// \--- MOTOR SETTINGS \---  
// MotorInterfaceType 1 means a driver with Step and Direction pins  
AccelStepper focusMotor(AccelStepper::DRIVER, STEP\_PIN, DIR\_PIN);

// FINE TUNING  
// How many "steps" to move per command.   
// Smaller \= Slower but more precise focus. Larger \= Faster zoom.  
const int FOCUS\_INCREMENT \= 50; 

// Speed settings (Steps per second)  
const float MAX\_SPEED \= 1000.0;  
const float ACCELERATION \= 500.0;

String inputString \= "";      // A String to hold incoming data  
bool stringComplete \= false;  // Whether the string is complete

void setup() {  
  // 1\. Initialize Serial USB  
  Serial.begin(9600);  
  while (\!Serial) {  
    ; // Wait for serial port to connect (Needed for native USB boards like Teensy)  
  }

  // 2\. Setup Motor  
  pinMode(ENABLE\_PIN, OUTPUT);  
  digitalWrite(ENABLE\_PIN, LOW); // LOW usually enables the driver  
    
  focusMotor.setMaxSpeed(MAX\_SPEED);  
  focusMotor.setAcceleration(ACCELERATION);  
    
  // Initial ready message  
  Serial.println("NavyScope Focus System: READY");  
}

void loop() {  
  // 1\. PROCESS MOVEMENTS  
  // The run() function must be called as frequently as possible  
  focusMotor.run();

  // 2\. PROCESS COMMANDS  
  if (stringComplete) {  
    parseCommand(inputString);  
    // Clear for next command  
    inputString \= "";  
    stringComplete \= false;  
  }  
}

/\*  
 \* SerialEvent occurs whenever a new data comes in the hardware serial RX.  
 \* This handles reading the "F+" or "F-" from the Android phone.  
 \*/  
void serialEvent() {  
  while (Serial.available()) {  
    // Read the new byte:  
    char inChar \= (char)Serial.read();  
      
    // Add it to the inputString:  
    inputString \+= inChar;  
      
    // If we detect the '+' or '-' command terminator, we are ready to process  
    // Note: The Python script sends 'F+' or 'F-', so we look for these specific endings  
    if (inChar \== '+' || inChar \== '-') {  
      stringComplete \= true;  
    }  
  }  
}

/\*  
 \* Decodes the command and sets the motor target  
 \*/  
void parseCommand(String command) {  
  // Check if command contains "F+" (Focus IN/Forward)  
  if (command.indexOf("F+") \>= 0\) {  
    // Move relative to current position  
    focusMotor.move(FOCUS\_INCREMENT);  
    // Serial.println("ACK: Focus IN"); // Optional debug  
  }  
    
  // Check if command contains "F-" (Focus OUT/Backward)  
  else if (command.indexOf("F-") \>= 0\) {  
    focusMotor.move(-FOCUS\_INCREMENT);   
    // Serial.println("ACK: Focus OUT"); // Optional debug  
  }  
}

### **Hardware Wiring Guide**

To connect your **Samsung S25** (via USB-C OTG) to this Microcontroller and Motor, follow this schematic logic:

1. **Microcontroller (e.g., Teensy 4.1):**  
   * **USB Port:** Connects to Samsung S25.  
   * **Pin 2 (STEP):** Connects to STEP pin on Stepper Driver.  
   * **Pin 3 (DIR):** Connects to DIR pin on Stepper Driver.  
   * **GND:** Connects to Driver GND.  
2. **Stepper Driver (A4988 / DRV8825):**  
   * **VMOT:** Connect to 12V Battery (Motor Power).  
   * **VDD:** Connect to Microcontroller 5V/3.3V (Logic Power).  
   * **1A/1B/2A/2B:** Connect to the Stepper Motor coils.  
3. **Power Supply:**  
   * **CRITICAL:** Do *not* try to power the stepper motor from the Samsung phone's USB port. It does not have enough amps. You need a separate 12V Li-Ion battery pack (or your 12V reserve system) to power the motor driver's VMOT pin. The Phone only powers the Microcontroller "brain."

### **How it Works with the Python Script**

1. **Android:** The Python script detects the image is blurry (low contrast).  
2. **Command:** It sends F+ over USB.  
3. **Arduino:** The serialEvent() catches the \+, and parseCommand() tells AccelStepper to move the motor **50 steps** forward.  
4. **Loop:** focusMotor.run() executes those 50 steps smoothly with acceleration.  
5. **Repeat:** If the image gets sharper, the Android sends F+ again. If it gets worse, the Android logic flips and sends F-.
