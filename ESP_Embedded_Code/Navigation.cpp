#include "Navigation.h"
#include "Constants.h"

// Navigation Algorithms and Helper functions
Navigation::Navigation(Walker *pWalker)
{
    this->pWalker = pWalker;
}

// Determine Potentiometer Speed
void Navigation::setSpeed()
{
    this->pWalker->pPotentiometer->readValue();
    int potVal = this->pWalker->pPotentiometer->value;
    if (potVal > 3500)
    {
        this->pWalker->pWheelL->stopWheel();
        this->pWalker->pWheelR->stopWheel();
        this->pWalker->curSpeed = 0;
        this->pWalker->curOffset = 0;
    }
    else if (potVal > 1000)
    {
        this->pWalker->curSpeed = SPEED_1;
        this->pWalker->curOffset = SPEED_1_RIGHT_WHEEL_OFFSET;
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
    }
    else
    {
        this->pWalker->curSpeed = SPEED_2;
        this->pWalker->curOffset = SPEED_2_RIGHT_WHEEL_OFFSET;
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
    }
}

void Navigation::Sample_Sonar_Avoidance()
{
    if (this->pWalker->pS2->distance < 100 || this->pWalker->pS3->distance < 100)
    {
        if (this->pWalker->pS2->distance > this->pWalker->pS3->distance)
        {
            pulseHaptic(3, 'R');
            veer(45.0, 'L');
            pulseHaptic(3, 'R');
        }
        else
        {
            pulseHaptic(3, 'L');
            veer(45.0, 'R');
            pulseHaptic(3, 'L');
        }
    }
}

// braking function
void Navigation::emergencyStop()
{
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();
    delay(100);
    this->pWalker->pWheelR->startWheel(500, 'R');
    this->pWalker->pWheelL->startWheel(500, 'R');
    delay(100);
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();
}

// Haptic pulse patterns
void Navigation::pulseHaptic(int urgency, char direction)
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
        this->pWalker->pHapticL->startHaptic();
        delay(delayTime);
        this->pWalker->pHapticL->stopHaptic();
        this->pWalker->pHapticL->startHaptic();
        delay(delayTime);
        this->pWalker->pHapticL->stopHaptic();
        this->pWalker->pHapticL->startHaptic();
        delay(delayTime);
        this->pWalker->pHapticL->stopHaptic();
    }
    else
    {
        this->pWalker->pHapticR->startHaptic();
        delay(delayTime);
        this->pWalker->pHapticR->stopHaptic();
        this->pWalker->pHapticR->startHaptic();
        delay(delayTime);
        this->pWalker->pHapticR->stopHaptic();
        this->pWalker->pHapticR->startHaptic();
        delay(delayTime);
        this->pWalker->pHapticR->stopHaptic();
    }
}

void Navigation::veer(float aspect, char direction)
{
    // Stop Wheels
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();

    // Pivot
    pivot(aspect, direction);

    // Move forward till object is out of way
    switch (direction)
    {
        // Start Wheels
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');

    // Wait for appropriate sensor to be no longer close range
    case 'L':
        while (this->pWalker->pS3->distance < 100)
        {
            this->pWalker->pS3->updateDistance();
        }
        break;
    case 'R':
        while (this->pWalker->pS2->distance < 100)
        {
            this->pWalker->pS2->updateDistance();
        }
        break;
    default:
        break;

        // Stop wheels
        this->pWalker->pWheelL->stopWheel();
        this->pWalker->pWheelR->stopWheel();
    }

    // Pivot back
    aspect = -aspect;
    if (direction == 'L')
        direction == 'R';
    else
        direction == 'L';
    pivot(aspect, direction);

    // Retart wheels
    this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
    this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
}

// Routine to stop and pivot the walker by [aspect] degrees
void Navigation::pivot(float aspect, char direction)
{
    float initAngle = this->pWalker->pIMU->yaw;
    float deltaAngle = 0.0;

    // Pivot Walker
    if (direction == 'L')
    {
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
        while (fabs(deltaAngle) < aspect)
        {
            this->pWalker->pIMU->updateData();
            deltaAngle = fmod((initAngle - this->pWalker->pIMU->yaw) + 180.0, 360.0) - 180.0;
        }
        this->pWalker->pWheelR->stopWheel();
    }
    else
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
        while (fabs(deltaAngle) < aspect)
        {
            this->pWalker->pIMU->updateData();
            deltaAngle = fmod((initAngle - this->pWalker->pIMU->yaw) + 180.0, 360.0) - 180.0;
        }
        this->pWalker->pWheelL->stopWheel();
    }
}
