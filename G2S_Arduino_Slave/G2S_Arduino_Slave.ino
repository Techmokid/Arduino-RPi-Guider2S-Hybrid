// Modified the start of:
// C:\Users\aj200\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino\hardwareSerial.h
//
// Define or undefine "SERIAL_TX_BUFFER_SIZE" and "SERIAL_RX_BUFFER_SIZE" to fix any serial memory issues. It will take 50% of SRAM

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <LinkedList.h>

#define DEG_TO_RAD 0.017453292519943295 // approximately π/180

#define YP A2
#define XM A3
#define YM 8
#define XP 9

#define ERR_PIN   22
#define FAN_PIN   23
#define LIGHT_PIN 24

#define EXTRUDER_TEMP_SENSOR_PIN A15
#define BED_TEMP_SENSOR_PIN      A14
#define EXTRUDER_HEATER_PIN      27
#define BED_HEATER_PIN           28

int desiredExtruderTemp = 24;
int desiredBedTemp = 24;

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
int Point_X,Point_Y;
TSPoint p;

LinkedList<String> commands = LinkedList<String>();
String inputBuffer = "";
unsigned long prevMillis;
int currentTextSize = 1;

void setup() {
  Serial.begin(115200);
  Serial.flush();

  pinMode(ERR_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(EXTRUDER_TEMP_SENSOR_PIN, INPUT);
  pinMode(BED_TEMP_SENSOR_PIN, INPUT);
  pinMode(EXTRUDER_HEATER_PIN, OUTPUT);
  pinMode(BED_HEATER_PIN, OUTPUT);

  TFT_ID = tft.readID();
  tft.begin(TFT_ID);
  tft.setRotation(1); 
  tft.fillScreen(RGBTo565(0,0,0));
  tft.setFont(NULL);
  tft.setTextSize(1);

  // Display logo
  tft.fillScreen(RGBTo565(30, 30, 30));
  tft.fillTriangle(80, 120, 160, 200, 160, 40, RGBTo565(200,200,200)); // First triangle
  tft.fillTriangle(160, 40, 160, 200, 240, 120, RGBTo565(100,100,100)); // First triangle
  tft.fillRect(120, 100, 40, 40, RGBTo565(120,0,120));
  tft.fillRect(160, 100, 40, 40, RGBTo565(0,200,120));

  Serial.println("AWAKE:"+String(TFT_ID));
}

void readSerial() {
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        inputBuffer += inChar;

        // If we receive a newline, print the line and clear that part of the buffer
        if (inChar == '\n') {
            inputBuffer.trim();  // Remove any whitespace or new line characters at the end
            if (inputBuffer.length() > 0) {
                //Serial.println(inputBuffer);  // Echo the complete line back to Serial (for debugging)
                commands.add(inputBuffer);
            }
            inputBuffer = "";  // Clear the buffer
        }
    }
}

String element;
String GetNextElement(String& input, bool* lastElement) {
  if (input.length() == 0) {return "";}

  int idx = input.indexOf(':');
  *lastElement = (idx == -1);
  if (*lastElement) { return input; }

  element = input.substring(0, idx);  // Extract element before the colon.
  input = input.substring(idx + 1);          // Update input to remove the extracted part.
  return element;  // Return the extracted element.
}

bool StringToBool(String x) {
  x.toLowerCase();
  return (x == "true" || x == "1" || x == "on");
}

int countChar(const String& str, char c) {
    int count = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str.charAt(i) == c) {
            count++;
        }
    }
    return count;
}

String msg;
String command;
String order;
unsigned long previousTempSensorsMillis = 0;
bool prevRelease = true;
int loopsSinceLastTouch = 0;
void loop() {
  delay(10);

  handleTemperatures();
  if (millis() - previousTempSensorsMillis > 500) {
    previousTempSensorsMillis = millis();
    Serial.println("TEMPS:" + String(GetHeadTemp()) + ":" + String(GetBedTemp()));
  }

  if (!Serial.available()) {
    if (millis() - prevMillis < 5) {return;} // Don't flood the rpi serial monitor with unecessary touch prompts
    prevMillis = millis();

    p = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(XM, HIGH);

    if ((p.z <= 0) || (p.z > 1000)) {
      if (loopsSinceLastTouch > 3) {
        if (!prevRelease) {
          prevRelease = true;
          Serial.println("RELEASED:" + String(Point_X) + ":" + String(Point_Y) + ":" + String(p.z));
        }
      }

      loopsSinceLastTouch++;
      return;
    }
    
    if (X_Max < p.x) { X_Max = p.x; }
    if (Y_Max < p.y) { Y_Max = p.y; }
    if (X_Min > p.x) { X_Min = p.x; }
    if (Y_Min > p.y) { Y_Min = p.y; }
    Point_X = map(p.x, 960, 115, 0, 320);
    Point_Y = map(p.y, 920, 102, 0, 240);
    Serial.println("TOUCH:" + String(Point_X) + ":" + String(Point_Y) + ":" + String(p.z));
    loopsSinceLastTouch = 0;
    prevRelease = false;
    return;
  }

  // Serial is available, do what the rpi tells you to
  readSerial();

  command = "";
  order = "";
  while (commands.size() > 0) {
    command = commands.shift(); // Process and remove the first command
    //Serial.println(command);

    bool isLast;
    order = GetNextElement(command, &isLast);
    if (order == "FCIRCLE") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int radius = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.fillCircle(x, y, radius, RGBTo565(r, g, b));  // Draw the circle
      Serial.println("FILL OK");
    } else if (order == "FRECT") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int width = GetNextElement(command, &isLast).toInt();
      int height = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.fillRect(x, y, width, height, RGBTo565(r, g, b));
      Serial.println("FILL OK");
    } else if (order == "FSQUARE") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int size = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.fillRect(x, y, size, size, RGBTo565(r, g, b));
      Serial.println("FILL OK");
    } else if (order == "CIRCLE") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int radius = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.drawCircle(x, y, radius, RGBTo565(r, g, b));  // Draw the circle
      Serial.println("DRAW OK");
    } else if (order == "RECT") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int width = GetNextElement(command, &isLast).toInt();
      int height = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.drawRect(x, y, width, height, RGBTo565(r, g, b));
      Serial.println("DRAW OK");
    } else if (order == "SQUARE") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int size = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.drawRect(x, y, size, size, RGBTo565(r, g, b));
      Serial.println("DRAW OK");
    } else if (order == "TRIG") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x1 = GetNextElement(command, &isLast).toInt();
      int y1 = GetNextElement(command, &isLast).toInt();
      int x2 = GetNextElement(command, &isLast).toInt();
      int y2 = GetNextElement(command, &isLast).toInt();
      int x3 = GetNextElement(command, &isLast).toInt();
      int y3 = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.drawTriangle(x1,y1,x2,y2,x3,y3, RGBTo565(r, g, b));
      Serial.println("DRAW OK");
    } else if (order == "FTRIG") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x1 = GetNextElement(command, &isLast).toInt();
      int y1 = GetNextElement(command, &isLast).toInt();
      int x2 = GetNextElement(command, &isLast).toInt();
      int y2 = GetNextElement(command, &isLast).toInt();
      int x3 = GetNextElement(command, &isLast).toInt();
      int y3 = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.fillTriangle(x1,y1,x2,y2,x3,y3, RGBTo565(r, g, b));
      Serial.println("DRAW OK");
    } else if (order == "TEXT") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      int x = GetNextElement(command, &isLast).toInt();
      int y = GetNextElement(command, &isLast).toInt();
      int size = GetNextElement(command, &isLast).toInt();
      msg = GetNextElement(command, &isLast);
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      if (currentTextSize != size) { tft.setTextSize(size); currentTextSize = size; }
      tft.setCursor(x, y);
      tft.setTextColor(RGBTo565(r, g, b));
      tft.print(msg);
      Serial.println("TEXT OK");
    } else if (order == "LINE") {
      int x0 = GetNextElement(command, &isLast).toInt();
      int y0 = GetNextElement(command, &isLast).toInt();
      int x1 = GetNextElement(command, &isLast).toInt();
      int y1 = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      tft.drawLine(x0, y0, x1, y1, RGBTo565(r, g, b));
      Serial.println("LINE OK");
    } else if (order == "ROT") {
      // Rotated rectangle using 2 triangles
      int pivotX = GetNextElement(command, &isLast).toInt();
      int pivotY = GetNextElement(command, &isLast).toInt();
      int width = GetNextElement(command, &isLast).toInt();
      int height = GetNextElement(command, &isLast).toInt();
      int angleDeg = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();

      float angleRad = angleDeg * DEG_TO_RAD; // Convert angle to radians
      float sinAngle = sin(angleRad);
      float cosAngle = cos(angleRad);

      // Rectangle's half dimensions
      float hw = width / 2.0;
      float hh = height / 2.0;

      // Calculate vertices around the center (pivot)
      int x0 = pivotX + (-hw * cosAngle - -hh * sinAngle);
      int y0 = pivotY + (-hw * sinAngle + -hh * cosAngle);
      int x1 = pivotX + ( hw * cosAngle - -hh * sinAngle);
      int y1 = pivotY + ( hw * sinAngle + -hh * cosAngle);
      int x2 = pivotX + ( hw * cosAngle -  hh * sinAngle);
      int y2 = pivotY + ( hw * sinAngle +  hh * cosAngle);
      int x3 = pivotX + (-hw * cosAngle -  hh * sinAngle);
      int y3 = pivotY + (-hw * sinAngle +  hh * cosAngle);

      // Draw two triangles to form the rectangle
      tft.drawTriangle(x0, y0, x1, y1, x2, y2, RGBTo565(r, g, b)); // First triangle
      tft.drawTriangle(x0, y0, x2, y2, x3, y3, RGBTo565(r, g, b)); // Second triangle
      Serial.println("ROT OK");
    } else if (order == "FROT") {
      int pivotX = GetNextElement(command, &isLast).toInt();
      int pivotY = GetNextElement(command, &isLast).toInt();
      int width = GetNextElement(command, &isLast).toInt();
      int height = GetNextElement(command, &isLast).toInt();
      int angleDeg = GetNextElement(command, &isLast).toInt();
      int r = GetNextElement(command, &isLast).toInt();
      int g = GetNextElement(command, &isLast).toInt();
      int b = GetNextElement(command, &isLast).toInt();
      
      float angleRad = angleDeg * DEG_TO_RAD; // Convert angle to radians
      float sinAngle = sin(angleRad);
      float cosAngle = cos(angleRad);

      // Rectangle's half dimensions
      float hw = width / 2.0;
      float hh = height / 2.0;

      // Calculate vertices around the center (pivot)
      int x0 = pivotX + (-hw * cosAngle - -hh * sinAngle);
      int y0 = pivotY + (-hw * sinAngle + -hh * cosAngle);
      int x1 = pivotX + ( hw * cosAngle - -hh * sinAngle);
      int y1 = pivotY + ( hw * sinAngle + -hh * cosAngle);
      int x2 = pivotX + ( hw * cosAngle -  hh * sinAngle);
      int y2 = pivotY + ( hw * sinAngle +  hh * cosAngle);
      int x3 = pivotX + (-hw * cosAngle -  hh * sinAngle);
      int y3 = pivotY + (-hw * sinAngle +  hh * cosAngle);

      // Draw two triangles to form the rectangle
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, RGBTo565(r, g, b)); // First triangle
      tft.fillTriangle(x0, y0, x2, y2, x3, y3, RGBTo565(r, g, b)); // Second triangle
      Serial.println("FROT OK");
    } else if (order == "HEAD") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      desiredExtruderTemp = GetNextElement(command, &isLast).toInt();
      Serial.println("OK");
    } else if (order == "BED") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      desiredBedTemp = GetNextElement(command, &isLast).toInt();
      Serial.println("OK");
    } else if (order == "LIGHT") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      digitalWrite(LIGHT_PIN,StringToBool(GetNextElement(command, &isLast)));
      Serial.println("LIGHT OK");
    } else if (order == "FAN") {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      analogWrite(FAN_PIN,StringToBool(GetNextElement(command, &isLast)));
      Serial.println("FAN OK");
    } else if (order == "PINWRITE") {
      int pin = GetNextElement(command, &isLast).toInt();
      bool state = StringToBool(GetNextElement(command, &isLast));
      digitalWrite(pin,state);
      //Serial.println("WRITE DONE");
    } else if (order == "PINREAD") {
      int pin = GetNextElement(command, &isLast).toInt();
      bool pinState = digitalRead(pin);
      Serial.println("PINREAD:" + String(pin) + ":" + String(pinState));
    } else if (order == "PINAWRITE") {
      int pin = GetNextElement(command, &isLast).toInt();
      int state = StringToBool(GetNextElement(command, &isLast));
      analogWrite(pin,state);
      //Serial.println("WRITE DONE");
    } else if (order == "PINAREAD") {
      int pin = GetNextElement(command, &isLast).toInt();
      int pinState = analogRead(pin);
      Serial.println("PINAREAD:" + String(pin) + ":" + String(pinState));
    } else {
      //if (countChar(command, ':') != 5) {Serial.println("ERR");}
      digitalWrite(ERR_PIN,!digitalRead(ERR_PIN));
      //Serial.println("ERR");
    }
  }

  //commands.clear();
}

float tempSimMovementSpeed = 0.001;
float tempSimHead = 24;
float tempSimBed = 24;
//int GetHeadTemp() { return analogRead(EXTRUDER_TEMP_SENSOR_PIN); }
//int GetBedTemp() { return analogRead(BED_TEMP_SENSOR_PIN); }
int GetHeadTemp() {
  if (tempSimHead < desiredExtruderTemp) { tempSimHead += tempSimMovementSpeed; }
  if (tempSimHead > desiredExtruderTemp) { tempSimHead -= tempSimMovementSpeed; }
  return tempSimHead;
}
int GetBedTemp() {
  if (tempSimBed < desiredBedTemp) { tempSimBed += tempSimMovementSpeed; }
  if (tempSimBed > desiredBedTemp) { tempSimBed -= tempSimMovementSpeed; }
  return tempSimBed;
}

void handleTemperatures() {
  digitalWrite(EXTRUDER_HEATER_PIN, GetHeadTemp()<desiredExtruderTemp);
  digitalWrite(BED_HEATER_PIN, GetBedTemp()<desiredBedTemp);
}






