from ArduinoInstance import get_arduino_instance
from Button import Button, Buttons
import Colors

print("Arduino communications opening...")
arduino = get_arduino_instance()
print("Arduino communications open. Waiting for wakeup signal")

arduino.read_serial()
for i in arduino.buffer:
    if i.split(":")[0] == "AWAKE":
        print("Arduino awake and ready")
arduino.buffer = []


def buttonResponseFunction(msg):
    global buttonsHandler
    buttonsHandler.forceDrawButtons()
    print(msg)

# Start of buttons script
buttonsHandler = Buttons()
buttonsHandler.add_button(5, 50, 95, 170, 18, 70,
                   Colors.colors["CYAN"], Colors.colors["WHITE"],
                   Colors.colors["LIME"], Colors.colors["WHITE"],
                   "Build", lambda: buttonResponseFunction("You selected Build!"), None
                   )
buttonsHandler.add_button(110, 50, 95, 170, 5, 70,
                   Colors.colors["CYAN"], Colors.colors["WHITE"],
                   Colors.colors["LIME"], Colors.colors["WHITE"],
                   "Preheat", lambda: buttonResponseFunction("You selected Preheat!"), None
                   )
buttonsHandler.add_button(215, 50, 95, 170, 18, 70,
                   Colors.colors["CYAN"], Colors.colors["WHITE"],
                   Colors.colors["LIME"], Colors.colors["WHITE"],
                   "Tools", lambda: buttonResponseFunction("You selected Tools!"), None
                   )
def getTouchSpot():
    x, y, touched = arduino.read_touch_input()
    arduino.clear_buffer()
    if touched is not None:
        return x,y,touched    # If touched is true, we are pressing and holding. If touched is false, then this is the exact frame we let go of the button on the touchscreen
    return -1,-1,False        # If we return this -1,-1,False, this is telling us there is no touch data, and hence we haven't touched the screen for more than a few frames

print("Running script...")
while True:
    buttonsHandler.process(getTouchSpot())
