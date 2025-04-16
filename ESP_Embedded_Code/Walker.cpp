#include "Walker.h"
#include "Pins.h"
#include "Constants.h"
#include <iostream>

// Create object
Walker::Walker(Networking *pNetworking)
{
    this->curSpeedL = 0;
    this->curSpeedR = 0;
    this->curOffset = 0;

    this->forward = {0.0, 1.0}; // Initialize APF direction vector

    // Initizalize the Sonar objects
    this->pS1 = new Sonar(TRIG1, ECHO1);
    this->pS2 = new Sonar(TRIG2, ECHO2);
    this->pS3 = new Sonar(TRIG3, ECHO3);
    this->pS4 = new Sonar(TRIG4, ECHO4);

    // Intialize IMU object
    this->pIMU = new Imu();

    // Initialize Haptics Objects
    this->pHapticL = new Haptic(LHMP1, LHMP2);
    this->pHapticR = new Haptic(RHMP1, RHMP2);

    // Initialize Wheel Objects
    this->pWheelL = new Wheel(LWMPF, LWMPR);
    this->pWheelR = new Wheel(RWMPF, RWMPR);

    // Initialize Camera
    this->pCamera = new Camera(pNetworking);

    // Initialize Potentiometer
    this->pPotentiometer = new Potentiometer(POW);

    // Initialize Headlight
    this->pHeadlight = new Headlight(HEADLIGHT);

    // Initialize Photoresistor
    this->pPhotoresistor = new Photoresistor(PHOTO, PHOTORESISTOR_RES);
}
