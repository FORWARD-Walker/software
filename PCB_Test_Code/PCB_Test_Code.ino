// # Include
#include <Wire.h>  // I2C
#include "Lidar.h"
#include "Imu.h"

// # Defines
#define LED 2

// I2C bus
#define SCL 22
#define SDA 21

// IMU Object
Imu* pIMU = NULL;

// LiDAR Object
Lidar* pLidar = NULL;

hw_timer_t* Timer_1Hz = NULL;
static void IRAM_ATTR Timer_1Hz_ISR();
bool Timer_1HZ_FG = false;

void setup()
{
  // Set up serial
  Serial.begin(115200);  // Init Serial

  // Set up I2C
  Wire.begin(SDA, SCL);

  // Setup heartbeat
  pinMode(LED, OUTPUT);     // Set up LED as output
  digitalWrite(LED, HIGH);  // Init to high

  pLidar = new Lidar();  // Init object
  Serial.println("LiDAR Initialized!\n");

  pIMU = new Imu();                                   // Init object
  Serial.println("IMU Initialized!\n");  // Print confirmation

  // Timer 1 Hz
  Timer_1Hz = timerBegin(1000000);
  timerAttachInterrupt(Timer_1Hz, &Timer_1Hz_ISR);
  timerAlarm(Timer_1Hz, 1000000, true, 0);
}

// Main loop
void loop() 
{
  // 1 HZ ISR
  if (Timer_1HZ_FG)
  {
    pIMU->updateData();
    pLidar->readDistance();

    Serial.println(pLidar->distance);
    Serial.println(pIMU->roll);
    Serial.println(pIMU->pitch);
    Serial.println(pIMU->yaw);

    // Reset ISR
    Timer_1HZ_FG = false;
    digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat
  }
}

// Every 1 second (1 FPS)
static void IRAM_ATTR Timer_1Hz_ISR()
{
  Timer_1HZ_FG = true;
}

