/*
   Bridge_Receiver_MCP.ino
   Receives ESP-NOW messages and pulses MCP23008 output pins on the bridge.
*/

#include <Wire.h>
#include <Adafruit_MCP23008.h>
#include <ESP8266WiFi.h>
extern "C" {
#include <espnow.h>
}

#include <structure.h>  // Shared struct definition
messages inMessage;
Adafruit_MCP23008 mcp;

// --- Debounce ---
volatile bool dataReceived = false;
volatile uint8_t lastButton = 0;
volatile unsigned long lastReceivedTime = 0;
const unsigned long debounceInterval = 1000; // ms

const int pulseTime = 50;  // ms

// ==================== ESP-NOW receive callback ====================
void onReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  (void)mac;  // MAC address not needed in this context
  if (len != sizeof(inMessage)) return;

  messages temp;
  memcpy(&temp, incomingData, sizeof(temp));

  unsigned long now = millis();
  if (temp.buttonNumber == lastButton && (now - lastReceivedTime < debounceInterval)) {
    return;
  }

  lastButton = temp.buttonNumber;
  lastReceivedTime = now;

  memcpy(&inMessage, &temp, sizeof(inMessage));
  dataReceived = true;
}



// ==================== Setup ====================
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- ESP-NOW Bridge Receiver (MCP) ---");

  Wire.begin(D2, D1);  // SDA, SCL
  mcp.begin();         // Default address 0x20

  //All outputs are active low
  for (uint8_t i = 0; i < 8; i++) {
    mcp.pinMode(i, OUTPUT);
    mcp.digitalWrite(i, LOW);
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_promiscuous_enable(true);
  wifi_set_channel(1);
  wifi_promiscuous_enable(false);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(onReceive);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
}



// ==================== Loop ====================
void loop() {
  if (dataReceived) {
    dataReceived = false;

    Serial.print("Received buttonNumber = ");
    Serial.println(inMessage.buttonNumber);

    if (inMessage.buttonNumber >= 1 && inMessage.buttonNumber <= 8) {
      uint8_t mcpPin = inMessage.buttonNumber - 1;
      Serial.printf("Pulsing MCP pin GP%d\n", mcpPin);
      mcp.digitalWrite(mcpPin, HIGH);
      delay(pulseTime);
      mcp.digitalWrite(mcpPin, LOW);
    } else {
      Serial.println("Invalid button number");
    }
  }
}
