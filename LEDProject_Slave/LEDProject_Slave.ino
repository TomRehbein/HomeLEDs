#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "reactive_common.h"

#define LED_PIN 2
#define NUM_LEDS 144 // chnage to real num

#define MIC_LOW 0    // not using a mic
#define MIC_HIGH 644 // not using a mic

#define SAMPLE_SIZE 20 // for what?
#define LONG_TERM_SAMPLES 250
#define BUFFER_DEVIATION 400
#define BUFFER_SIZE 3

#define LAMP_ID 1 // ++ for the next lamp
WiFiUDP UDP;

const char *ssid = "TTTArduinoWiFi";       // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "KmwcjaDYuKPKEVrN"; // The password of the Wi-Fi network

CRGB leds[NUM_LEDS];

struct averageCounter *samples;
struct averageCounter *longTermSamples;
struct averageCounter *sanityBuffer;

float globalHue; // for what?
float globalBrightness = 255;
int hueOffset = 120;
float fadeScale = 1.3;
float hueIncrement = 0.7;

struct led_command
{
  uint8_t opmode;
  uint32_t data;
};

unsigned long lastReceived = 0; // for what?
unsigned long lastHeartBeatSent;
const int heartBeatInterval = 100;
bool fade = false;

struct led_command cmd;
void connectToWifi();

// ------- setup start -------

void setup()
{
  globalHue = 0;
  samples = new averageCounter(SAMPLE_SIZE);
  longTermSamples = new averageCounter(LONG_TERM_SAMPLES);
  sanityBuffer = new averageCounter(BUFFER_SIZE);

  while (sanityBuffer->setSample(250) == true)
  {
  }

  while (longTermSamples->setSample(200) == true)
  {
  }

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  WiFi.begin(ssid, password); // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  connectToWifi();
  sendHeartBeat();
  UDP.begin(7001);
}

void connectToWifi()
{
  WiFi.mode(WIFI_STA);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(0, 0, 0);
  }

  leds[0] = CRGB(0, 255, 0);
  FastLED.show();

  int i = 0;

  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 (slave) to the master

  leds[0] = CRGB(0, 0, 255);
  FastLED.show();
  lastReceived = millis();
}

void sendHeartBeat()
{
  struct heartbeat_message hbm;
  hbm.client_id = LAMP_ID;
  hbm.chk = 77777;

  Serial.println("Sending heartbeat");

  IPAddress ip(192, 168, 4, 1); // ?? maybe need to be changed
  UDP.beginPacket(ip, 7171);

  int ret = UDP.write((char *)&hbm, sizeof(hbm));

  printf("Returned: %d, also sizeof hbm: %d \n", ret, sizeof(hbm));

  UDP.endPacket();
  lastHeartBeatSent = millis();
}

// ------- loop start -------

void loop()
{
  if (millis() - lastHeartBeatSent > heartBeatInterval)
  {
    sendHeartBeat();
  }

  int packetSize = UDP.parsePacket();
  if (packetSize)
  {
    UDP.read((char *)&cmd, sizeof(struct led_command));
    lastReceived = millis();
  }

  if (millis() - lastReceived >= 5000)
  {
    connectToWifi();
  }

  int opMode = cmd.opmode;
  int analogRaw = cmd.data;

  switch (opMode) // rewrite
  {
  case 1:
    fade = false;
    soundReactive(analogRaw);
    break;

  case 2:
    fade = false;
    allWhite();
    break;

  case 3:
    chillFade();
    break;
  }
}

void allWhite()
{
  foreach (var led in leds)
  {
    led = CRGB(255, 255, 235);
  }
  FastLED.show();
}
