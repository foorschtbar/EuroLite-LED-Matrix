#include <Arduino.h>

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

  if (color == 0)
  {
    writeLine(PIN_R1, PIN_G1, PIN_TXD, data, 0);
  }
  else if (color == 1)
  {
    writeLine(PIN_R1, PIN_G1, PIN_TXD, 0, data);
  }
  else
  {
    writeLine(PIN_R1, PIN_G1, PIN_TXD, data, data);
  }

  digitalWrite(PIN_EN, HIGH);
  // delayMicroseconds(100);
  triggerMultivibrator();

  line++;
  if (line >= 16)
  {
    line = 0;
  }
  // Serial.print("x");

  if (millis() - timer > 10)
  {
    timer = millis();

    if (~data == 0)
    {
      data = 0;
      color++;
    }
    if (color > 2)
    {
      color = 0;
    }

    data = data << 1;
    data = data + 1;
  }
}
