// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code serves to drive all process on the ESP32 MCU for our project
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
#define IMU_I2C_ADDR 0x29 // IMU I2C addr
#define IMU_DEV_ID 55
#define TFLUNA_I2C_ADDR 0x10
#define FRAME_LENGTH 250 // (msec) 4 Hz

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
bool hostNetwork = true; // Set to host network
bool useCV = true; // Set to use computer vision
bool useSonar = false; // Set to use sonar functions
bool useLiDAR = false; // Set to use LiDAR functions
bool useImu = false; // Set to use IMU

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
sensors_event_t event; // Event object
float yaw = -1;
float pitch = -1;
float roll = -1;
float accx = -1;
float accy = -1;
float accz = -1;

// LiDAR Global Var
uint16_t lidarDistance = -1;

// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(115200); // Init Serial
  Serial.println("\nSerial Initialized!\n");  // Print confirmation

  // Set up I2C
  Wire.begin();
  Serial.println("I2C Initialized!\n"); // Print confirmation

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

    Serial.println("Sonar Initialized!\n"); // Print confirmation
  }

  // Intialize IMU object
  if(useImu)
  {
    if (!bno.begin()) {
      Serial.println("Failed to initialize BNO055! Check connections.");
      while (1);
    }
  
    bno.setExtCrystalUse(true);
    delay(100);  // Allow the sensor to initialize

    Serial.println("IMU Initialized!\n"); // Print confirmation

  }
}

// Main loop
void loop()
{
  Serial.println("\nNew Iteration:");

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
    bno.getEvent(&event); // Get current data
    yaw = event.orientation.x; // Extract yaw
    pitch = event.orientation.y; // Extract pitch
    roll = event.orientation.z; // Extract roll
    accx = event.acceleration.x; // x acel
    accy = event.acceleration.y; // y acel
    accz = event.acceleration.z;// z acel

    // Print Data readings
    Serial.print("Yaw: "); Serial.print(yaw); Serial.print(", "); // deg cw+
    Serial.print("Pitch: "); Serial.print(pitch); Serial.print(", ");// deg ccw+
    Serial.print("Roll: "); Serial.print(roll); Serial.print(", ");// deg ccw+
    Serial.println();
    Serial.print("X Accel: "); Serial.print(accx); Serial.print(", "); // deg cw+
    Serial.print("Y Accel: "); Serial.print(accy); Serial.print(", "); // deg cw+
    Serial.print("Z Accel: "); Serial.print(accz); Serial.print(", "); // deg cw+
    Serial.println();
  }

  // Read from LiDAR
  if(useLiDAR)
  {
    lidarDistance = readTFLunaDistance(); // Function to get distance

    // Data Validity check
    if (lidarDistance == 0xFFFF)
    {
      Serial.print("Error reading Lidar");
    }
    else
    {
      Serial.print("LiDAR Distance (cm): ");
      Serial.print(lidarDistance);// cm
    }
    Serial.println();
  }

  // Currently pulling incoming data of wifi (Object Detection)
  if(hostNetwork && useCV)
  {
    // Assert bit to signal recieve a packet
    udp.beginPacket("192.168.4.2", 12345); // IP of camera board
    uint8_t rxFG = 1;
    udp.write(rxFG);
    udp.endPacket();

    char incomingPacket[512];  // Buffer for incoming packets
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
        // Read the packet into the buffer
        int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);  // Leave space for null-terminator
        if (len > 0) {
            incomingPacket[len] = '\0';  // Null-terminate the string
        }

        // Print the incoming packet
        Serial.printf("%s", incomingPacket);
    }
  }
  
  delay(FRAME_LENGTH); // Delay
  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
}

/////////////// Functions ///////////////

// Read Sonar distance function with trigger and echo pins at inputs
long readSonarDistance(int trigPin, int echoPin)
{
  // Send out pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Recieve return data
  long duration = pulseIn(echoPin, HIGH);

  // Return distance in cm
  return duration * 0.034 / 2;
}

// Simplified function to read LiDAR distance over I2C
uint16_t readTFLunaDistance()
{
  Wire.beginTransmission(TFLUNA_I2C_ADDR); // Open data line
  Wire.write(0x00);  // Request distance data
  Wire.endTransmission(); // Close data line

  // Request 2 bytes (distance low and high bytes)
  if (Wire.requestFrom(TFLUNA_I2C_ADDR, (uint8_t)2) == 2)
  {
    uint16_t distance = Wire.read();      // Low byte of distance
    distance |= Wire.read() << 8;         // High byte of distance
    return distance;                      // Return Distance
  } 
  else
  {
    return 0xFFFF;  // Error code for insufficient bytes received
  }
}
