#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

// Defines the ssid and password variables as const char*
#include "WiFiSettings.h"

unsigned long triggerInterval = 10000;
unsigned long lastTrigger = 0;
bool chimeIsTriggered = false;

void chimeTriggered();

void setup() {
  Serial.begin(9600);
  delay(100);

  // Connect WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.hostname("Smart Doorbell");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off

  pinMode(D8, INPUT_PULLDOWN_16);
  attachInterrupt(digitalPinToInterrupt(D8), chimeTriggered, RISING);
}

void loop() {
  if (chimeIsTriggered) {
    chimeIsTriggered = false;
    unsigned long triggerTime = millis();
    if (((unsigned long)triggerTime - lastTrigger) > triggerInterval) {
      lastTrigger = triggerTime;
      Serial.println("Chime was triggered! Sending HTTP request.");

      WiFiClient client;
      HTTPClient http;
      http.begin(client, "http://pi-server.local:15927");
      http.GET();
      http.end();

      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

ICACHE_RAM_ATTR void chimeTriggered() {
  chimeIsTriggered = true;
}
