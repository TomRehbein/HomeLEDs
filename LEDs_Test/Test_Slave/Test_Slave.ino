#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "reactive_common.h"

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

struct averageCounter *longTermSamples;
struct averageCounter *sanityBuffer;

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
   longTermSamples = new averageCounter(LONG_TERM_SAMPLES);
   sanityBuffer = new averageCounter(BUFFER_SIZE);

   while (sanityBuffer->setSample(250) == true)
   {
   }

   while (longTermSamples->setSample(200) == true)
   {
   }

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