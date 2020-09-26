#include <ILI9341_t3.h>
#include <font_Arial.h>
#include <font_ArialBold.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

#define LED_PIN 13
#define TFT_DC  9
#define TFT_CS 10
#define TOUCH_CS 8

typedef struct {
  int x;
  int y;
} points_t;

#define NUM_PTS 10000

points_t points[10000];
int write_idx = 0;
int read_idx = 0;
boolean record = true;
int width = 2;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
XPT2046_Touchscreen ts(TOUCH_CS);

boolean pointInRect(int x, int y, int rectX, int rectY, int rectW, int rectH) {
  boolean ret = false;
  if ((x >= rectX) && (x <= (rectX + rectW)) && (y >= rectY) && (y <= (rectY + rectH))) {
    ret = true;
  }
  return ret;
}

char text[4][10] = {
  "Rec ",
  "Clear",
  "Play",
  "Width "
};

void redrawButtons() {
  tft.setTextColor(ILI9341_WHITE);
  for (int i=0; i < 4; i++) {
    tft.fillRoundRect(i * 80, 224, 76, 14, 3, CL(0,0,0));
    tft.setCursor((i * 80) + 20, 226);
    tft.print(text[i]);
    if (i == 0) {
      if (record) {
        tft.print("Off");
      }
      else {
        tft.print("On");
      }
    }
    if (i == 3) {
      tft.print(width);
    }
  }
}

void clearDisplay() {
  tft.fillScreenVGradient(CL(255,255,255), CL(0,0,255));
  redrawButtons();
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Alive");
  tft.begin();
  tft.setRotation(3);
  ts.begin();
  ts.setRotation(3);
  clearDisplay();
}

void loop() {
  if (ts.touched()) {
    int X, Y;
    TS_Point p = ts.getPoint();
    X = p.x - 231;
    X = map(X, 3482, 0, 0, 320);
    Y = p.y - 360;
    Y = map(Y, 3465, 0, 0, 240);
    if (Y < 220) {
      tft.fillCircle(X, Y, width, CL(0,255,0));
      if (record) {
        points[write_idx].x = X;
        points[write_idx].y = Y;
        write_idx++;
        if (write_idx >= NUM_PTS) {
          write_idx = 0;
        }
        delay(4);
      }
    }
    if (pointInRect(X, Y, 0, 224, 76, 14)) {
      record = !record;
      if (!record) {
        Serial.println("========");
        Serial.println("typedef struct {");
        Serial.println("  int x;\n  int y;");
        Serial.println("} points_t;\n");
        Serial.println("const points_t points[] = {");
        for (int i = 0; i < write_idx; i++) {
          Serial.print("  { ");
          Serial.print(points[i].x);
          Serial.print(',');
          Serial.print(points[i].y);
          Serial.println(" },");
        }
        Serial.println("};\n");
        Serial.print("const int width = ");
        Serial.print(width);
        Serial.println(";\n========");
        write_idx = 0;
      }
      redrawButtons();
      delay(200);
    }
    if (pointInRect(X, Y, 80, 224, 76, 14)) {
      clearDisplay();
      delay(1000);
    }
    if (pointInRect(X, Y, 160, 224, 76, 14)) {
      read_idx = 0;
      while (read_idx < write_idx) {
        tft.fillCircle(points[read_idx].x, points[read_idx].y, width, CL(255,255,255));
        delay(1);
        read_idx++;
        if (ts.touched()) {
          break;
        }
      }
    }
    if (pointInRect(X, Y, 240, 224, 76, 14)) {
      width++;
      if (width > 9) {
        width = 1;
      }
      redrawButtons();
      delay(200);
    }
  }
}
