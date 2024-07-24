from ArduinoInstance import get_arduino_instance

arduino = get_arduino_instance()

arduino.read_serial()
for i in arduino.buffer:
    if i.split(":")[0] == "AWAKE":
        print("Arduino awake and ready")
arduino.buffer = []

while True:
    arduino.read_serial()
    if len(arduino.buffer) > 0:
        command = arduino.buffer.pop(0)
        print(command)
