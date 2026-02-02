/*
   bridge_sender_MCP.ino
   Uses MCP23008 for 8 input pins. Sends UDP messages when inputs go HIGH.
*/

#define SKETCH_NAME "Bridge_Sender_MCP.ino"
#include <Wire.h>
#include <Adafruit_MCP23008.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// ---------- CONFIGURATION ----------
const char* ssid       = "iot";
const char* password   = "806194edb8";
const char* udpAddress = "192.168.1.111";  // Ubuntu PC IP
const unsigned int udpPort = 5005;

Adafruit_MCP23008 mcp;
WiFiUDP udp;

// ---------- INPUT–MESSAGE MAPPING ----------
struct InputMap {
  uint8_t mcpPin;         // MCP23008 pin (0–7)
  const char* message;
  bool lastState;
};

//MCP pins. A pulse on pin 0-7 sends a UDP message like "happy", "main", etc.
InputMap inputs[] = {
  { 0, "happy", LOW },
  { 1, "main",  LOW },
  { 2, "waves", LOW },
  { 3, "green", LOW },
  { 4, "white", LOW },
  { 5, "hint1", LOW },
  { 6, "hint2", LOW },
  { 7, "hint3", LOW }
};
const int numInputs = sizeof(inputs) / sizeof(inputs[0]);




// ==================== setup() ====================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Sketch Name: " SKETCH_NAME);

  // --- Initialize MCP23008 ---
  Wire.begin(D2, D1);  // SDA, SCL
  mcp.begin();         // Default address 0x20

  for (uint8_t i = 0; i < 8; i++) {
    mcp.pinMode(i, INPUT);    //Inputs are floating
  }

  // --- Connect to WiFi ---
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // --- Initialize UDP ---
  udp.begin(udpPort);
  Serial.printf("UDP ready to send to %s:%d\n", udpAddress, udpPort);
}



// ==================== loop() ====================
void loop() {
  for (int i = 0; i < numInputs; i++) {
    bool currentState = mcp.digitalRead(inputs[i].mcpPin);

    // Detect rising edge (LOW to HIGH)
    if (currentState == HIGH && inputs[i].lastState == LOW) {
      Serial.printf("Trigger on MCP pin %d (video %d → Sending UDP: %s\n",
                    inputs[i].mcpPin, inputs[i].mcpPin + 1, inputs[i].message);

      udp.beginPacket(udpAddress, udpPort);
      udp.write(inputs[i].message);
      udp.endPacket();
    }

    inputs[i].lastState = currentState;
  }

  delay(20);  // Debounce
}
