import matplotlib.pyplot as plt
import matplotlib.animation as animation
from ArduinoCommunicationsInterface import Arduino

# Initialize Arduino instance (replace with your actual port and baudrate)
arduino = Arduino(port='COM4', baudrate=115200)

# Function to get data from Arduino
def get_data():
    x, y, touched = arduino.read_touch_input()
    arduino.clear_buffer()
    
    if touched is not None:
        return x, 240 - y
    return 0, 0

# Set up the plot
fig, ax = plt.subplots()
crosshair, = ax.plot([], [], marker='+', color='red', markersize=10)  # Crosshair
coord_text = ax.text(5, 5, '', color='blue')  # Coordinates text

ax.set_xlim(0, 320)  # Set according to your screen resolution
ax.set_ylim(0, 240)  # Set according to your screen resolution

def update(frame):
    x, y = get_data()
    if x is not None and y is not None:
        crosshair.set_data([x], [y])
        coord_text.set_text(f'({x}, {y})')
        coord_text.set_position((x + 5, y + 5))  # Adjust text position
    return crosshair, coord_text

ani = animation.FuncAnimation(fig, update, interval=1)  # Set interval to 1 ms
plt.show()
