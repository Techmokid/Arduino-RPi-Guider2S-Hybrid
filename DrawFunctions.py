if __name__ == "__main__":
    import Main
    exit()

from ArduinoInstance import get_arduino_instance
import Colors
import random
import time
import socket
import psutil

arduino = get_arduino_instance()
NetworkStatus = 0
PreviousNetworkStatus = 0

header_titles = ["Fleshforge","Fleshfart","Florshfert","Flashfert","Flashnt","Fleshnt","Blinkforge", "Crafty Cortex", "Pixel Pioneer", "GizmoGrafter", "Pi-lot Print", "Arpino Gizmo"]
header_title = random.choice(header_titles)

def check_network_connection():
    global NetworkStatus
    
    network_connection = 0  # Assume no connection initially

    local_connected = any(snic.family == socket.AF_INET and not snic.address.startswith("127.") 
                          for adapter in psutil.net_if_addrs().values() 
                          for snic in adapter)

    if not local_connected:
        return network_connection

    network_connection = 1  # Local network connected, but no internet confirmed yet

    try:
        host = socket.gethostbyname("www.google.com")
        s = socket.create_connection((host, 80), 2)
        s.close()
        network_connection = 2  # Internet access confirmed
    except:
        pass
    return network_connection

def draw_shape(shape, x, y, width, height=None, col=None, fill=False):
    msg = f"{'F' if fill else ''}{shape}:{x}:{y}:{width}"
    if height is not None:
        msg += f":{height}"
    if col is not None:
        msg += f":{col[0]}:{col[1]}:{col[2]}"
    arduino.send_command(msg)

def drawCircle(x, y, radius, col, fill):
    draw_shape("CIRCLE", x, y, radius, col=col, fill=fill)

def drawSquare(x, y, size, col, fill):
    draw_shape("SQUARE", x, y, size, col=col, fill=fill)

def drawRect(x, y, width, height, col, fill):
    draw_shape("RECT", x, y, width, height=height, col=col, fill=fill)

def drawText(x, y, size, msg, col):
    arduino.send_command(f"TEXT:{x}:{y}:{size}:{msg}:{col[0]}:{col[1]}:{col[2]}")

def drawRotatedRect(x, y, width, height, angle, col, fill):
    #draw_shape("ROT", x, y, width, height=height, col=col, fill=fill)
    msg = f"ROT:{x}:{y}:{width}:{height}:{angle}:{col[0]}:{col[1]}:{col[2]}"
    if fill:
        msg = "F" + msg
    arduino.send_command(msg)
 
def drawTriangle(x0, y0, x1, y1, x2, y2, col, fill):
    msg = f"TRIG:{x0}:{y0}:{x1}:{y1}:{x2}:{y2}:{col[0]}:{col[1]}:{col[2]}"
    if fill:
        msg = "F" + msg
    arduino.send_command(msg)

def drawArrow(arrowX, arrowY, arrowHeight, arrowWidth, col):
    drawRect(arrowX, arrowY, arrowWidth, arrowHeight,  Colors.colors["WHITE"], True)
    drawTriangle(
        arrowX-5, arrowY, #Left of arrow head
        arrowX+arrowWidth+5, arrowY, #Right of arrow head
        arrowX+arrowWidth/2, arrowY-10, #Top of arrow head
        Colors.colors["WHITE"], True)

def fillScreen(col):
    arduino.send_command(f"FRECT:0:0:320:240:{col[0]}:{col[1]}:{col[2]}")

def clearScreen():
    fillScreen([0, 0, 0])







def drawTopBar_InternetConnectivity(force=True):
    global NetworkStatus, PreviousNetworkStatus
    NetworkStatus = check_network_connection()
    if NetworkStatus == PreviousNetworkStatus:
        if not force:
            return
    PreviousNetworkStatus = NetworkStatus
    
    drawRect(289, 5, 25, 20, [120, 120, 120], True)
    drawRect(291, 7, 21, 15, Colors.colors["BLACK"], True)
    if NetworkStatus == 0:
        drawRotatedRect(301, 14, 2, 30, 55, Colors.colors["RED"], True)
        drawRotatedRect(301, 14, 2, 30, -55, Colors.colors["RED"], True)
    elif NetworkStatus == 1:
        drawText(296, 8, 2, "?", Colors.colors["ORANGE"])
    elif NetworkStatus == 2:
        drawRotatedRect(305, 14, 2, 15, 55, Colors.colors["GREEN"], True)
        drawRotatedRect(298, 17, 2, 5, -55, Colors.colors["GREEN"], True)
    
def drawTopBar():
    drawRect(0, 0, 320, 30, [30, 30, 30], True)
    #drawRect(0, 30, 320, 210, [0, 0, 0], True)
    drawRotatedRect(16, 16, 13, 13, 45, [200, 200, 200], True)
    drawTriangle(17, 5, 17, 26, 27, 15, [100, 100, 100], True)
    drawRect(12, 13, 5, 6, [120, 0, 120], True)
    drawRect(17, 13, 5, 6, [0, 200, 120], True)
    drawText(45, 8, 2, header_title, Colors.colors["WHITE"])
    drawTopBar_InternetConnectivity()
