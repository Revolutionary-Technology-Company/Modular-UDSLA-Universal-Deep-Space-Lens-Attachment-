To make the base "VERY solid" and integrate it with our existing Samsung/Arduino logic, we will move from a standard tripod design to a **"Navy Turret" Mount**.  
This design uses **Worm Drive Gearing**. This is critical because worm gears are **self-locking**—meaning if power is lost, the telescope does not fall; it stays frozen in position, just like a tank turret.

### **1\. The Hardware: "The Turret" Base**

* **The Joint:** **12-inch Industrial Slewing Bearing.**  
  * *Why:* This replaces flimsy tripod heads. It is a solid ring of steel ball bearings that can support 1,000+ lbs.  
* **The Motors:** **NEMA 23 Stepper Motors (High Torque).**  
  * *Why:* Your NEMA 17 focus motor is too weak for the base. NEMA 23s are the "muscle."  
* **The Transmission:** **50:1 Worm Gearbox.**  
  * *Why:* This creates massive torque and prevents the wind from moving the scope.

### **2\. Updated System Block Diagram**

We are adding two new "Organs" to your system: **Azimuth (Rotation)** and **Altitude (Tilt)**.

Plaintext

       \[ SAMSUNG S25 (Brain) \]  
              |  
              | USB (Stellarium \+ NavyScope App)  
              v  
       \[ MICROCONTROLLER (Teensy 4.1) \]  
              |  
      \+-------+-------+  
      |       |       |  
  \[DRIVER 1\] \[DRIVER 2\] \[DRIVER 3\]  
      |       |       |  
      v       v       v  
   (FOCUS)  (AZIMUTH) (ALTITUDE)  
  \[Nema 17\] \[Nema 23\] \[Nema 23\]  
   (Lens)    (Base)    (Tilt)

### **3\. The "Action" Code (Arduino C++)**

You need to update your firmware to listen for Stellarium's "Slew" commands (:Mn\#, :Ms\#, :Mw\#, :Me\# for North, South, West, East).  
Here is the **Motion Control Logic** to add to your existing code.

C++

\#include \<AccelStepper.h\>

// \--- PIN DEFINITIONS \---  
// BASE (Azimuth)  
\#define AZ\_STEP\_PIN 5  
\#define AZ\_DIR\_PIN 6  
// TILT (Altitude)  
\#define ALT\_STEP\_PIN 7  
\#define ALT\_DIR\_PIN 8

// \--- MOTOR SETUP \---  
AccelStepper azMotor(AccelStepper::DRIVER, AZ\_STEP\_PIN, AZ\_DIR\_PIN);  
AccelStepper altMotor(AccelStepper::DRIVER, ALT\_STEP\_PIN, ALT\_DIR\_PIN);

// SPEEDS  
const float SLEW\_SPEED \= 2000.0; // Fast for moving to target  
const float TRACK\_SPEED \= 15.0;  // Slow to follow stars

void setup() {  
  Serial.begin(9600);  
    
  // Settings for Heavy Turret Base  
  azMotor.setMaxSpeed(3000);  
  azMotor.setAcceleration(1000); // Smooth start/stop (No Jerk)  
    
  altMotor.setMaxSpeed(3000);  
  altMotor.setAcceleration(1000);  
}

void loop() {  
  azMotor.run();  
  altMotor.run();  
    
  // (Existing Serial Event logic goes here)  
}

// \--- UPDATED COMMAND PARSER \---  
void parseCommand(String command) {  
    
  // 1\. FOCUS (Existing)  
  if (command.indexOf("F+") \>= 0\) { /\* Focus Move Code \*/ }

  // 2\. MOVE NORTH (Tilt Up)  
  // Stellarium Command: :Mn\#  
  if (command.indexOf(":Mn\#") \>= 0\) {  
    altMotor.setSpeed(SLEW\_SPEED);   
    // We don't use 'move()' here because we want it to run UNTIL we say stop  
  }

  // 3\. MOVE SOUTH (Tilt Down)  
  // Stellarium Command: :Ms\#  
  if (command.indexOf(":Ms\#") \>= 0\) {  
    altMotor.setSpeed(-SLEW\_SPEED);  
  }

  // 4\. MOVE EAST (Rotate Base Right)  
  // Stellarium Command: :Me\#  
  if (command.indexOf(":Me\#") \>= 0\) {  
    azMotor.setSpeed(SLEW\_SPEED);  
  }

  // 5\. STOP MOVEMENT  
  // Stellarium Command: :Q\# (Quit moving)  
  if (command.indexOf(":Q\#") \>= 0\) {  
    azMotor.setSpeed(0);  
    altMotor.setSpeed(0);  
    // Optional: Switch to "Tracking Speed" here to follow the earth's rotation  
  }  
}

### **4\. Wiring the "Heavy Metal"**

Since you are using NEMA 23 motors, **you cannot use the small drivers** (like A4988) used for the focus motor. They will burn out.

* **Required Driver:** **DM542T or TB6600** (Industrial external drivers).  
* **Wiring:**  
  * PUL+ / DIR+ go to Arduino Pins 5/6/7/8.  
  * PUL- / DIR- go to Arduino GND.  
  * VCC / GND go to your **24V Power Supply** (Needed for heavy motors; 12V is too weak for this weight).

### **Execution Checklist**

1. **Buy:** 12" Slewing Bearing, 2x NEMA 23 Motors, 2x DM542T Drivers.  
2. **Build:** Bolt the bearing to a heavy plate. Mount the Worm Drive gears to the motor shafts.  
3. **Flash:** Upload the code above to the Arduino.  
4. **Connect:** Plug into Samsung S25. Open Stellarium. Press the "Arrow Keys" in the app. The turret will turn.