if __name__ == "__main__":
    import Main
    exit()

from ArduinoCommunicationsInterface import Arduino

def get_arduino_instance():
    return Arduino(port="COM4", baudrate=115200)
