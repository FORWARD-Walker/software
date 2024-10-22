// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code servers to drive all process on the ESP32 MCU for our project
// Features include: WiFi connectivity, data transmission over wifi, Locally 
// hosted network, sensor data processing, motor controls, basic coding functionality
// Located remotely in https://github.com/FORWARD-Walker/software

// # Include
#include "WiFi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

// # Defines
#define LED 2 // For LED Heartbeat
#define PORT 80 // server port 
#define TRIG1 19 // Trigger pin for sensor 1
#define ECHO1 18 // Echo pin for sensor 1
#define TRIG2 22 // Trigger pin for sensor 2
#define ECHO2 23 // Echo pin for sensor 2

// Boolean flags
bool useWiFi = false; // Set to use WiFi
bool hostNetwork = true; // Set to host network
bool sonar = false; // Set to use sonar functions

// WiFi global variables
const char* ssid = ""; // Wifi network name
const char* password = ""; // Wifi network password

// Network hosting global variables
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const char nom[10] = ""; // Device name
uint8_t broadcastAddress[2][6] = {-1}; // AMB82 MAC

// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(9600); // Init Serial
  Serial.println("\nSerial Initialized");  // Print confirmation
  
  // Setup heartbeat
  pinMode(LED, OUTPUT); // Set up LED as output
  digitalWrite(LED, HIGH); // Init to high
  Serial.println("\nHeartbeat Initialized"); // Print confirmation

  // Setup WiFi if enabled
  if(useWiFi)
  {
    const char* ssid = "SpectrumSetup-FE65"; // Wifi network name
    const char* password = "tannerboys05"; // Wifi network password

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

  // Setup if ESP32 is hosting a network
  if(hostNetwork)
  {
    const char* ssid = "ESP_Acess_Point_Network"; // Network name
    const char* password = "ESP32?AP"; // Network pass

    // Initialize the access point
    WiFi.softAP(ssid, password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    // Handle WebSocket connections
    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    // Start the server
    server.begin();
  
    /*const char nom[10] = "ESP32"; // Device name
    uint8_t broadcastAddress[2][6] = {0x40, 0xf4, 0xc9, 0x12, 0xc7, 0xc7}; // AMB82 MAC

    // TODO: setup host network code
    // https://learn.sparkfun.com/tutorials/sending-sensor-data-over-wifi/all
    // https://www.aranacorp.com/en/creating-an-esp32-network-with-esp-now/*/
  }

  // Setup Sonar sensors if connected
  if(sonar)
  {
    // Set pin directions
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);
  }
}

// Main loop
void loop()
{
  // Data transmit reading
  if(hostNetwork)
  {
    ws.cleanupClients();
  }

  // Sensor data readings
  if(sonar)
  {
    
    long duration1, distance1 = -1; // Store duration and distance for sensor 1
    long duration2, distance2 = -1; // Store duration and distance for sensor 2
    
    // Measure distance from Sensor 1
    digitalWrite(TRIG1, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG1, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG1, LOW);

    duration1 = pulseIn(ECHO1, HIGH); 
    distance1 = duration1 * 0.034 / 2;  // Speed of sound = 343 m/s -> 0.034 cm/us

    // Measure distance from Sensor 2
    digitalWrite(TRIG2, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG2, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG2, LOW);

    duration2 = pulseIn(ECHO2, HIGH);
    distance2 = duration2 * 0.034 / 2;

    // Print results to the serial monitor
    Serial.print("S1: ");
    Serial.print(distance1);
    Serial.print(" cm");

    Serial.print("S2: ");
    Serial.print(distance2);
    Serial.print(" cm");

    Serial.println();
  }

  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
  delay(500);
}

/////////////// Functions ///////////////
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client connected");
    client->text("Hello from ESP32!");
  } else if (type == WS_EVT_DATA) {
    // Handle incoming data from clients
    Serial.println("Data received");
    client->text("Data received");
  }
}





