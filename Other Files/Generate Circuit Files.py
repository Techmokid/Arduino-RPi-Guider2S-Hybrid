saveDir = "D:/Circuit Drawing/"
fileext = "bmp"






# FUNCTIONS ------------------------------------------------------------------------------
from PIL import Image, ImageDraw
import os,shutil

scale = 0
im = None
draw = None
standardSpacing = 2.54
arduinoDir = saveDir + "Arduinos/"
ICsDir = saveDir + "ICs/"

def drawFilledCircle(radius, X, Y, col):
    radius *= scale
    X *= scale
    Y *= scale
    left_up_point = (X - radius, Y - radius)
    right_down_point = (X + radius, Y + radius)
    draw.ellipse([left_up_point, right_down_point], fill=col)

def drawPinHole(X,Y):
    drawFilledCircle(1.02, X, Y, (0,0,0))
    drawFilledCircle(0.35, X, Y, (255,255,255))

def createPinArray(X,Y,X_Count,Y_Count):
    for x in range(0,X_Count):
        for y in range(0,Y_Count):
            drawPinHole(X + x*standardSpacing,Y + y*standardSpacing)

def getReqSize(count):
    return (1000 + 1245*(count - 1))






# Code -----------------------------------------------------------------------------------
print("Initializing directory: " + saveDir)
if os.path.exists(saveDir):
    shutil.rmtree(saveDir)
if not os.path.exists(saveDir):
    os.makedirs(saveDir, exist_ok=True)
if not os.path.exists(saveDir + "ICs/"):
    os.makedirs(arduinoDir, exist_ok=True)
if not os.path.exists(saveDir + "ICs/"):
    os.makedirs(ICsDir, exist_ok=True)

X = 1.02
Y = 1.02
scale = 49
print("Variables set")

#Create Arduino Uno Board Pins
print()
print("Arduino Boards:")
im = Image.new(mode = "RGB", size = (6000,2465), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,10,1)
createPinArray(X + 10.5*standardSpacing,Y,8,1)
createPinArray(X + 17*standardSpacing, Y + 8*standardSpacing, 2, 3)
createPinArray(X + 12.5*standardSpacing,Y + 19*standardSpacing,6,1)
createPinArray(X + 3.5*standardSpacing,Y + 19*standardSpacing,8,1)
im.save(arduinoDir + "Mega." + fileext)
print("FAILED: Arduino Mega Board")

im = Image.new(mode = "RGB", size = (2340,2465), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,10,1)
createPinArray(X + 10.5*standardSpacing,Y,8,1)
createPinArray(X + 17*standardSpacing, Y + 8*standardSpacing, 2, 3)
createPinArray(X + 12.5*standardSpacing,Y + 19*standardSpacing,6,1)
createPinArray(X + 3.5*standardSpacing,Y + 19*standardSpacing,8,1)
im.save(arduinoDir + "Uno." + fileext)
print("Created Arduino Uno/Leonardo Board")

im = Image.new(mode = "RGB", size = (1842,845), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,15,1)
createPinArray(X,Y + 6*standardSpacing,15,1)
im.save(arduinoDir + "Nano." + fileext)
print("Created Arduino Nano Board")

im = Image.new(mode = "RGB", size = (1842,1345), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,15,1)
createPinArray(X,Y + 10*standardSpacing,15,1)
im.save(arduinoDir + "ESP32." + fileext)
print("Created ESP32 Board")

im = Image.new(mode = "RGB", size = (972,1222), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,8,1)
createPinArray(X,Y + 9*standardSpacing,8,1)
im.save(arduinoDir + "WEMOS D1 Mini ESP8266." + fileext)
print("Created ESP8266 Board")


scale = 490

#Create Basic Pins
print()
print("Basic pins:")
im = Image.new(mode = "RGB", size = (1000,1000), color = (255,255,255))
draw = ImageDraw.Draw(im)
drawPinHole(X,Y)
im.save(saveDir + "Basic Circle." + fileext)

print("Created Basic Circle")

for i in range(2,11):
    im = Image.new(mode = "RGB", size = (2245 + 1245*(i-2),1000), color = (255,255,255))
    draw = ImageDraw.Draw(im)
    createPinArray(X,Y,i,1)
    im.save(saveDir + str(i) + " Pin." + fileext)
    print("Created " + str(i) + " Pin")

#Create ICs
print()
print("ICs:")
for i in range(6,30,2):
    im = Image.new(mode = "RGB", size = (3490 + 1245*int(i/2 - 3),4735), color = (255,255,255))
    draw = ImageDraw.Draw(im)
    createPinArray(X,Y,int(i/2),1)
    createPinArray(X,Y + 3*standardSpacing,int(i/2),1)
    im.save(ICsDir + str(i) + " Pin IC." + fileext)
    print("Created " + str(i) + " Pin IC")
print()

#Create RPi GPIO
print("Other:")
im = Image.new(mode = "RGB", size = (getReqSize(20),getReqSize(2)), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,20,2)
im.save(saveDir + "RPi GPIO." + fileext)
print("Created RPi GPIO")

im = Image.new(mode = "RGB", size = (getReqSize(8),getReqSize(6)), color = (255,255,255))
draw = ImageDraw.Draw(im)
createPinArray(X,Y,8,1)
createPinArray(X,Y + 5*standardSpacing,8,1)
im.save(saveDir + "A4988 Stepper Module." + fileext)
print("Created A4988 Stepper Module")
print()




















#im.show()
print("Completed all generation")
