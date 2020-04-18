#include "FastLED.h"
#define NUM_LEDS 300
#define DATA_PIN 6

int i = 0;
    
CRGB leds[NUM_LEDS];

void setup() 
{
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS ; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  
  FastLED.show();
}

void loop() 
{
  unsigned long time = millis();

  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    leds[dot] = CRGB::White;  
    FastLED.show();
    //leds[dot] = CRGB::Green;
  } 
}
