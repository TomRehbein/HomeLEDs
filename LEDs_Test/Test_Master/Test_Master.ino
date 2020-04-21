#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "reactive_common.h"

#define NUMBER_OF_CLIENTS 1
bool heartbeat;

const int checkDelay = 5000;

WiFiUDP UDP;

struct led_command
{
   uint8_t opmode;
   uint32_t data;
};

void setup()
{
   Serial.begin(115200);
   Serial.println();
   Serial.print("Setting soft-AP ... ");
   WiFi.persistent(false);
   WiFi.mode(WIFI_AP);
   WiFi.softAP("TestWiFi", "QwErT12345!");
   Serial.print("Soft-AP IP address = ");
   Serial.println(WiFi.softAPIP());
   UDP.begin(7171);
   resetHeartBeat(); // check from here on
   waitForConnections();
   lastChecked = millis();
   buttonChecked = 0;

   Serial.println("connected");
}

void resetHeartBeat()
{
   heartbeat = false
}

void waitForConnections()
{
   while (true)
   {
      readHeartBeat();

      if (checkHeartBeat())
      {
         return;
      }

      delay(checkDelay);
      resetHeartBeat();
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

      heartbeat = true;
   }
}

bool checkHeartBeat()
{
   if (!heartbeat)
   {
      return false;
   }

   resetHeartBeat();
   return true;
}

void loop()
{
   Serial.println("Test done!");
   delay(2000);
}