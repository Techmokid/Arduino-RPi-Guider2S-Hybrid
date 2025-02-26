#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <stdint.h>
#include "TouchScreen.h"
#include <FreeDefaultFonts.h>
#include <LinkedList.h>
#include <math.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define AUDIO_PIN 51
#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define RPI_SERIAL_PORT Serial1

// -----------------------------------------------------------
#if defined(RPI_SERIAL_PORT) && (RPI_SERIAL_PORT != Serial)
  #define DO_SERIAL_DIAGS
#else
  #ifdef DO_SERIAL_DIAGS
    #undef DO_SERIAL_DIAGS
  #endif
#endif

String PrevMenuScreen = "Main Menu";
String MenuScreen = "Main Menu";

MCUFRIEND_kbv tft;
uint16_t TFT_ID;

const int TS_LEFT=876,TS_RT=139,TS_TOP=390,TS_BOT=769;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
int Cursor_X, Cursor_Y;
bool Cursor_Pressed;
bool Touch_getXY(void) {
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    Cursor_X = map(p.x, 960, 115, 0, 320);
    Cursor_Y = map(p.y, 920, 102, 0, 240);
  }
  return pressed;
}

// Colors are in 5-6-5 binary format (16 bits total)
// 5 bits are red
// 6 bits are green
// 5 bits are blue
#define BLACK   0x0000
#define BLUE    RGBTo565(0,   0,   255)
#define RED     RGBTo565(255, 0,   0)
#define GREEN   RGBTo565(0,   255, 0)
#define CYAN    RGBTo565(0,   255, 255)
#define MAGENTA RGBTo565(255, 0,   255)
#define YELLOW  RGBTo565(255, 255, 0)
#define WHITE   RGBTo565(255, 255, 255)

uint16_t RGBTo565(uint8_t red, uint8_t green, uint8_t blue) {
    // Scale down and shift each color component
    uint16_t r = (red >> 3) << 11;   // Red: shift right by 3, then left by 11
    uint16_t g = (green >> 2) << 5;  // Green: shift right by 2, then left by 5
    uint16_t b = blue >> 3;          // Blue: shift right by 3
    return r | g | b;
}

struct CustomSentence {
  int x,y;
  String msg;
  int scale;

  CustomSentence(int _X, int _Y, int _scale, String _msg) 
        : x(_X), y(_Y), scale(_scale), msg(_msg) {
        // Constructor body, if needed
  }

  void Run(uint16_t col) {
    tft.setTextColor(col);
    tft.setCursor(x, y);
    tft.setTextSize(scale);
    tft.print(msg);
  }

  void Wipe() {
    Run(BLACK);
  }

  void Replace(uint16_t col, String _msg) {
    Wipe();
    msg = _msg;
    Run(col);
  }
};

void rotatePoint(int pivotX, int pivotY, int &x, int &y, float angleRad) {
    float s = sin(angleRad);
    float c = cos(angleRad);

    // Translate point back to origin
    x -= pivotX;
    y -= pivotY;

    // Rotate point
    float xNew = x * c - y * s;
    float yNew = x * s + y * c;

    // Translate point back
    x = xNew + pivotX;
    y = yNew + pivotY;
}

void drawRotatedRect(int centerX, int centerY, int width, int height, float angleDeg, uint16_t color) {
    float angleRad = angleDeg * M_PI / 180.0; // Convert to radians

    // Calculate the half dimensions
    int halfW = width / 2;
    int halfH = height / 2;

    // Calculate original corner coordinates (before rotation)
    int x0 = centerX - halfW;
    int y0 = centerY - halfH;
    int x1 = centerX + halfW;
    int y1 = centerY - halfH;
    int x2 = centerX + halfW;
    int y2 = centerY + halfH;
    int x3 = centerX - halfW;
    int y3 = centerY + halfH;

    // Rotate each corner around the center
    rotatePoint(centerX, centerY, x0, y0, angleRad);
    rotatePoint(centerX, centerY, x1, y1, angleRad);
    rotatePoint(centerX, centerY, x2, y2, angleRad);
    rotatePoint(centerX, centerY, x3, y3, angleRad);

    // Draw the two triangles
    tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
    tft.fillTriangle(x0, y0, x2, y2, x3, y3, color);
}

struct CustomTickBox {
  int x,y,w,h;
  int state = 0;

  CustomTickBox(int _X,int _Y,int _W,int _H) 
        : x(_X), y(_Y), w(_W), h(_H) {
        // Constructor body, if needed
  }

  void UpdateStatus(int newStatus) {
    if (state == newStatus) { return; }

    state = newStatus;
    tft.fillRect(x, y, w, h, BLACK);
    tft.drawRect(x, y, w, h, YELLOW);
    int x_center = x + w/2;
    int y_center = y + h/2;
    if (state == 0) {
      // Red X
      tft.setTextSize(w/12);
      tft.setTextColor(RED);
      tft.setCursor(x_center - w/4, y_center - h/3);
      tft.print("X");
    } else if (state == 1) {
      // Green Tick
      drawRotatedRect(x_center - w/6 - w/18, y_center + h/6 - h/15, w/2, h/5, 45, GREEN);
      drawRotatedRect(x_center + w/5 - w/18, y_center       - h/15, w/1.5, h/5, -45, GREEN);
    } else {
      // Yellow Question Mark
      tft.setTextSize(w/12);
      tft.setTextColor(YELLOW);
      tft.setCursor(x_center - w/4, y_center - h/3);
      tft.print("?");
    }
  }
};

String waitForSerialData() {
  String receivedData = "";

  while (RPI_SERIAL_PORT.available() == 0) { }

  char incomingChar = RPI_SERIAL_PORT.read();
  while (incomingChar != '\n') {
    receivedData += incomingChar;
    while (RPI_SERIAL_PORT.available() == 0) { }
    incomingChar = RPI_SERIAL_PORT.read();
  }

  return receivedData;
}




CustomSentence flashprint(0,0,0,"");
uint16_t titleBarCol = RGBTo565(15, 15, 15);

void setup() {
  randomSeed(analogRead(0));
  pinMode(AUDIO_PIN,OUTPUT);

  #ifdef DO_SERIAL_DIAGS
    Serial.begin(115200);
  #endif

  RPI_SERIAL_PORT.begin(115200);

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);

  TFT_ID = tft.readID();
  while (TFT_ID == 54227) {
    digitalWrite(LED_BUILTIN,LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(1000);

    TFT_ID = tft.readID();
  }

  #ifdef DO_SERIAL_DIAGS
    Serial.println("Attempting to wake up display");
  #endif
  tft.begin(TFT_ID);
  tft.setRotation(1); 
  tft.fillScreen(BLACK);
  tft.setFont(NULL);

  #ifdef DO_SERIAL_DIAGS
    Serial.println("Changing over to LCD output on ID: " + String(TFT_ID)); // Ignore Serial, default to TFT from here on
  #endif
  String printerTitleArray[] = {"FlashPants","FleshPants","FleshFarts","FloshPront",""};
  int arraySize = sizeof(printerTitleArray)/sizeof(printerTitleArray[0]);
  flashprint = CustomSentence(60, 11, 1.5, printerTitleArray[random(0, arraySize)]);

  tft.fillCircle(60,150,25,YELLOW);

  CustomSentence connectText(75, 50, 2, "  Connecting... ");
  CustomSentence rpiText(120, 140, 2, "RPi");
  connectText.Run(CYAN);
  rpiText.Run(YELLOW);

  CustomTickBox rpiTickbox(250, 125, 50, 50);
  rpiTickbox.UpdateStatus(2);
  delay(1000);
  
  String serialMsg = waitForSerialData();
  while (serialMsg != "RPi Ready") {
    rpiTickbox.UpdateStatus(0);
    tft.fillCircle(60,150,25,RED);
    delay(3000);
    
    tft.fillCircle(60,150,25,YELLOW);
    serialMsg = waitForSerialData();
  }

  rpiTickbox.UpdateStatus(1);
  tft.fillCircle(60,150,25,GREEN);
  delay(2000);

  doStartupSound();
  RPI_SERIAL_PORT.println("Arduino Ready");
  #ifdef DO_SERIAL_DIAGS
    Serial.println("RPi connected to Arduino successfully");
  #endif

  drawMainMenu();
}

void drawTopBar() {
  tft.fillRect(0, 0, 320, 30, titleBarCol);
  flashprint.Run(WHITE);

  // Draw the image
  tft.fillCircle(15, 7, 4, GREEN);
  tft.fillCircle(15, 7, 2, titleBarCol);
  tft.fillRect(18, 7, 10, 2, GREEN);
  drawRotatedRect(30, 11, 8, 2, 45, GREEN);
  tft.fillRect(32, 13, 8, 2, GREEN);
  tft.fillCircle(44, 14, 4, GREEN);
  tft.fillCircle(44, 14, 2, titleBarCol);
  tft.fillCircle(15, 19, 4, GREEN);
  tft.fillCircle(15, 19, 2, titleBarCol);
  tft.fillRect(18, 19, 10, 2, GREEN);
  drawRotatedRect(30, 17, 8, 2, -45, GREEN);
}

void doStartupSound() {
  for (int i = 0; i < 3; i++) {
    tone(AUDIO_PIN, 1450, 175);
    delay(175 + 10);
  }
  tone(AUDIO_PIN, 1230, 1200);
  delay(1200 + 200);

  for (int i = 0; i < 3; i++) {
    tone(AUDIO_PIN, 1360, 175);
    delay(175 + 10);
  }
  tone(AUDIO_PIN, 1160, 1200);
  delay(1200 + 200);
}

int headTemp = 0;
int bedTemp = 0;
CustomSentence headTempText(260, 9, 2, "H");
CustomSentence bedTempText(290, 9, 2, "B");
void UpdateTempGauges() {
  int R,G,B;
  int cynStrt = 15;
  int grnStrt = 30;
  int yelStrt = 60;
  int redStrt = 100;

  if (headTemp <= cynStrt) {
    R=0;G=255;B=255;
  } else if ((headTemp > cynStrt) && (headTemp <= grnStrt)) {
    R=0;G=255;B=map(headTemp, cynStrt, grnStrt, 255, 0);
  } else if ((headTemp > grnStrt) && (headTemp <= yelStrt)) {
    R=map(headTemp, grnStrt, yelStrt, 0, 255);G=255;B=0;
  } else if ((headTemp > yelStrt) && (headTemp <= redStrt)) {
    R=255;G=map(headTemp, yelStrt, redStrt, 255, 0);B=0; 
  } else {
    R=255;G=0;B=0;
  }
  tft.fillCircle(headTempText.x + 4, 15, 12, RGBTo565(R,G,B));

  cynStrt = 15;
  grnStrt = 25;
  yelStrt = 40;
  redStrt = 75;
  if (bedTemp <= cynStrt) {
    R=0;G=255;B=255;
  } else if ((bedTemp > cynStrt) && (bedTemp <= grnStrt)) {
    R=0;G=255;B=map(bedTemp, cynStrt, grnStrt, 255, 0);
  } else if ((bedTemp > grnStrt) && (bedTemp <= yelStrt)) {
    R=map(bedTemp, grnStrt, yelStrt, 0, 255);G=255;B=0;
  } else if ((bedTemp > yelStrt) && (bedTemp <= redStrt)) {
    R=255;G=map(bedTemp, yelStrt, redStrt, 255, 0);B=0; 
  } else {
    R=255;G=0;B=0;
  }
  tft.fillCircle(bedTempText.x + 4, 15, 12, RGBTo565(R,G,B));

  headTempText.Run(BLACK);
  bedTempText.Run(BLACK);
}

bool touchInBox(int x, int y, int width, int height) {
  if (!Touch_getXY()) { return false; }

  int tX = Cursor_X;
  int tY = Cursor_Y;

  if (tX < x)          { return false; }
  if (tX > x + width)  { return false; }
  if (tY < y)          { return false; }
  if (tY > y + height) { return false; }
  return true;
}

void drawMainMenu() {
  tft.fillScreen(BLACK);
  delay(20);
  drawTopBar();

  int edgeWidth = 20;
  int spacing = 10;
  int width = (360 - 4*edgeWidth - 2*spacing)/3;
  tft.fillRect(edgeWidth, 80, width, 130, RGBTo565(14, 146, 244));
  tft.fillRect(edgeWidth + width + spacing, 80, width, 130, RGBTo565(14, 146, 244));
  tft.fillRect(edgeWidth + 2*width + 2*spacing, 80, width, 130, RGBTo565(14, 146, 244));
  
  CustomSentence buildText =   CustomSentence(32,180,2, "Build");
  CustomSentence preheatText = CustomSentence(118,180,2, "Preheat");
  CustomSentence toolsText =   CustomSentence(225,180,2, "Tools");
  buildText.Run(WHITE);
  preheatText.Run(WHITE);
  toolsText.Run(WHITE);

  bool runMainMenu = true;
  unsigned long prevMillis = millis();
  while(runMainMenu) {
    if (millis() - prevMillis >= 3000) {
      prevMillis = millis();
      UpdateTempGauges();
    }
    
    if (touchInBox()) { // Build

    }
  }
}

void loop() { }








