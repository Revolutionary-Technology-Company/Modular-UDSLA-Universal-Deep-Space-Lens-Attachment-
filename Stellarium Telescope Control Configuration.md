To make Stellarium recognize your custom "NavyScope" as a valid device, we cannot select "NavyScope" from the menu because it doesn't exist in their commercial database.  
Instead, we must configure your Arduino to **emulate** a standard protocol that Stellarium already speaks. The industry standard for DIY telescopes is the **Meade LX200** protocol.  
Here is the configuration document to bridge the two.

# ---

**Document: Stellarium Mobile \<\> NavyScope Integration**

### **1\. The Strategy: "The Meade Mask"**

Stellarium sends commands like *"Where are you?"* and *"Move to Coordinates X,Y."* It expects a specific language.

* **Protocol to use:** Meade LX200 (Classic)  
* **Why:** It is the simplest ASCII protocol. If your Arduino simply acknowledges these commands, Stellarium will turn "Green" (Connected), allowing you to see where the telescope is pointing on your phone screen.

### ---

**2\. Samsung S25 Configuration Steps (Stellarium Mobile Plus)**

Follow these specific menu settings on your Android device to add the interface.  
**Step 1: Physical Connection**

1. Connect the Samsung Galaxy S25 to the Arduino/Teensy via the **USB-C OTG Cable**.  
2. Accept any Android permission pop-ups asking to "Allow Stellarium to access USB device."

**Step 2: Stellarium Setup**

1. Open **Stellarium Mobile Plus**.  
2. Tap the **Hamburger Menu (≡)** in the top left.  
3. Select **Observing Tools** $\\rightarrow$ **Telescope Control**.  
4. Tap **Add New Telescope** (or the **\+** icon).

Step 3: Device Settings (The "Spoof")  
Input exactly these settings to match the Arduino firmware:

* **Connection Type:** Serial (Direct USB connection).  
* **Telescope Name:** NavyScope Main (You can name this whatever you want).  
* **Device Model:** Meade LX200 (Classic) (CRITICAL: Do not choose GOTO or newer generic drivers).  
* **Coordinate System:** J2000 (Standard for Stellarium).

**Step 4: Serial Parameters**

* **Serial Port:** Auto (or select USB Serial / /dev/bus/usb... if listed).  
* **Baud Rate:** 9600 (Must match your Arduino code).  
* **Data Bits:** 8  
* **Parity:** None  
* **Stop Bits:** 1  
* *(Note: This is often written as "9600 8N1")*

**Step 5: Initialization**

1. Tap **Save**.  
2. Toggle the switch **ON** next to "NavyScope Main."  
3. **Status Indicator:**  
   * **Yellow:** Trying to handshake.  
   * **Green:** Connected (Success).  
   * **Red:** Connection Failed (Usually means Arduino didn't reply to the handshake).

### ---

**3\. Required Arduino Update (The Handshake)**

The previous Arduino code *only* listened for Focus (F+). For Stellarium to give you a "Green Light," the Arduino **must** reply to the LX200 handshake queries (Right Ascension and Declination requests).  
Add this snippet to your existing Arduino serialEvent or loop to handle the identity check:

C++

/\* \* NAVYSCOPE LX200 EMULATOR SNIPPET  
 \* Paste this into your existing parseCommand() function.  
 \* This tricks Stellarium into thinking it is talking to a real LX200.  
 \*/

void parseCommand(String command) {  
    
  // \--- EXISTING FOCUS COMMANDS \---  
  if (command.indexOf("F+") \>= 0\) {  
    focusMotor.move(FOCUS\_INCREMENT);   
  }  
  else if (command.indexOf("F-") \>= 0\) {  
    focusMotor.move(-FOCUS\_INCREMENT);   
  }

  // \--- NEW: STELLARIUM HANDSHAKES (LX200 Protocol) \---  
    
  // Command: :GR\#  (Get RA \- "Where are you pointing?")  
  // We reply with a dummy coordinate (or real encoder data if you have it)  
  // Format: HH:MM:SS\#  
  if (command.indexOf(":GR\#") \>= 0\) {  
    Serial.print("12:00:00\#");   
  }

  // Command: :GD\# (Get Declination)  
  // Format: sDD\*MM\# (s=sign)  
  else if (command.indexOf(":GD\#") \>= 0\) {  
    Serial.print("+45\*00\#");   
  }

  // Command: :CM\# (Sync/Calibrate)  
  // Stellarium says "You are here." We say "Okay."  
  else if (command.indexOf(":CM") \>= 0\) {  
    Serial.print("M31 EXCALIBUR\#"); // Any text ending in \# usually works  
  }  
}

### **4\. How to Use It**

1. **Open Stellarium:** Confirm the telescope icon is connected (Green).  
2. **Select Object:** Tap "Jupiter" in the sky chart.  
3. **Command Slew:** Tap the **Telescope Icon** $\\rightarrow$ **Slew**.  
4. **Result:** Stellarium sends the coordinates of Jupiter to your Arduino.  
   * *Note:* Since your current design only has a *Focus Motor*, the Arduino will receive the data but won't move the tube. However, this confirms the **database link** is active, allowing you to add RA/DEC motors later without changing the software.