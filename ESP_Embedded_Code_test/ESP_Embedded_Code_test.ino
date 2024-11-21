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
#include <Wire.h>  // I2C
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// # Defines
#define LED 2              // For LED Heartbeat
#define PORT 12345         // server port
#define IMU_I2C_ADDR 0x29  // IMU I2C addr
#define IMU_DEV_ID 55
#define TFLUNA_I2C_ADDR 0x10
#define FRAME_LENGTH 33  // (msec) 30 Hz

// Sonar Pins
#define TRIG1 32  // Trigger pin for sensor 1
#define ECHO1 36  // Echo pin for sensor 1
#define TRIG2 33  // Trigger pin for sensor 2
#define ECHO2 39  // Echo pin for sensor 2
#define TRIG3 25  // Trigger pin for sensor 3
#define ECHO3 34  // Echo pin for sensor 3
#define TRIG4 26  // Trigger pin for sensor 4
#define ECHO4 35  // Echo pin for sensor 4

// Boolean flags
bool useWiFi = false;     // Set to use WiFi
bool hostNetwork = true;  // Set to host network
bool useCV = true;        // Set to use computer vision
bool useSonar = true;     // Set to use sonar functions
bool useLiDAR = false;     // Set to use LiDAR functions
bool useImu = false;       // Set to use IMU
bool plotRange = false;
bool plotIMU = false;

// WiFi global variables
const char* ssid = "";      // Wifi network name
const char* password = "";  // Wifi network password

// Network hosting global variables
WiFiUDP udp;  // UDP object
WebServer server(80);

// Global Variables for Sensor
float c = 0.034326;  // speed of sound (cm/microsecond)

// IMU Global Var
Adafruit_BNO055 bno = Adafruit_BNO055(IMU_DEV_ID, IMU_I2C_ADDR, &Wire);
sensors_event_t event;  // Event object

// LiDAR Global Var
uint16_t lidarDistance = -1;

// Setup Code
void setup() {
  // Set up serial
  Serial.begin(115200);  // Init Serial

  // Set up I2C
  Wire.begin();

  // Setup heartbeat
  pinMode(LED, OUTPUT);     // Set up LED as output
  digitalWrite(LED, HIGH);  // Init to high

  // Setup WiFi if enabled
  if (useWiFi) {
    const char* ssid = "SpectrumSetup-FE65";  // Wifi network name
    const char* password = "tannerboys05";    // Wifi network password

    WiFi.mode(WIFI_STA);         // WiFi mode
    WiFi.begin(ssid, password);  // Connect to the network
    while (WiFi.status() != WL_CONNECTED)  // Loop till connected
    {
      delay(500);  // 500 ms delay
    }
  }

  // Setup if ESP32 is hosting a network
  if (hostNetwork) {
    const char* ssid = "FORWARD_Network";   // Network name
    const char* password = "Forward?0525";  // Network pass

    // Initialize the access point
    WiFi.softAP(ssid, password);

    // Start the server
    server.begin();

    // UDP server setup
    udp.begin(PORT);
  }

  // Setup Sonar sensors if connected
  if (useSonar) {
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
  if (useImu) {
    if (!bno.begin()) {
      while (1)
        ;
    }

    bno.setExtCrystalUse(true);
    delay(100);  // Allow the sensor to initialize

  }
}

// Main loop
void loop() {

  // Sonar data readings
  if (useSonar) {
    // Obtain distance
    int durS1 = readSonarDuration(TRIG1, ECHO1);
    int distS1 = durS1 * (c / 2);  // one way pulse
    delay(3);              // avoid interference

    long durS2 = readSonarDuration(TRIG2, ECHO2);
    long distS2 = durS2 * (c / 2.0);
    delay(3);

    int durS3 = readSonarDuration(TRIG3, ECHO3);
    int distS3 = durS3 * (c / 2);
    delay(3);

    int durS4 = readSonarDuration(TRIG4, ECHO4);
    int distS4 = durS4 * (c / 2);
    delay(3);

    // Print results to the serial monitor
    if (plotRange) {
      Serial.print("S1:");
      Serial.print(distS1);
      Serial.print(",");

      Serial.print("S2:");
      Serial.print(distS2);
      Serial.print(",");

      Serial.print("S3:");
      Serial.print(distS3);
      Serial.print(",");

      Serial.print("S4:");
      Serial.print(distS4);
      Serial.print(",");
    }
  }

  // IMU data readings
  if (useImu) {
    bno.getEvent(&event);         // Get Euler angles
    float yaw = event.orientation.x;    // Extract yaw
    float pitch = event.orientation.y;  // Extract pitch
    float roll = event.orientation.z;   // Extract roll

    // Print Data readings
    if (plotIMU) {
      Serial.print("Yaw:");
      Serial.print(yaw);
      Serial.print(",");  // deg cw+
      Serial.print("Pitch:");
      Serial.print(pitch);
      Serial.print(",");  // deg ccw+
      Serial.print("Roll:");
      Serial.print(roll);
      Serial.print(",");  // deg ccw+
      //Serial.println();
    }

    // get the linear accleration AFTER, because x,y,z need to be refreshed
    imu::Vector<3> linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

    if (plotIMU) {
      Serial.print("AccelX:");
      Serial.print(linearAccel.x());
      Serial.print(",");  // deg cw+
      Serial.print("AccelY:");
      Serial.print(linearAccel.y());
      Serial.print(",");  // deg cw+
      Serial.print("AccelZ:");
      Serial.print(linearAccel.z());
      Serial.print(",");  // deg cw+
      //Serial.println();
    }
  }

  // Read from LiDAR
  if (useLiDAR) {
    lidarDistance = readTFLunaDistance();  // Function to get distance

    // Data Validity check
    if (lidarDistance == 0xFFFF) {
    } else {
      if (plotRange) {
        Serial.print("LiDAR:");
        Serial.print(lidarDistance);  // cm
        Serial.print(',');
      }
    }
    Serial.println();
  }

  // Currently pulling incoming data of wifi (Object Detection)
  if (hostNetwork && useCV) {
    // Assert bit to signal recieve a packet
    udp.beginPacket("192.168.4.2", 12345);  // IP of camera board
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
      //Serial.printf("%s", incomingPacket);
    }
  }

  delay(FRAME_LENGTH);                      // Delay
  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
}

/////////////// Functions ///////////////

// Read Sonar duration function with trigger and echo pins at inputs
long readSonarDuration(int trigPin, int echoPin) {
  // Send out pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Recieve return data
  long duration = pulseIn(echoPin, HIGH);

  // Return duration in microseconds (\mu s)
  return duration;
}

// Simplified function to read LiDAR distance over I2C
uint16_t readTFLunaDistance() {
  Wire.beginTransmission(TFLUNA_I2C_ADDR);  // Open data line
  Wire.write(0x00);                         // Request distance data
  Wire.endTransmission();                   // Close data line

  // Request 2 bytes (distance low and high bytes)
  if (Wire.requestFrom(TFLUNA_I2C_ADDR, (uint8_t)2) == 2) {
    uint16_t distance = Wire.read();  // Low byte of distance
    distance |= Wire.read() << 8;     // High byte of distance
    return distance;                  // Return Distance
  } else {
    return 0xFFFF;  // Error code for insufficient bytes received
  }
}
