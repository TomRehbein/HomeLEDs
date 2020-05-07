#include <FastLED.h>

#define NUM_LEDS 300
CRGB leds[NUM_LEDS]; // maybe need a change

#define DATA_PIN 3

int R = 0;
int G = 0;
int B = 0;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS)

  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0); // need a change
  }
  FastLED.show();
}

void loop() {
  
}

// ---------- Modis ----------

void mono(int R, int G, int B, int W)
{
  for(int i = 0; i < NUM_LEDS;i++)
  {
    leds[i] = CRGB(R, G, B) // need a change
  }
}

void musik()
{
  
}

void chill() 
{
  
}

void romantic()
{
  
}

void kochen()
{
  
}

// ---------- extra funktions ----------

int getRed()
{
  
}

int getGreen()
{
  
}

int getBlue()
{
  
}
