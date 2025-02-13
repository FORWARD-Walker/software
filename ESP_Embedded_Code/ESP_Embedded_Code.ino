// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code serves to drive all process on the ESP32 MCU for our project
// Features include: WiFi connectivity, data transmission over wifi, Locally 
// hosted network, sensor data processing, motor controls, basic coding functionality
// Located remotely in https://github.com/FORWARD-Walker/software

// # Include
#include <Wire.h> // I2C
#include "Sonar.h"
#include "Lidar.h"
#include "Imu.h"
#include "Networking.h"
#include "Haptic.h"

// # Defines
#define LED 2 // For LED Heartbeat
#define FRAME_LENGTH 33 // (msec) [1000 = 1 Hz, 500 = 2 Hz, 250 = 4 Hz, 100 = 10 Hz, 33 = 30 Hz]

// Sonar Pins
#define TRIG1 32 // Trigger pin for sensor 1
#define ECHO1 36 // Echo pin for sensor 1
#define TRIG2 33 // Trigger pin for sensor 2
#define ECHO2 39 // Echo pin for sensor 2
#define TRIG3 25 // Trigger pin for sensor 3
#define ECHO3 34 // Echo pin for sensor 3
#define TRIG4 26 // Trigger pin for sensor 4
#define ECHO4 35 // Echo pin for sensor 4

// Haptic pins
#define LMP1 19 // Haptic motor 1 pin 2
#define LMP2 18 // Haptic motor 1 pin 2
#define LME 14  // Haptic motor 1 enable pin
#define RMP1 17 // Haptic motor 2 pin 1
#define RMP2 16 // Haptic motor 2 pin 2
#define RME 12  // Haptic motor 2 enable pin

// Boolean flags
bool hostNetwork = true; // Set to host network
bool useCV = true; // Set to use computer vision
bool useSonar = true; // Set to use sonar functions
bool useLiDAR = true; // Set to use LiDAR functions
bool useImu = true; // Set to use IMU
bool useHaptics = true; // Set to use Haptics

// Network Object
Networking* pNetworking;

// Utils
utils* utils;

// Sensor Objects
Sonar* pS1;
Sonar* pS2;
Sonar* pS3;
Sonar* pS4;

// IMU Object
Imu* pIMU;

// LiDAR Object
Lidar* pLidar;

// Haptic Objects
Haptic* pHapticL;
Haptic* pHapticR;

// Control flags
boolean s1FG = false;
boolean s2FG = false;
boolean s3FG = false;
boolean s4FG = false;
boolean buzzLeft = false;
boolean buzzRight = false;

// Function Prototype
void Update_Data();

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

  // Setup if ESP32 is hosting a network
  if(hostNetwork)
  {
    pNetworking = new Networking();  // Init object
    Serial.println("Network Initialized!\n");
  }

  // Setup Sonar sensors if connected
  if(useSonar)
  {
    // Initizalize the Sonar objects
    pS1 = new Sonar(TRIG1, ECHO1);
    pS1->printPins();
    pS2 = new Sonar(TRIG2, ECHO2);
    pS2->printPins();
    pS3 = new Sonar(TRIG3, ECHO3);
    pS3->printPins();
    pS4 = new Sonar(TRIG4, ECHO4);
    pS4->printPins();
    Serial.println("Sonar Initialized!\n"); // Print confirmation
  }

  // Initizalize the LiDAR object
  if(useLiDAR)
  {
    pLidar = new Lidar();  // Init object
    Serial.println("LiDAR Initialized!\n");
  }

  // Intialize IMU object
  if(useImu)
  {
    pIMU = new Imu(); // Init object
    Serial.println("IMU Initialized!\n"); // Print confirmation
  }

  // Initialize Haptics Objects
  if(useHaptics)
  {
    pHapticL = new Haptic(LMP1, LMP2, LME);  // Init object
    pHapticL->printPins();
    pHapticR = new Haptic(RMP1, RMP2, RME);  // Init object
    pHapticR->printPins();
    Serial.println("Haptics Initialized!\n"); // Print confirmation
  }
}

// Main loop
void loop()
{
  Update_Data(); // Update Sensor data
  
  // Reset Flags
  s1FG = false;
  s2FG = false;
  s3FG = false;
  s4FG = false;

  // Null pointer check sonar sensors
  if(pS1 && pS2 && pS3 && pS4)
  {
    // Check if object is on the left
    if(pS1->distance < 100)
    {
      Serial.println("Object within 1 meter on the left!");
      Serial.print("S1: ");
      Serial.println(pS1->distance);
      s1FG = true;
    }

    // Check if object is on the front left
    if(pS2->distance < 300)
    {
      Serial.println("Object within 3 meters in front Left!");
      Serial.print("S2: ");
      Serial.println(pS2->distance);
      s2FG = true;
    }

    // Check if object is on the front right
    if (pS3->distance < 300)
    { 
      Serial.println("Object within 3 meters in front right!");
      Serial.print("S3: ");
      Serial.println(pS3->distance);
      s3FG = true;
    }

    // Check if an object is on the right
    if(pS4->distance < 100)
    {
      Serial.println("Object within 1 meter on the right!");
      Serial.print("S4: ");
      Serial.println(pS4->distance);
      s4FG = true;
    }

    // Classify Object
    if (s1FG || s2FG || s3FG || s4FG)
    {
      // Check Camera data
      int numObj = -1;
      if(pNetworking && useCV)
      {
        char data[512];
        pNetworking->getUDPPacket(data, sizeof(data));
        Serial.println("\nObject Data: ");
        Serial.println(data);

        // Parse data to store
        // Number of objects
        // Each object data
      }
    }
  }

  delay(FRAME_LENGTH); // Delay
  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
}


// Update Sensor Data
void Update_Data()
{
  // Update Sonar Distances
  if(pS1 && pS2 && pS3 && pS4)
  {
    pS1->readDistance();
    pS2->readDistance();
    pS3->readDistance();
    pS4->readDistance();
  }

  // Update LiDAR Distances
  if(pLidar)
  {
    pLidar->readDistance();
  }

  // Update IMU readings
  if(pIMU)
  {
    pIMU->updateData();
  }
}