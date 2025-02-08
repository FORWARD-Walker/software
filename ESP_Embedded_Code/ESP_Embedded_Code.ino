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
#include "Wheel.h"

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
#define LHMP1 19 // Left Haptic motor pin 1
#define LHMP2 18 // Left Haptic motor pin 2
#define LHME 27  // Left Haptic motor enable pin
#define RHMP1 17 // Right Haptic motor pin 1
#define RHMP2 16 // Right Haptic motor pin 2
#define RHME 14  // Right Haptic motor enable pin

// Wheel Pins
#define LWMPF 4 // Left Wheel motor pin 1
#define LWMPR 0 // Left Wheel motor pin 2
#define RWMPF 2 // Right Wheel motor pin 1
#define RWMPR 15 // Right Wheel motor pin 2

// Boolean flags
bool hostNetwork = true; // Set to host network
bool useCV = true; // Set to use computer vision
bool useSonar = true; // Set to use sonar functions
bool useLiDAR = true; // Set to use LiDAR functions
bool useImu = true; // Set to use IMU
bool useHaptics = true; // Set to use Haptics
bool useWheels = true; // Set to use Wheels

// Network Object
Networking* pNetworking = NULL;

// Sensor Objects
Sonar* pS1 = NULL;
Sonar* pS2 = NULL;
Sonar* pS3 = NULL;
Sonar* pS4 = NULL;

// IMU Object
Imu* pIMU = NULL;

// LiDAR Object
Lidar* pLidar = NULL;

// Haptic Objects
Haptic* pHapticL = NULL;
Haptic* pHapticR = NULL;

// Wheel Objects
Wheel* pWheelL = NULL;
Wheel* pWheelR = NULL;

// Function Prototypes
void Update_Data();
void Test_System();
void Send_Sensor_Data();

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
    pHapticL = new Haptic(LHMP1, LHMP2, LHME);  // Init object
    pHapticL->printPins();
    pHapticR = new Haptic(RHMP1, RHMP2, RHME);  // Init object
    pHapticR->printPins();
    Serial.println("Haptics Initialized!\n"); // Print confirmation
  }

  // Initialize Wheel Objects
  if(useWheels)
  {
    pWheelL = new Wheel(LWMPF, LWMPR);  // Init object
    pWheelL->printPins();
    pWheelR = new Wheel(RWMPF, RWMPR);  // Init object
    pWheelR->printPins();
    Serial.println("Wheels Initialized!\n"); // Print confirmation
  }
}

// Main loop
void loop()
{
  Update_Data(); // Update Sensor data

  // // Example avoidance of object in front
  // if(!pWheelL->spinning) pWheelL->startWheel(350, true);
  // if(!pWheelR->spinning) pWheelR->startWheel(350, true);

  // if (pS2->distance < 50 || pS3->distance < 50)
  // {
  //   pWheelL->startWheel(300, false);
  //   pWheelR->startWheel(300, false);
  //   pHapticL->startHaptic(3);
  //   pHapticR->startHaptic(3);
  //   delay(2000);
  //   pWheelL->stopWheel();
  //   pWheelR->stopWheel(); 
  //   delay(5000);
  //   pHapticL->stopHaptic();
  //   pHapticR->stopHaptic();
  // }
  
  Send_Sensor_Data();

  delay(FRAME_LENGTH); // Delay
  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
}

// Update Sensor Data
void Update_Data()
{
    // Update Sonar Distances
    if (pS1 && pS2 && pS3 && pS4)
    {
        pS1->readDistance();
        pS2->readDistance();
        pS3->readDistance();
        pS4->readDistance();
    }

    // Update LiDAR Distances
    if (pLidar)
    {
        pLidar->readDistance();
    }

    // Update IMU readings
    if (pIMU)
    {
        pIMU->updateData();
    }
}

// send Sensor data to website
void Send_Sensor_Data()
{
  String sensorData;

  sensorData = "S1: ";
  sensorData += pS1->distance;
  sensorData += " S2: ";
  sensorData += pS2->distance;
  sensorData += " S3: ";
  sensorData += pS3->distance;
  sensorData += " S4: ";
  sensorData += pS4->distance;
  sensorData += '\n';

  sensorData += "LiDAR: ";
  sensorData += pLidar->distance;
  sensorData += '\n';
  
  sensorData += "Roll: ";
  sensorData += pIMU->roll;
  sensorData += " Pitch: ";
  sensorData += pIMU->pitch;
  sensorData += " Yaw: ";
  sensorData += pIMU->yaw;
  sensorData += '\n';

  char data[512];
  pNetworking->getUDPPacket(data, sizeof(data));
  sensorData += data;

  pNetworking->pushSerialData(sensorData);
  pNetworking->update();
}

// Run full system test and upload data to http://192.168.4.1/
void Test_System()
{
  Send_Sensor_Data();

  Serial.println("Running Wheels!");
  pWheelL->startWheel(500, true);
  pWheelR->startWheel(500, true);
  Serial.println("Running Haptics!");
  pHapticL->startHaptic(3);
  pHapticR->startHaptic(3);
  delay(3000);
  pWheelL->stopWheel();
  pWheelR->stopWheel();
  pHapticL->stopHaptic();
  pHapticR->stopHaptic();

}

