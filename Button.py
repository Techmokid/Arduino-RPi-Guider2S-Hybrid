if __name__ == "__main__":
    import Main
    exit()

from DrawFunctions import *

class Button:
    def __init__(self,
                 x, y, width, height, textX, textY,
                 color, text_color,
                 pressed_color, pressed_text_color,
                 text, press_function, draw_image_function):
        if width <= 0:
            raise Exception("Invalid radio button params, hence cannot create button. Width <= 0")
        if height <= 0:
            raise Exception("Invalid radio button params, hence cannot create button. Height <= 0")
        
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.textX = textX
        self.textY = textY
        self.color = color
        self.text = text
        self.text_color = text_color
        self.pressed_color = pressed_color
        self.pressed_text_color = pressed_text_color
        self.press_function = press_function
        self.draw_image_function = draw_image_function
        self.prevPressed = False
        
        self.outlineOnly = False
        self.textSize = 2

    def draw(self):
        drawRect(self.x, self.y, self.width, self.height, self.color, True)
        drawText(self.textX + self.x, self.textY + self.y, 2, self.text, self.text_color)
        
        if self.draw_image_function is not None:
            self.draw_image_function(self.color)

    def draw_pressed(self):
        if self.outlineOnly:
            drawRect(self.x, self.y, 5, self.height, self.pressed_color, True)
            drawRect(self.x+self.width-5, self.y, 5, self.height, self.pressed_color, True)
            drawRect(self.x, self.y, self.width, 5, self.pressed_color, True)
            drawRect(self.x, self.y+self.height-5, self.width, 5, self.pressed_color, True)
        else:
            drawRect(self.x, self.y, self.width, self.height, self.pressed_color, True)
        
        drawText(self.textX + self.x, self.textY + self.y, self.textSize, self.text, self.pressed_text_color)
        if self.draw_image_function is not None:
            if self.outlineOnly:
                self.draw_image_function(self.color)
            else:
                self.draw_image_function(self.pressed_color)
        

class RadioButton:
    def __init__(self, x, y, width, height, activeColor, inactiveColor, userDef_press_function, state=False):
        if width <= 0:
            raise Exception("Invalid radio button params, hence cannot create button. Width <= 0")
        if height <= 0:
            raise Exception("Invalid radio button params, hence cannot create button. Height <= 0")
        if not isinstance(state,bool):
            raise Exception("Invalid radio button params, hence cannot create button. State not boolean value")
        if height >= width:
            raise Exception("Invalid radio button params, hence cannot create button. Height >= Width")
        
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.activeColor = activeColor
        self.inactiveColor = inactiveColor
        self.state = state
        self.userDef_press_function = userDef_press_function
        
        self.prevPressed = False

    def draw(self):
        # Draw shell
        lineWidth = 4
        halfHeight = self.height/2
        drawCircle(self.x + halfHeight, self.y + halfHeight, halfHeight, Colors.colors["WHITE"], True)
        drawCircle(self.x + self.width - halfHeight, self.y + halfHeight, halfHeight, Colors.colors["WHITE"], True)
        drawRect(self.x + halfHeight, self.y, self.width - self.height, self.height+1, Colors.colors["WHITE"], True)

        # Draw statusColor
        statusColor = self.inactiveColor
        if self.state:
            statusColor = self.activeColor
        
        drawCircle(self.x + halfHeight, self.y + halfHeight, halfHeight-lineWidth, statusColor, True)
        drawCircle(self.x + self.width - halfHeight, self.y + halfHeight, halfHeight-lineWidth, statusColor, True)
        drawRect(self.x + halfHeight, self.y+lineWidth, self.width - self.height, self.height+1-2*lineWidth, statusColor, True)
        
        if not self.state:
            drawCircle(self.x + halfHeight, self.y + halfHeight, halfHeight-2*lineWidth, Colors.colors["WHITE"], True)
        else:
            drawCircle(self.x + self.width - halfHeight, self.y + halfHeight, halfHeight-2*lineWidth, Colors.colors["WHITE"], True)
    def draw_pressed(self):
         self.draw() # Ignore this. This is compatibility for the other button type. Just draw normally for now

    def press_function(self):
        self.state = not self.state
        self.draw()

        if self.userDef_press_function is not None:
            self.userDef_press_function(self.state)

class Buttons:
    def __init__(self):
        self.buttons_list = []

    def clear_buttons(self):
        self.buttons_list = []

    def add_preexisting_button(self, button):
        self.buttons_list.append(button)
        
    def add_preexisting_radio_button(self, button):
        self.buttons_list.append(button)
        
    def add_button(self, x, y, width, height, textX, textY, color, text_color, pressed_color, pressed_text_color, text, press_function, draw_image_function):
        button = Button(x, y, width, height, textX, textY, color, text_color, pressed_color, pressed_text_color, text, press_function, draw_image_function)
        self.buttons_list.append(button)
        button.draw()
        return button

    def add_radio_button(self, x, y, width, height, activeColor, inactiveColor, userDef_press_function, state=False):
        button = RadioButton(x, y, width, height, activeColor, inactiveColor, userDef_press_function, state)
        self.buttons_list.append(button)
        button.draw()
        return button

    def is_touched(self, button, touch_x, touch_y):
        return button.x <= touch_x <= button.x + button.width and button.y <= touch_y <= button.y + button.height

    def process(self, touch_spot):
        touch_x, touch_y, touched = touch_spot
        if touched is None:
            return
        
        for b in self.buttons_list:
            if touched: # We are pressing and holding the button
                if self.is_touched(b, touch_x, touch_y):
                    if not b.prevPressed:
                        b.prevPressed = True
                        b.draw_pressed()
                else:                        # If we are not pressing the button, but it was previously pressed, redraw it as normal
                    if b.prevPressed:
                        b.prevPressed = False
                        b.draw()
            else:
                if self.is_touched(b, touch_x, touch_y):
                    if b.press_function is not None:
                        b.press_function()
                    else:
                        if b is isinstance(b,Button):
                            print("Error: No function was set for button \"" + b.text + "\"")
                        b.draw()
                        b.prevPressed = False

    def forceDrawButtons(self):
        for i in self.buttons_list:
            i.draw()
