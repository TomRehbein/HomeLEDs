#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "reactive_common.h"

#define READ_PIN 0
#define BUTTON_PIN 1

#define NUMBER_OF_CLIENTS 1 // change to real num
bool heartbeats[NUMBER_OF_CLIENTS];
const int numOpModes = 3;
static int opMode = 1;

const int checkDelay = 5000;
const int buttonDoubleTapDelay = 200; // don´t no if needed

unsigned long lastChecked; // hole block - don´t no if needed
unsigned long buttonChecked;
bool buttonClicked = false;
bool queueDouble = false;
bool clickTrigger;
bool doubleTapped;

WiFiUDP UDP;

struct led_command
{
  uint8_t opmode;
  uint32_t data;
};

// ------- setup start -------

void setup()
{
  pinMode(READ_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  // --- WiFi Part ---
  Serial.begin(115200);
  Serial.println();
  Serial.print("Setting soft-AP ... ");
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("TTTArduinoWiFi", "KmwcjaDYuKPKEVrN");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  UDP.begin(7171);
  resetHeartBeats(); // check from here on
  waitForConnections();
  lastChecked = millis();
  buttonChecked = 0;
}

void resetHeartBeats()
{
  for (int i = 0; i < NUMBER_OF_CLIENTS; i++)
  {
    heartbeats[i] = false;
  }
}

void waitForConnections()
{
  while (true)
  {
    readHeartBeat();
    if (checkHeartBeats())
    {
      return;
    }
    delay(checkDelay);
    resetHeartBeats();
  }
}

void readHeartBeat() // maybe rewrite
{
  struct heartbeat_message hbm;
  while (true)
  {
    int packetSize = UDP.parsePacket();
    if (!packetSize)
    {
      break;
    }
    UDP.read((char *)&hbm, sizeof(struct heartbeat_message));
    if (hbm.client_id > NUMBER_OF_CLIENTS)
    {
      Serial.println("Error: invalid client_id received");
      continue;
    }
    heartbeats[hbm.client_id - 1] = true;
  }
}

bool checkHeartBeats()
{
  for (int i = 0; i < NUMBER_OF_CLIENTS; i++)
  {
    if (!heartbeats[i])
    {
      return false;
    }
  }
  resetHeartBeats();
  return true;
}

// ------- loop start -------

void loop()
{
  buttonCheck();

  if (millis() - lastChecked > checkDelay) // is this needed?
  {
    if (!checkHeartBeats())
    {
      waitForConnections();
    }
    lastChecked = millis();
  }

  uint32_t Data = 0;

  if (opMode == 2)
  {
    Data = analogRead(READ_PIN);
  }

  sendLedData(opMode, Data);

  delay(4);
}

void buttonCheck() // maybe rewrite
{
  int but = digitalRead(BUTTON_PIN);
  if (but == 0)
  {
    if (millis() - buttonChecked < buttonDoubleTapDelay && buttonClicked == false)
    {
      doubleClicked();
      doubleTapped = true;
    }
    clickTrigger = true;
    buttonClicked = true;
    buttonChecked = millis();
  }

  else if (but == 1)
  {
    if (millis() - buttonChecked > buttonDoubleTapDelay && clickTrigger)
    {
      if (!doubleTapped)
      {
        clicked();
      }
      clickTrigger = false;
      doubleTapped = false;
    }
    buttonClicked = false;
  }
}

void clicked() // need rewrite
{
  if (opMode == numOpModes)
    opMode = 1;
  else
    opMode++;
  Serial.printf("Setting opmode %d \n", opMode);
}

void sendLedData(uint8_t op_mode, uint32_t data) // however ...
{
  struct led_command send_data;
  send_data.opmode = op_mode;
  send_data.data = data;
  for (int i = 0; i < NUMBER_OF_CLIENTS; i++)
  {
    IPAddress ip(192, 168, 4, 2 + i);
    UDP.beginPacket(ip, 7001);
    UDP.write((char *)&send_data, sizeof(struct led_command));
    UDP.endPacket();
  }
}