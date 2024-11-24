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
#define FRAME_LENGTH 1000 // (msec) [1000 = 1 Hz, 500 = 2 Hz, 250 = 4 Hz, 100 = 10 Hz, 33 = 30 Hz]

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
bool hostNetwork = false; // Set to host network
bool useCV = false; // Set to use computer vision
bool useSonar = false; // Set to use sonar functions
bool useLiDAR = false; // Set to use LiDAR functions
bool useImu = false; // Set to use IMU
bool useHaptics = false; // Set to use Haptics

// Network Object
Networking* pNetworking;

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
    pS2 = new Sonar(TRIG2, ECHO2);
    pS3 = new Sonar(TRIG3, ECHO3);
    pS4 = new Sonar(TRIG4, ECHO4);
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
    pHapticR = new Haptic(RMP1, RMP2, RME);  // Init object
    Serial.println("Haptics Initialized!\n"); // Print confirmation
  }
}

// Main loop
void loop()
{
  // Sonar pointer check
  if(pS1 && pS2 && pS3 && pS4)
  {
    // If an object to be within a meter on the sides or 3 meters in front
    if((pS1 < 100) || (pS2 < 300) || (pS3 < 300) || (pS4 < 100))
    {
      // Check Camera data
      int numObj = -1;
      if(pNetworking && useCV)
      {
        char data[512];
        pNetworking->getUDPPacket(data, sizeof(data));

        // Parse data to store
        // Number of objects
        // Each object data
      }

      // Check for special cases
      if(numObj > 3)
      {
        // Check if all objects are car, person, etc.
      }

      // Check if we have identified object
      if(numObj > 0)
      {
        // Call GNC loop
      }
    }
  }

  delay(FRAME_LENGTH); // Delay
  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
}

