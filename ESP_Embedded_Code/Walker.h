#ifndef WALKER_H
#define WALKER_H

#include "Sonar.h"
#include "Imu.h"
#include "Haptic.h"
#include "Wheel.h"
#include "Potentiometer.h"
#include "Camera.h"
#include "Headlight.h"
#include "Photoresistor.h"
#include "Networking.h"

// Walker structure
class Walker
{
public:
  Walker(Networking *pNetworking); // Constructor

  std::vector<double> forward; // APF direction vector
  void steer();                // Determines motor differential speed based on the APF direction vector

  int curSpeedL; // left wheel speed command
  int curSpeedR; // right wheel speed command
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

  // Camera Object
  Camera *pCamera;

  // Headlight
  Headlight *pLight;

  // Photoresistor
  Photoresistor *pPhoto;
};

#endif