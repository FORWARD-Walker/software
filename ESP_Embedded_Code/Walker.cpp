#include "Walker.h"
#include "Pins.h"

// Create object
Walker::Walker(Networking *pNetworking)
{
    this->curSpeed = 0;
    this->curOffset = 0;

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

    // Initialize Potentiometer Object
    this->pPotentiometer = new Potentiometer(POW);

    // Initialize Camera Object
    this->pCamera = new Camera(pNetworking);
}