if __name__ == "__main__":
    import Main
    exit()

from ArduinoInstance import get_arduino_instance
from Button import *
from DrawFunctions import *
from DrawImages import *

buttonsHandler = Buttons()

variablesDictionary = {}
variablesDictionary["PreheatMenu_BedTemp"] = 60
variablesDictionary["PreheatMenu_HeadTemp"] = 215
variablesDictionary["PreheatMenu_DoHeadHeat"] = True
variablesDictionary["PreheatMenu_DoBedHeat"] = True








def cleanupScreenForNextDraw():
    global buttonsHandler
    buttonsHandler.clear_buttons()
    arduino.buffer = []
    clearScreen()
    drawTopBar()











def drawMainMenu():
    global buttonsHandler
    
    cleanupScreenForNextDraw()
    arduino.reconnectFunction = drawMainMenu
    
    buttonsHandler.add_button(5, 50, 95, 170, 18, 135,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "Build",
                           None,
                           lambda backgroundColor: DrawMainMenuBuild(backgroundColor))
    buttonsHandler.add_button(110, 50, 95, 170, 5, 135,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "Preheat",
                           lambda: drawPreheatMenu(),
                           lambda backgroundColor: DrawMainMenuPreheat(backgroundColor))
    buttonsHandler.add_button(215, 50, 95, 170, 18, 135,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "Tools",
                           None,
                           lambda backgroundColor: DrawMainMenuTools(backgroundColor))







    
    
def drawDigitPadHead():
    global variablesDictionary
    variablesDictionary["PreheatMenu_CurrentTempType"] = "head"
    drawDigitPad()
def drawDigitPadBed():
    global variablesDictionary
    variablesDictionary["PreheatMenu_CurrentTempType"] = "bed"
    drawDigitPad()
def drawDigitPad_HandleButton_PLUS(): # INCOMPLETE
    button = variablesDictionary["PreheatMenu_TempAdjust_Button" + variablesDictionary["PreheatMenu_TempAdjust_CurrentButton"]]
    newNumber = int(button.text) + 1
    if newNumber > 9:
        newNumber = 0
    if newNumber < 0:
        newNumber = 9
    button.text = str(newNumber)
    button.draw()
def drawDigitPad_HandleButton_MINUS(): # INCOMPLETE
    button = variablesDictionary["PreheatMenu_TempAdjust_Button" + variablesDictionary["PreheatMenu_TempAdjust_CurrentButton"]]
    newNumber = int(button.text) - 1
    if newNumber > 9:
        newNumber = 0
    if newNumber < 0:
        newNumber = 9
    button.text = str(newNumber)
    button.draw()
def drawDigitPad_HandleButton_A():
    variablesDictionary["PreheatMenu_TempAdjust_CurrentButton"] = "A"
def drawDigitPad_HandleButton_B():
    variablesDictionary["PreheatMenu_TempAdjust_CurrentButton"] = "B"
def drawDigitPad_HandleButton_C():
    variablesDictionary["PreheatMenu_TempAdjust_CurrentButton"] = "C"
def drawDigitPad_Finish():
    result = variablesDictionary["PreheatMenu_TempAdjust_ButtonA"].text
    result += variablesDictionary["PreheatMenu_TempAdjust_ButtonB"].text
    result += variablesDictionary["PreheatMenu_TempAdjust_ButtonC"].text
    result = int(result)
    
    if variablesDictionary["PreheatMenu_CurrentTempType"].lower() in ["head","extruder"]:
        variablesDictionary["PreheatMenu_HeadTemp"] = result
    elif variablesDictionary["PreheatMenu_CurrentTempType"].lower() in ["bed","plate"]:
        variablesDictionary["PreheatMenu_BedTemp"] = result
    else:
        raise Exception("Digit pad temp type error")
    
    drawPreheatMenu()
def drawDigitPad():
    global buttonsHandler, variablesDictionary
    cleanupScreenForNextDraw()
    arduino.reconnectFunction = drawDigitPad
    
    temp = 0
    if variablesDictionary["PreheatMenu_CurrentTempType"].lower() in ["head","extruder"]:
        temp = variablesDictionary["PreheatMenu_HeadTemp"]
    elif variablesDictionary["PreheatMenu_CurrentTempType"].lower() in ["bed","plate"]:
        temp = variablesDictionary["PreheatMenu_BedTemp"]
    else:
        raise Exception("Digit pad temp type error")
    
    drawText(85, 60, 2, "Temp Adjust", Colors.colors["WHITE"])
    tripletDigit = "0"*(3-len(str(temp))) + str(temp)
    buttonsHandler.add_button(35, 100, 50, 50, 20, 18,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "-",
                           lambda: drawDigitPad_HandleButton_MINUS(),
                           None)
    variablesDictionary["PreheatMenu_TempAdjust_ButtonA"] = buttonsHandler.add_button(
                           89, 100, 45, 50, 18, 18,
                           [100,100,150], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           tripletDigit[0],
                           lambda: drawDigitPad_HandleButton_A(),
                           None)
    variablesDictionary["PreheatMenu_TempAdjust_ButtonB"] = buttonsHandler.add_button(
                           138, 100, 45, 50, 18, 18,
                           [100,100,150], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           tripletDigit[1],
                           lambda: drawDigitPad_HandleButton_B(),
                           None)
    variablesDictionary["PreheatMenu_TempAdjust_ButtonC"] = buttonsHandler.add_button(
                           187, 100, 45, 50, 18, 18,
                           [100,100,150], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           tripletDigit[2],
                           lambda: drawDigitPad_HandleButton_C(),
                           None)
    buttonsHandler.add_button(236, 100, 50, 50, 20, 18,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "+",
                           lambda: drawDigitPad_HandleButton_PLUS(),
                           None)

    # Navigation buttons
    buttonsHandler.add_button(0, 200, 320, 40, 130, 12,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "Return",
                           lambda: drawDigitPad_Finish(),
                           None)
    variablesDictionary["PreheatMenu_TempAdjust_CurrentButton"] = "A"












def PreheatMenu_ToggleHeadHeat(newState):
    variablesDictionary["PreheatMenu_DoHeadHeat"] = newState
def PreheatMenu_ToggleBedHeat(newState):
    variablesDictionary["PreheatMenu_DoBedHeat"] = newState
def drawPreheatMenu():
    global buttonsHandler
    global variablesDictionary

    arduino.reconnectFunction = drawPreheatMenu
    cleanupScreenForNextDraw()

    # Draw extruder
    drawText(20,60, 2, "Extruder: ", [255, 255, 255])
    buttonsHandler.add_radio_button(130, 50, 80, 40,
                                    Colors.colors["LIME"], Colors.colors["RED"],
                                    lambda newState: PreheatMenu_ToggleHeadHeat(newState), variablesDictionary["PreheatMenu_DoHeadHeat"])
    buttonsHandler.add_button(225, 50, 80, 40, 10, 12,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           str(variablesDictionary["PreheatMenu_HeadTemp"]) + " "*(4-len(str(variablesDictionary["PreheatMenu_HeadTemp"]))) + "C",
                           lambda: drawDigitPadHead(),
                           None)

    # Draw bed
    drawText(20,140, 2, "Bed: ", [255, 255, 255])
    buttonsHandler.add_radio_button(130, 130, 80, 40,
                                    Colors.colors["LIME"], Colors.colors["RED"],
                                    lambda newState: PreheatMenu_ToggleBedHeat(newState), variablesDictionary["PreheatMenu_DoBedHeat"])
    buttonsHandler.add_button(225, 130, 80, 40, 10, 12,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           str(variablesDictionary["PreheatMenu_BedTemp"]) + " "*(4-len(str(variablesDictionary["PreheatMenu_BedTemp"]))) + "C",
                           lambda: drawDigitPadBed(),
                           None)

    # Draw navigation buttons
    buttonsHandler.add_button(0, 200, 160, 40, 50, 12,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "Start",
                           lambda: drawPreheatHeatingMenu(),
                           None)
    buttonsHandler.add_button(160, 200, 160, 40, 50, 12,
                           Colors.colors["CYAN"], Colors.colors["WHITE"],
                           Colors.colors["LIME"], Colors.colors["WHITE"],
                           "Back",
                           lambda: drawMainMenu(),
                           None)









def drawPreheatHeatingMenu():
    global buttonsHandler
    global variablesDictionary

    arduino.reconnectFunction = drawPreheatHeatingMenu
    cleanupScreenForNextDraw()

    























