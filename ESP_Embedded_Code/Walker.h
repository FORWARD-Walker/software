#ifndef WALKER_H
#define WALKER_H

#include "Sonar.h"
#include "Imu.h"
#include "Haptic.h"
#include "Wheel.h"
#include "Potentiometer.h"

// Walker structure
class Walker
{
public:
  Walker(); // Constructor

  int curSpeed;  // Track current speed
  int curOffset; // Track Current right wheel offset

  // Sensor Objects
  Sonar *pS1;
  Sonar *pS2;
  Sonar *pS3;
  Sonar *pS4;

  // IMU Object
  Imu *pIMU;

  // Haptic Objects
  Haptic *pHapticL;
  Haptic *pHapticR;

  // Wheel Objects
  Wheel *pWheelL;
  Wheel *pWheelR;

  // Potentiometer Object
  Potentiometer *pPotentiometer;
};

#endif