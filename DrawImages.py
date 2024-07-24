if __name__ == "__main__":
    import Main
    exit()

from DrawFunctions import *
import Colors

def DrawMainMenuBuild(backgroundColor):
    drawRect(12, 150, 80, 8,  Colors.colors["WHITE"], True) # Bed

    # Draw print head
    drawTriangle(28,120,48,140,48,120,Colors.colors["WHITE"], True)
    drawRect(48,120,10,21,Colors.colors["WHITE"],True)
    drawTriangle(78,120,58,140,58,120,Colors.colors["WHITE"], True)
    drawRect(39, 85, 30, 35, Colors.colors["WHITE"], True)
    
    # Draw head cutout
    drawRect(48, 85, 12, 48, [200,200,200], True)

    # Draw filament
    drawRect(52, 85, 4, 40, Colors.colors["WHITE"], True)

def DrawMainMenuPreheat(backgroundColor):
    drawRect(118, 150, 80, 8, Colors.colors["WHITE"], True) # Bed
    drawArrow(130, 80, 60, 6, Colors.colors["WHITE"])       # Left arrow
    drawArrow(155, 80, 60, 6, Colors.colors["WHITE"])       # Middle arrow
    drawArrow(180, 80, 60, 6, Colors.colors["WHITE"])       # Right arrow

def DrawMainMenuTools(backgroundColor):
    # Draw spanner
    drawRotatedRect(260,110,15,80, 30,  [200, 200, 200], True)
    drawCircle(240,145,14,              [200, 200, 200], True)
    drawCircle(280,75,16,               [200, 200, 200], True)

    # Cut out spanner
    drawCircle(240,145,7,               backgroundColor, True)
    drawRect(280,67, 17, 17,            backgroundColor, True)
    return


