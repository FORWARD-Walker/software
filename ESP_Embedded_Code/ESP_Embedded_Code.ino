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
#define LED 2

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
bool hostNetwork = false; // Set to host network
bool useCV = false; // Set to use computer vision
bool useSonar = false; // Set to use sonar functions
bool useLidar = false; // Set to use LiDAR functions
bool useImu = false; // Set to use IMU
bool useHaptics = false; // Set to use Haptics
bool useWheels = false; // Set to use Wheels

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

// Timer Objects
hw_timer_t *Timer_1Hz = NULL;
hw_timer_t *Timer_10Hz = NULL;
hw_timer_t *Timer_30Hz = NULL;

// Helper Function Prototypes
void Update_Data();
void Test_System();
void Send_Sensor_Data();

// ISR prototypes
static void IRAM_ATTR Timer_1Hz_ISR();
static void IRAM_ATTR Timer_10Hz_ISR();
static void IRAM_ATTR Timer_30Hz_ISR();

// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(115200); // Init Serial

  // Set up I2C
  Wire.begin();

  // Setup heartbeat
  pinMode(LED, OUTPUT); // Set up LED as output
  digitalWrite(LED, HIGH); // Init to high

  // Setup Timers and Interrupts
  // Timer 1 Hz
  Timer_1Hz = timerBegin(1000000);
  timerAttachInterrupt(Timer_1Hz, &Timer_1Hz_ISR);
  timerAlarm(Timer_1Hz, 1000000, true, 0);

  // Timer 10 Hz
  Timer_10Hz = timerBegin(10000000);
  timerAttachInterrupt(Timer_10Hz, &Timer_10Hz_ISR);
  timerAlarm(Timer_10Hz, 1000000, true, 0);  
  
  // Timer 30 Hz
  Timer_30Hz = timerBegin(30000000);
  timerAttachInterrupt(Timer_30Hz, &Timer_30Hz_ISR);
  timerAlarm(Timer_30Hz, 1000000, true, 0);

  // Setup if ESP32 is hosting a network
  if(hostNetwork)
  {
    pNetworking = new Networking();  // Init object
    pNetworking->pushSerialData("Network Initialized!\n");
  }

  // Setup Sonar sensors if connected
  if(useSonar)
  {
    // Initizalize the Sonar objects
    pS1 = new Sonar(TRIG1, ECHO1);
    pNetworking->pushSerialData("S1: " + pS1->printPins());
    pS2 = new Sonar(TRIG2, ECHO2);
    pNetworking->pushSerialData("S2: " + pS2->printPins());
    pS3 = new Sonar(TRIG3, ECHO3);
    pNetworking->pushSerialData("S3: " + pS3->printPins());
    pS4 = new Sonar(TRIG4, ECHO4);
    pNetworking->pushSerialData("S4: " + pS4->printPins());
    pNetworking->pushSerialData("Sonar Initialized!\n"); // Print confirmation
  }

  // Initizalize the LiDAR object
  if(useLidar)
  {
    pLidar = new Lidar();  // Init object
    pNetworking->pushSerialData("LiDAR Initialized!\n");
  }

  // Intialize IMU object
  if(useImu)
  {
    pIMU = new Imu(); // Init object
    pNetworking->pushSerialData("IMU Initialized!\n"); // Print confirmation
  }

  // Initialize Haptics Objects
  if(useHaptics)
  {
    pHapticL = new Haptic(LHMP1, LHMP2, LHME);  // Init object
    pNetworking->pushSerialData("Left Haptic: " + pHapticL->printPins());
    pHapticR = new Haptic(RHMP1, RHMP2, RHME);  // Init object
    pNetworking->pushSerialData("Right Haptic: " + pHapticR->printPins());
    pNetworking->pushSerialData("Haptics Initialized!\n"); // Print confirmation
  }

  // Initialize Wheel Objects
  if(useWheels)
  {
    pWheelL = new Wheel(LWMPF, LWMPR);  // Init object
    pNetworking->pushSerialData("Left Wheek: " + pWheelL->printPins());
    pWheelR = new Wheel(RWMPF, RWMPR);  // Init object
    pNetworking->pushSerialData("Right Wheel: " + pWheelR->printPins());
    pNetworking->pushSerialData("Wheels Initialized!\n"); // Print confirmation
  }
}

// Boolean Processing Flags
bool Timer_1HZ_FG = false;
bool Timer_10HZ_FG = false;
bool Timer_30HZ_FG = false;

// Main loop
void loop()
{    
  // 1 HZ ISR
  if(Timer_1HZ_FG)
  {
    digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
    Update_Data(); // Update Sensor Data
    Send_Sensor_Data(); // Push Serial Data
    Timer_1HZ_FG = false;
  }

  // 10 HZ ISR
  if(Timer_10HZ_FG)
  {
    Timer_10HZ_FG = false;
  }  
  
  // 30 HZ ISR
  if(Timer_30HZ_FG)
  {
    Timer_30HZ_FG = false;
  }

}

// ISR's
// Every 1 second (1 FPS)
static void IRAM_ATTR Timer_1Hz_ISR()
{
  Timer_1HZ_FG = true;
}

// Every 100 msec (10 FPS)
static void IRAM_ATTR Timer_10Hz_ISR()
{
  Timer_10HZ_FG = true;
}

// Ever 33 msec (30 FPS)
static void IRAM_ATTR Timer_30Hz_ISR()
{
    Timer_30HZ_FG = true;
}

// Update Sensor Data
void Update_Data()
{
    // Update Sonar Distances
    if (useSonar)
    {
        pS1->readDistance();
        pS2->readDistance();
        pS3->readDistance();
        pS4->readDistance();
    }

    // Update LiDAR Distances
    if (useLidar)
    {
        pLidar->readDistance();
    }

    // Update IMU readings
    if (useImu)
    {
        pIMU->updateData();
    }
}

// send Sensor data to website
void Send_Sensor_Data()
{
  String sensorData;

  if(useSonar)
  {
    sensorData = "S1: ";
    sensorData += pS1->distance;
    sensorData += " S2: ";
    sensorData += pS2->distance;
    sensorData += " S3: ";
    sensorData += pS3->distance;
    sensorData += " S4: ";
    sensorData += pS4->distance;
    sensorData += '\n';
  }

  if(useLidar)
  {
    sensorData += "LiDAR: ";
    sensorData += pLidar->distance;
    sensorData += '\n';
  }

  if(useImu)
  {
    sensorData += "Roll: ";
    sensorData += pIMU->roll;
    sensorData += " Pitch: ";
    sensorData += pIMU->pitch;
    sensorData += " Yaw: ";
    sensorData += pIMU->yaw;
    sensorData += '\n';
  }

  if(useCV)
  {
    char data[512];
    pNetworking->getUDPPacket(data, sizeof(data));
    sensorData += data;
  }

  pNetworking->pushSerialData(sensorData);
  pNetworking->update();
}

// Run full system test and upload data to http://192.168.4.1/
void Test_System()
{
  if(useCV && useHaptics && useImu && useLidar && useSonar && useWheels)
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
}

void Sample_GNC_Alg()
{
  if(useCV && useHaptics && useImu && useLidar && useSonar && useWheels)
  {
    // Example avoidance of object in front
    if(!pWheelL->spinning) pWheelL->startWheel(350, true);
    if(!pWheelR->spinning) pWheelR->startWheel(350, true);

    if (pS2->distance < 50 || pS3->distance < 50)
    {
      pWheelL->startWheel(350, false);
      pWheelR->startWheel(350, false);
      pHapticL->startHaptic(3);
      pHapticR->startHaptic(3);
      delay(1000);
      pWheelL->stopWheel();
      pWheelR->stopWheel(); 
      delay(2000);
      pWheelL->startWheel(350, true);
      pWheelR->startWheel(350, false);
      delay(8000);
      pHapticL->stopHaptic();
      pHapticR->stopHaptic();
      pWheelL->stopWheel();
      pWheelR->stopWheel();
    }
  }
}

