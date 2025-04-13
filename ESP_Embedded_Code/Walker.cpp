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

    this->forward = {0, 1}; // Initialize APF direction vector

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
    if (useWheels)
    {
        this->pWheelL = new Wheel(LWMPF, LWMPR);
        this->pWheelR = new Wheel(RWMPF, RWMPR);
    }

    // Initialize Potentiometer
    this->pPotentiometer = new Potentiometer(POW);

    // Initialize Headlight
    this->pLight = new Headlight(HEADLIGHT);

    // Initialize Photoresistor
    this->pPhoto = new Photoresistor(PHOTO, PHOTORESISTOR_RES);
}

// Invoke in Navigation.cpp to steer the Walker!! Use the potentiometer speed as argument 2
void Walker::steer(std::vector<double> direction_vector, int speed)
{
    this->curSpeedL = static_cast<int>(direction_vector[0] * speed);
    this->curSpeedR = static_cast<int>(direction_vector[1] * speed);

    this->curSpeedR += this->curOffset;

    this->pWheelL->startWheel(this->curSpeedL, 'F');
    this->pWheelR->startWheel(this->curSpeedR, 'F');
}