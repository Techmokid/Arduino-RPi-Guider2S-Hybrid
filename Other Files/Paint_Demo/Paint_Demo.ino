#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <stdint.h>
#include "TouchScreen.h"

#define BLACK   0x0000
#define YELLOW  RGBTo565(255, 255, 0)

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;
uint16_t TFT_ID;

uint16_t RGBTo565(uint8_t red, uint8_t green, uint8_t blue) {
    uint16_t r = (red >> 3) << 11;
    uint16_t g = (green >> 2) << 5;
    uint16_t b = blue >> 3;
    return r | g | b;
}

int X_Max = 0;
int X_Min = 1000;
int Y_Max = 0;
int Y_Min = 1000;
void setup(void) {
  Serial.begin(9600);

  TFT_ID = tft.readID();
  tft.begin(TFT_ID);
  tft.setRotation(1); 
  tft.fillScreen(BLACK);
  tft.setFont(NULL);
}

void loop(void) {
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);

  if (p.z <= 0) { return; }
  
  if (X_Max < p.x) { X_Max = p.x; }
  if (Y_Max < p.y) { Y_Max = p.y; }
  if (X_Min > p.x) { X_Min = p.x; }
  if (Y_Min > p.y) { Y_Min = p.y; }
  int Point_X = map(p.x, 960, 115, 0, 320);
  int Point_Y = map(p.y, 920, 102, 0, 240);
  
  Serial.print("X = ");
  Serial.print(X_Min);
  Serial.print(" : ");
  Serial.print(X_Max);
  Serial.print("\t\t\t");

  Serial.print("Y = ");
  Serial.print(Y_Min);
  Serial.print(" : ");
  Serial.println(Y_Max);

  tft.fillCircle(Point_X, Point_Y, 7, YELLOW);
  delay(10);
  tft.fillCircle(Point_X, Point_Y, 7, BLACK);
}
