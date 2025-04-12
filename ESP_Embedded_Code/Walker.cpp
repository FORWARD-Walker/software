#include "Walker.h"
#include "Pins.h"
#include "Constants.h"

// Create object
Walker::Walker(bool useSonar, bool useImu, bool useHaptics, bool useWheels)
{
    // Setup Sonar sensors if connected
    if (useSonar)
    {
        // Initizalize the Sonar objects
        this->pS1 = new Sonar(TRIG1, ECHO1);
        this->pS2 = new Sonar(TRIG2, ECHO2);
        this->pS3 = new Sonar(TRIG3, ECHO3);
        this->pS4 = new Sonar(TRIG4, ECHO4);
    }

    // Intialize IMU object
    if (useImu)
    {
        this->pIMU = new Imu(); // Init object
    }

    // Initialize Haptics Objects
    if (useHaptics)
    {
        this->pHapticL = new Haptic(LHMP1, LHMP2, LHME); // Init object
        this->pHapticR = new Haptic(RHMP1, RHMP2, RHME); // Init object
    }

    // Initialize Wheel Objects
    if (useWheels)
    {
        this->pWheelL = new Wheel(LWMPF, LWMPR); // Init object
        this->pWheelR = new Wheel(RWMPF, RWMPR); // Init object
    }

    // Initialize Peripherals
    if (usePotentiometer)
    {
        this->pPOW = new Potentiometer(POW); // Init 
    }

    if (useHeadlight)
    {
        this->pLIGHT = new Headlight(LIGHT); // Init
    }

    if (usePhotoresistor)
    {
        this->pPHOTO = new Photoresistor(PHOTO, PHOTORESISTOR_RES); //Init
    }
}
