#include <Arduino.h>
#include <U8g2lib.h>

#define PIN_A 16   // Data A0 --> U1 --> U3/U4 --> U22/U23/U24/U25
#define PIN_B 17   // Data A1 --> U1 --> U3/U4 --> U22/U23/U24/U25
#define PIN_C 18   // Data A2 --> U1 --> U3/U4 --> U22/U23/U24/U25
#define PIN_D 19   // Data Select Upper/Lower Row --> [5] U1 [15] --> [5] U3 (^E2) + [6] U4 (E3)
#define PIN_LAT 21 // Latch --> [2, 3, 4] U1 [16, 17, 18] --> [12] U6/U7/U8...U21 (STCP - storage register clock input)
#define PIN_EN 22  // Enable --> [7, 8] U2 [12, 13] --> [4] U3/U4 (E1)
#define PIN_R1 23  // Red LEDs (DS - serial data input)
#define PIN_G1 25  // Green LEDs (DS - serial data input)
#define PIN_TXD 26 // Clock --> [2,3,4] U2 [16, 17, 18] --> [11] U6/U7/U8...U21  (SHCP - shift register clock input)
                   // U5 (Multivibrator) --> [13] U6/U7/U8...U21 (^OE - output enable input(active LOW))

U8G2_BITMAP u8g2(64, 16, U8G2_R0);

int line = 0;

void setup()
{
  // put your setup code here, to run once:
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_LAT, OUTPUT);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_R1, OUTPUT);
  pinMode(PIN_G1, OUTPUT);
  pinMode(PIN_TXD, OUTPUT);

  Serial.begin(115200);
  Serial.println("Setup Start");

  u8g2.begin();

  u8g2.drawFrame(0, 0, 64, 16);

  u8g2.drawFrame(2, 2, 4, 4);
  u8g2.drawFilledEllipse(4, 11, 2, 2, U8G2_DRAW_ALL);

  u8g2.setFont(u8g2_font_5x7_mf);
  // u8g2_SetFont(&u8g2, u8g2_font_helvB18_tr);
  u8g2.drawStr(9, 8, "A");
  u8g2.drawStr(17, 8, "B");
  u8g2.drawStr(25, 8, "C");
  u8g2.drawStr(33, 8, "D");
  u8g2.drawStr(9, 14, "1");
  u8g2.drawStr(17, 14, "2");
  u8g2.drawStr(25, 14, "3");
  u8g2.drawStr(33, 14, "4");

  Serial.println("Setup Done");
}

void serialPreview()
{

  // u8g2.writeBufferPBM(Serial);

  Serial.printf("W:%i (%i) x H:%i (%i)\n", u8g2.getBufferTileWidth(), u8g2.getBufferTileWidth() * 8, u8g2.getBufferTileHeight(), u8g2.getBufferTileHeight() * 8);

  uint16_t x, y;
  uint16_t w, h;

  w = u8g2.getBufferTileWidth();
  w *= 8;
  h = u8g2.getBufferTileHeight();
  h *= 8;

  for (y = 0; y < h; y++)
  {
    for (x = 0; x < w; x++)
    {
      if (u8x8_capture_get_pixel_1(x, y, u8g2.getBufferPtr(), u8g2.getBufferTileWidth()))
      {
        Serial.print("*");
      }
      else
      {
        Serial.print(" ");
      }
    }
    Serial.print("\n");
  }
}

void setLine(int n)
{
  digitalWrite(PIN_A, n & 0b001);
  digitalWrite(PIN_B, n & 0b010);
  digitalWrite(PIN_C, n & 0b100);
  digitalWrite(PIN_D, n & 0b1000);
}

void writeLine(int pin_r, int pin_g, int clock, uint64_t data_r, uint64_t data_g)
{
  for (size_t i = 0; i < 8; i++)
  {
    uint8_t panel_r = (~data_r) >> (8 * i);
    uint8_t panel_g = (~data_g) >> (8 * i);
    digitalWrite(pin_r, panel_r & 0b00000001);
    digitalWrite(pin_g, panel_g & 0b00000001);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b00000010);
    digitalWrite(pin_g, panel_g & 0b00000010);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b00000100);
    digitalWrite(pin_g, panel_g & 0b00000100);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b00001000);
    digitalWrite(pin_g, panel_g & 0b00001000);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b00010000);
    digitalWrite(pin_g, panel_g & 0b00010000);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b00100000);
    digitalWrite(pin_g, panel_g & 0b00100000);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b01000000);
    digitalWrite(pin_g, panel_g & 0b01000000);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);

    digitalWrite(pin_r, panel_r & 0b10000000);
    digitalWrite(pin_g, panel_g & 0b10000000);
    digitalWrite(clock, LOW);
    digitalWrite(clock, HIGH);
  }

  digitalWrite(PIN_LAT, LOW);
  digitalWrite(PIN_LAT, HIGH);
}

void triggerMultivibrator()
{
  digitalWrite(PIN_A, HIGH);
  digitalWrite(PIN_A, LOW);
}

uint64_t data = 0;
int color = 0;
long timer = 0;

void loop()
{

  digitalWrite(PIN_EN, LOW);
  setLine(line);

  digitalWrite(PIN_EN, HIGH);
  triggerMultivibrator();

  line++;
  if (line >= 16)
  {
    line = 0;
  }

  if (millis() - timer > 1000)
  {
    timer = millis();

    serialPreview();
  }

  // Brightness
  delayMicroseconds(100);
}
