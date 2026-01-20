import cv2  # OpenCV for image processing
import serial # PySerial for USB communication
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.clock import Clock
from kivy.graphics.texture import Texture
from kivy.uix.image import Image

# USB CONFIGURATION
# Connects to the Arduino/Teensy controlling the Stepper Motor
try:
    # Android usually mounts USB OTG devices here, but this path varies
    ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
except:
    print("USB Motor Controller not found. Running in simulation mode.")
    ser = None

class TelescopeCam(Image):
    def __init__(self, **kwargs):
        super(TelescopeCam, self).__init__(**kwargs)
        self.capture = cv2.VideoCapture(0) # 0 is usually the USB Capture Card input
        Clock.schedule_interval(self.update, 1.0 / 30.0) # 30 FPS
        self.roi_box = None # Region of Interest (Where you tapped)

    def update(self, dt):
        ret, frame = self.capture.read()
        if ret:
            # If we are autofocusing, draw a box around the target
            if self.roi_box:
                x, y, w, h = self.roi_box
                cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
                
                # 1. CALCULATE SHARPNESS (Contrast Detection) 
                # We crop the video to just the area you tapped
                roi_frame = frame[y:y+h, x:x+w]
                gray = cv2.cvtColor(roi_frame, cv2.COLOR_BGR2GRAY)
                # variance of Laplacian is a standard measure of "focus/sharpness"
                score = cv2.Laplacian(gray, cv2.CV_64F).var()
                
                # 2. SEND COMMAND TO MOTOR
                self.autofocus_logic(score)

            # Convert to Kivy Texture for display
            buf1 = cv2.flip(frame, 0)
            buf = buf1.tostring()
            image_texture = Texture.create(size=(frame.shape[1], frame.shape[0]), colorfmt='bgr')
            image_texture.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
            self.texture = image_texture

    def on_touch_down(self, touch):
        # 3. INTERACTION: TAP TO FOCUS [cite: 185]
        # Translate screen tap coordinates to video coordinates
        print(f"User tapped at: {touch.x}, {touch.y}")
        
        # Define a 100x100 pixel box around the tap to analyze
        # This tells the system: "Make THIS spot sharp" [cite: 197]
        self.roi_box = (int(touch.x) - 50, int(touch.y) - 50, 100, 100)
        return super(TelescopeCam, self).on_touch_down(touch)

    def autofocus_logic(self, current_score):
        # A simple "Hill Climbing" algorithm
        # If score is getting better, keep moving motor same way. 
        # If score drops, reverse direction.
        
        # Pseudo-code for USB Command
        if ser:
            # Example: Send "STEP_FORWARD" command
            # You would implement the full PID loop here
            ser.write(b'F+') 

class NavyScopeApp(App):
    def build(self):
        return TelescopeCam()

if __name__ == '__main__':
    NavyScopeApp().run()
