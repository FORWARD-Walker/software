// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code servers to drive all process on the ESP32 MCU for our project
// Features include: WiFi connectivity, data transmission over wifi, Locally 
// hosted network, sensor data processing, motor controls, basic coding functionality
// Located remotely in https://github.com/FORWARD-Walker/software

// # Include
#include "WiFi.h"
#include <WiFiUdp.h>
#include <WebServer.h>
#include <Wire.h> // I2C
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


// # Defines
#define LED 2 // For LED Heartbeat
#define PORT 12345 // server port 
#define IMU_I2C_ADDR 0x28 // IMU I2C addr
#define IMU_DEV_ID -1

// Sonar Pins
#define TRIG1 32 // Trigger pin for sensor 1
#define ECHO1 36 // Echo pin for sensor 1
#define TRIG2 33 // Trigger pin for sensor 2
#define ECHO2 39 // Echo pin for sensor 2
#define TRIG3 25 // Trigger pin for sensor 3
#define ECHO3 34 // Echo pin for sensor 3
#define TRIG4 26 // Trigger pin for sensor 4
#define ECHO4 35 // Echo pin for sensor 4

// Boolean flags
bool useWiFi = false; // Set to use WiFi
bool hostNetwork = false; // Set to host network
bool useSonar = true; // Set to use sonar functions
bool useLiDAR = false; // Set to use LiDAR functions
bool useImu = true; // Set to use IMU

// WiFi global variables
const char* ssid = ""; // Wifi network name
const char* password = ""; // Wifi network password

// Network hosting global variables
WiFiUDP udp; // UDP object
WebServer server(80);

// Global Variables for Sensor
long sonarDistance_1 = -1; // Store duration and distance for sensor 1
long sonarDistance_2 = -1; // Store duration and distance for sensor 2
long sonarDistance_3 = -1; // Store duration and distance for sensor 2
long sonarDistance_4 = -1; // Store duration and distance for sensor 2

// IMU Global Var
Adafruit_BNO055 bno = Adafruit_BNO055(IMU_DEV_ID, IMU_I2C_ADDR, &Wire);


// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(115200); // Init Serial
  Serial.println("\nSerial Initialized!\n");  // Print confirmation
  
  // Setup heartbeat
  pinMode(LED, OUTPUT); // Set up LED as output
  digitalWrite(LED, HIGH); // Init to high
  Serial.println("Heartbeat Initialized!\n"); // Print confirmation

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

    Serial.println("\nConnected to the WiFi network\n");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP()); // Print out IP

    Serial.print("Local ESP32 MAC: ");
    Serial.println(WiFi.macAddress()); // Print out MAC
    Serial.println();
  }

  // Setup if ESP32 is hosting a network
  if(hostNetwork)
  {
    const char* ssid = "FORWARD_Network"; // Network name
    const char* password = "Forward?0525"; // Network pass

    // Initialize the access point
    WiFi.softAP(ssid, password);

    // Serial out server init
    Serial.println("Server Initialized!");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println();

    // Start the server
    server.begin();

    // UDP server setup
    udp.begin(PORT);
    Serial.print("UDP Initialized!\n");
    Serial.printf("UDP server started at port %d\n", PORT);

  
    /*const char nom[10] = "ESP32"; // Device name
    uint8_t broadcastAddress[2][6] = {0x40, 0xf4, 0xc9, 0x12, 0xc7, 0xc7}; // AMB82 MAC

    // TODO: setup host network code
    // https://learn.sparkfun.com/tutorials/sending-sensor-data-over-wifi/all
    // https://www.aranacorp.com/en/creating-an-esp32-network-with-esp-now/*/
  }

  // Setup Sonar sensors if connected
  if(useSonar)
  {
    // Set pin directions
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);
    pinMode(TRIG3, OUTPUT);
    pinMode(ECHO3, INPUT);
    pinMode(TRIG4, OUTPUT);
    pinMode(ECHO4, INPUT);
  }

  // Intialize IMU object
  if(useImu)
  {
    while(!bno.begin())
    {
      Serial.println("Could not find a valid BNO sensor, check wiring!");
      delay(500);
    }
    bno.setExtCrystalUse(true);
    Serial.println("IMU Initialized!\n");

  }
}

// Main loop
void loop()
{
  // Currently pulling incoming data of wifi (Object Detection)
  if(hostNetwork)
  {
    char incomingPacket[512];  // Buffer for incoming packets

    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
        // Read the packet into the buffer
        int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);  // Leave space for null-terminator
        if (len > 0) {
            incomingPacket[len] = '\0';  // Null-terminate the string
        }

        // Print the incoming packet
        Serial.printf("%s\n", incomingPacket);
    }
  }

  // Sonar data readings
  if(useSonar)
  {   
    // Obtain distance 
    sonarDistance_1 = readSonarDistance(TRIG1, ECHO1);
    sonarDistance_2 = readSonarDistance(TRIG2, ECHO2);
    sonarDistance_3 = readSonarDistance(TRIG3, ECHO3);
    sonarDistance_4 = readSonarDistance(TRIG4, ECHO4);

    // Print results to the serial monitor
    Serial.print("S1: ");
    Serial.print(sonarDistance_1);
    Serial.print(" cm ");

    Serial.print("S2: ");
    Serial.print(sonarDistance_2);
    Serial.print(" cm ");

    Serial.print("S3: ");
    Serial.print(sonarDistance_3);
    Serial.print(" cm ");

    Serial.print("S4: ");
    Serial.print(sonarDistance_4);
    Serial.print(" cm ");

    Serial.println();
  }

  // IMU data readings
  if(useImu)
  {
    /* Get a new sensor event */ 
    sensors_event_t event; 
    bno.getEvent(&event);
    
    /* Display the floating point data */
    Serial.print("X: ");
    Serial.print(event.orientation.x, 4);
    Serial.print("\tY: ");
    Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: ");
    Serial.print(event.orientation.z, 4);
    Serial.println();

  }

    delay(500);
    digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
}

/////////////// Functions ///////////////
long readSonarDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
}

