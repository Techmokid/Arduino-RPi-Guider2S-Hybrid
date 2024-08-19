import time
import random
import Colors
from ArduinoInstance import get_arduino_instance
from DataInstance import get_data_instance
from DrawMenus import *

DataObject = get_data_instance()
print("Arduino communications opening...")
arduino = get_arduino_instance()
print("Arduino communications open. Waiting for wakeup signal")

searchingForAwakeSignal = True
while searchingForAwakeSignal:
    arduino.read_serial()
    
    for i in arduino.buffer:
        if i.split(":")[0] == "AWAKE":
            searchingForAwakeSignal = False
            print("Arduino awake and ready")
        else:
            print(i)
    arduino.buffer = []

    if searchingForAwakeSignal:
        time.sleep(0.5)
        print("Error talking to arduino. Buffer contents: ")
        print(arduino.buffer)
        print()
print("Arduino awake")

def getTouchSpot():
    x, y, touched = arduino.read_touch_input()
    arduino.clear_buffer()
    if touched is not None:
        return x,y,touched    # If touched is true, we are pressing and holding. If touched is false, then this is the exact frame we let go of the button on the touchscreen
    return -1,-1,False        # If we return this -1,-1,False, this is telling us there is no touch data, and hence we haven't touched the screen for more than a few frames

drawMainMenu()
while True:
    drawTopBar_InternetConnectivity(force=False)
    buttonsHandler.process(getTouchSpot())
    DataObject.update()
    
