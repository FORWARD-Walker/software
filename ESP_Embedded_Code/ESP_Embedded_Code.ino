// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code servers to drive all process on the ESP32 MCU for our project
// Features include: WiFi connectivity, data transmission over wifi, Locally 
// hosted network, sensor data processing, motor controls, basic coding functionality
// Located remotely in https://github.com/FORWARD-Walker/software

// # Include
#include "WiFi.h"

// # Defines
#define LED 2 // For LED Heartbeat
#define PORT 80 // server port

// Wifi setup
bool useWiFi = false; // Set to use WiFi
const char* ssid = "SpectrumSetup-FE65";
const char* password = "tannerboys05";

// Set up Web-based serial monitor
bool webSerial = false; // Set to use web serial at IP
WiFiServer server(PORT);


// TODO: Use to host network with ESP32
bool hostNetwork = false; // Set to host network
const char nom[10] = "ESP32"; // Device name
uint8_t broadcastAddress[2][6] = {0x40, 0xf4, 0xc9, 0x12, 0xc7, 0xc7}; // AMB82 MAC

// Define sensor pins

// Variable for distance

// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(115200); // Init Serial
  Serial.println("\nSerial Initialized");  // Print confirmation
  
  // Setup heartbeat
  pinMode(LED, OUTPUT); // Set up LED as output
  digitalWrite(LED, HIGH); // Init to high
  Serial.println("\nHeartbeat Initialized"); // Print confirmation

  // Setup WiFi if enabled
  if(useWiFi)
  {
    WiFi.mode(WIFI_STA); // WiFi mode
    WiFi.begin(ssid, password); // Connect to the network
    Serial.print("\nConnecting to WiFi Network ...");
    while(WiFi.status() != WL_CONNECTED) // Loop till connected
    {
      Serial.print(".");
      delay(500); // 500 ms delay
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP()); // Print out IP

    Serial.print("Local ESP32 MAC: ");
    Serial.println(WiFi.macAddress()); // Print out MAC
  }

  // Setup wireless data transmission
  if(webSerial)
  {
    WiFi.softAP(ssid, password); // Connect to soft access point
    server.begin(); // Start server
    Serial.print("Access Point IP address: "); // Print out access point
    Serial.println(WiFi.softAPIP());

    // For use: https://forum.arduino.cc/t/data-transfer-between-esp32-devices-via-wifi/1240769/9

  }

  // Setup if ESP32 is hosting a network
  if(hostNetwork)
  {
    // TODO: setup host network code
    // https://learn.sparkfun.com/tutorials/sending-sensor-data-over-wifi/all
    // https://www.aranacorp.com/en/creating-an-esp32-network-with-esp-now/
  }

}

// Main loop
void loop()
{
  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat

  delay(500); // 500 msec delay

}

/////////////// Functions ///////////////
