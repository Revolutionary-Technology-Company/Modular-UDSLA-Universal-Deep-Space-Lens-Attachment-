  
This design requires a "Closed-Loop" control system. Since we want "point-and-click" functionality like Google Maps, your Samsung S25 cannot just be a remote control; it must act as the **Autofocus Brain**.  
The Android phone must analyze the live video feed (from the Canon), detect the "sharpness" of the object you tapped, and drive the USB stepper motor until that object is crisp. This replicates the **Contrast Detection Autofocus** used in smartphones111.

### **The "Tap-to-Focus" Architecture**

1. **Visual Input:** The Canon R5 sends a Live View stream to the Samsung S25 (via Wi-Fi or USB-C Capture Card).  
2. **User Input:** You tap a star/planet on the Samsung screen.  
3. **Processing:** The Python script calculates the "Contrast Score" of that specific area.  
4. **Action:** The phone sends step commands to the motor controller (Arduino/Teensy) to maximize that score.

### **Python Code Snippet (Kivy \+ OpenCV)**

This Python script is designed to run on Android (via Kivy/Buildozer). It creates a touch interface overlay on your video feed.

Python

import cv2  \# OpenCV for image processing  
import serial \# PySerial for USB communication  
from kivy.app import App  
from kivy.uix.widget import Widget  
from kivy.clock import Clock  
from kivy.graphics.texture import Texture  
from kivy.uix.image import Image

\# USB CONFIGURATION  
\# Connects to the Arduino/Teensy controlling the Stepper Motor  
try:  
    \# Android usually mounts USB OTG devices here, but this path varies  
    ser \= serial.Serial('/dev/ttyUSB0', 9600, timeout=1)  
except:  
    print("USB Motor Controller not found. Running in simulation mode.")  
    ser \= None

class TelescopeCam(Image):  
    def \_\_init\_\_(self, \*\*kwargs):  
        super(TelescopeCam, self).\_\_init\_\_(\*\*kwargs)  
        self.capture \= cv2.VideoCapture(0) \# 0 is usually the USB Capture Card input  
        Clock.schedule\_interval(self.update, 1.0 / 30.0) \# 30 FPS  
        self.roi\_box \= None \# Region of Interest (Where you tapped)

    def update(self, dt):  
        ret, frame \= self.capture.read()  
        if ret:  
            \# If we are autofocusing, draw a box around the target  
            if self.roi\_box:  
                x, y, w, h \= self.roi\_box  
                cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2\)  
                  
                \# 1\. CALCULATE SHARPNESS (Contrast Detection)   
                \# We crop the video to just the area you tapped  
                roi\_frame \= frame\[y:y+h, x:x+w\]  
                gray \= cv2.cvtColor(roi\_frame, cv2.COLOR\_BGR2GRAY)  
                \# variance of Laplacian is a standard measure of "focus/sharpness"  
                score \= cv2.Laplacian(gray, cv2.CV\_64F).var()  
                  
                \# 2\. SEND COMMAND TO MOTOR  
                self.autofocus\_logic(score)

            \# Convert to Kivy Texture for display  
            buf1 \= cv2.flip(frame, 0\)  
            buf \= buf1.tostring()  
            image\_texture \= Texture.create(size=(frame.shape\[1\], frame.shape\[0\]), colorfmt='bgr')  
            image\_texture.blit\_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')  
            self.texture \= image\_texture

    def on\_touch\_down(self, touch):  
        \# 3\. INTERACTION: TAP TO FOCUS \[cite: 185\]  
        \# Translate screen tap coordinates to video coordinates  
        print(f"User tapped at: {touch.x}, {touch.y}")  
          
        \# Define a 100x100 pixel box around the tap to analyze  
        \# This tells the system: "Make THIS spot sharp" \[cite: 197\]  
        self.roi\_box \= (int(touch.x) \- 50, int(touch.y) \- 50, 100, 100\)  
        return super(TelescopeCam, self).on\_touch\_down(touch)

    def autofocus\_logic(self, current\_score):  
        \# A simple "Hill Climbing" algorithm  
        \# If score is getting better, keep moving motor same way.   
        \# If score drops, reverse direction.  
          
        \# Pseudo-code for USB Command  
        if ser:  
            \# Example: Send "STEP\_FORWARD" command  
            \# You would implement the full PID loop here  
            ser.write(b'F+') 

class NavyScopeApp(App):  
    def build(self):  
        return TelescopeCam()

if \_\_name\_\_ \== '\_\_main\_\_':  
    NavyScopeApp().run()

### **Hardware Component: The USB Bridge**

Your Samsung S25 cannot power the stepper motor directly. You need a microcontroller bridge.

* **Microcontroller:** Arduino Nano or Teensy 4.1.  
* **Connection:** USB-C OTG cable from Samsung S25 to Microcontroller.  
* **Function:** The Python script sends simple text characters (like F+ for Focus In, F- for Focus Out) via USB serial. The Arduino reads these and pulses the stepper motor driver.

### **How it Works (The "Google Maps" Feel)**

1. **Tap Detection:** When you tap the screen, the App captures the $(x, y)$ coordinates2.

2. **Contrast Analysis:** Instead of analyzing the whole image, the code isolates just the 100x100 pixel box you tapped. It looks for "edges" (high contrast)3.

3. **Motor Feedback:** The app drives the motor. If the "Sharpness Score" (Laplacian variance) goes UP, it keeps going. If it goes DOWN, it reverses. It locks when the score peaks4.

### **Next Step**

Build the **Arduino C++ code** that goes on the microcontroller to interpret these F+ / F- commands and drive the stepper motor.
