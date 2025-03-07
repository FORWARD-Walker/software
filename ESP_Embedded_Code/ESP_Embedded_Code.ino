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

// Sonar Pins
#define TRIG1 25  // Trigger pin for sensor 1
#define ECHO1 34  // Echo pin for sensor 1
#define TRIG2 26  // Trigger pin for sensor 2
#define ECHO2 35  // Echo pin for sensor 2
#define TRIG3 22  // Trigger pin for sensor 3
#define ECHO3 23  // Echo pin for sensor 3
#define TRIG4 19  // Trigger pin for sensor 4
#define ECHO4 21  // Echo pin for sensor 4

// Haptic pins
#define LHMP1 12  // Left Haptic motor pin 1
#define LHMP2 13  // Left Haptic motor pin 2
#define LHME  27  // Left Haptic motor enable pin
#define RHMP1 15  // Right Haptic motor pin 1
#define RHMP2 2  // Right Haptic motor pin 2
#define RHME  14   // Right Haptic motor enable pin

// Wheel Pins
#define LWMPF 16  // Left Wheel motor pin 1
#define LWMPR 17  // Left Wheel motor pin 2
#define RWMPF 5  // Right Wheel motor pin 1
#define RWMPR 18  // Right Wheel motor pin 2

// Boolean flags
bool useCV = false; // Set to use computer vision
bool useSonar = true; // Set to use sonar functions
bool useLidar = false; // Set to use LiDAR functions
bool useImu = false; // Set to use IMU
bool useHaptics = false; // Set to use Haptics
bool useWheels = true; // Set to use Wheels

// Network Object
Networking* pNetworking = NULL;

// Utils
utils* utils;

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
void Send_Sensor_Data();
void Sample_Haptic_Buzz();
void Sample_Sonar_Avoidance();
void braking_demo();
void veer(float aspect, char direction);
void pivot(float aspect, char direction);
void pulseHaptic(int urgency, char direction);

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
  Wire.begin(32,33);

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
  pNetworking = new Networking();  // Init object
  pNetworking->pushSerialData("Network Initialized!\n");

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
    pNetworking->pushSerialData("Left Wheel: " + pWheelL->printPins());
    pWheelR = new Wheel(RWMPF, RWMPR);  // Init object
    pNetworking->pushSerialData("Right Wheel: " + pWheelR->printPins());
    pNetworking->pushSerialData("Wheels Initialized!\n"); // Print confirmation

    // Start wheels
    pWheelL->startWheel(1000, 'F');
    pWheelR->startWheel(1000, 'F');
  }
}

// Boolean Processing Flags
bool Timer_1HZ_FG = false;
bool Timer_10HZ_FG = false;
bool Timer_30HZ_FG = false;

// Main loop
void loop()
{ 
  // 30 HZ ISR
  if(Timer_30HZ_FG)
  {
    Update_Data(); // Update Sensor Data
    braking_demo();

    Timer_30HZ_FG = false;
  }

  // 10 HZ ISR
  if(Timer_10HZ_FG)
  {
    // Reset ISR
    Timer_10HZ_FG = false;
  }


  // 1 HZ ISR
  if(Timer_1HZ_FG)
  {
    Send_Sensor_Data(); // Push Serial Data
    Timer_1HZ_FG = false;
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

void Sample_Sonar_Avoidance()
{
  if(pS2->distance < 100 || pS3->distance < 100)
  {
    pNetworking->pushSerialData("Object within 1 meter!\n");
    if(pS2->distance > pS3->distance)
    {
      pNetworking->pushSerialData("Veering Left!\n");
      pulseHaptic(3, 'R');
      veer(45.0, 'L');
    }
    else
    {
      pNetworking->pushSerialData("Veering Right!\n");
      pulseHaptic(3, 'L');     
      veer(45.0, 'R');
    }
  }
}

void Sample_Haptic_Buzz()
{
      if (pS1->distance < 50)
    {
      pHapticL->startHaptic(3);
      delay(500);
    }
    pHapticL->stopHaptic();
    if (pS4->distance < 50)
    {
      pHapticR->startHaptic(3);
      delay(500);
    }
    pHapticR->stopHaptic();
    if (pS2->distance > 300 && pS2->distance < 500)
    {
      pHapticL->startHaptic(3);
      delay(500);
    }
    pHapticL->stopHaptic();
    if (pS3->distance > 300 && pS3->distance < 500)
    {
      pHapticR->startHaptic(3);
      delay(500);
    }
    pHapticR->stopHaptic();
}

// Update Sensor Data
void Update_Data()
{
    // Update Sonar Distances
    if (useSonar)
    {
      pS1->updateDistance();
      pS2->updateDistance();
      pS3->updateDistance();
      pS4->updateDistance();
    }

    // Update LiDAR Distances
    if (useLidar)
    {
      pLidar->updateDistance();
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

void veer(float aspect, char direction)
{
  // Stop Wheels
  pWheelL->stopWheel();
  pWheelR->stopWheel();

  // Pivot
  pivot(aspect, direction);

  // Move forward till object is out of way
  switch (direction)
  {
    // Start Wheels
    pWheelL->startWheel(350, 'F');
    pWheelR->startWheel(350, 'F');

    // Wait for appropriate sensor to be no longer close range
    case 'L':
      while(pS3->distance < 100) { pS3->updateDistance(); }
      break;
    case 'R':
      while(pS2->distance < 100) { pS2->updateDistance(); }
      break;
    default:
      pNetworking->pushSerialData("Invalid Veer Direction!");
      break;

    // Stop wheels
    pWheelL->stopWheel();
    pWheelR->stopWheel();
  }

  // Pivot back
  aspect = -aspect;
  if (direction == 'L') direction == 'R';
  else direction == 'L';
  pivot(aspect, direction);

  // Retart wheels
  pWheelL->startWheel(350, 'F');
  pWheelR->startWheel(350, 'F');
}

// Routine to stop and pivot the walker by [aspect] degrees
void pivot(float aspect, char direction)
{
  float initAngle = pIMU->yaw;
  float deltaAngle = 0.0;

  // Pivot Walker 
  if (direction == 'L')
  {
    pWheelR->startWheel(350, 'F');
    while(fabs(deltaAngle) < aspect)
    { 
      pIMU->updateData();
      deltaAngle = fmod((initAngle - pIMU->yaw) + 180.0, 360.0) - 180.0;
    } 
    pWheelR->stopWheel();
  }
  else
  {
    pWheelL->startWheel(350, 'F');
    while(fabs(deltaAngle) < aspect)
    { 
      pIMU->updateData();
      deltaAngle = fmod((initAngle - pIMU->yaw) + 180.0, 360.0) - 180.0;
    } 
    pWheelL->stopWheel();
  }
}

// Haptic pulse patterns
void pulseHaptic(int urgency, char direction)
{
  int delayTime; 

  // Determine freq to buzz haptic
  switch (urgency)
  {
    case 1:
      delayTime = 500;
      break;
    case 2:
      delayTime = 250;
      break;    
    case 3:
      delayTime = 100;
      break;
    default:
      Serial.println("Invalid pulse code.");
  }

  // Buzz appropritate side
  if (direction == 'L')
  {
    pHapticL->startHaptic(urgency);
    delay(delayTime);
    pHapticL->stopHaptic();
    pHapticL->startHaptic(urgency);
    delay(delayTime);
    pHapticL->stopHaptic();
    pHapticL->startHaptic(urgency);
    delay(delayTime);
    pHapticL->stopHaptic();
  }
  else
  {
    pHapticR->startHaptic(urgency);
    delay(delayTime);
    pHapticR->stopHaptic();
    pHapticR->startHaptic(urgency);
    delay(delayTime);
    pHapticR->stopHaptic();
    pHapticR->startHaptic(urgency);
    delay(delayTime);
    pHapticR->stopHaptic();
  }
}

// braking function
void braking_demo()
{
  if(pS2->distance <= 100 || pS3->distance <= 100)
  {
    Serial.println("Coming to a stop.");
    pWheelL->stopWheel();
    pWheelR->stopWheel();
    delay(250);
    pWheelR->startWheel(500, 'R');
    pWheelL->startWheel(500, 'R');
    delay(250);
    pWheelL->stopWheel();
    pWheelR->stopWheel();
  }
}

