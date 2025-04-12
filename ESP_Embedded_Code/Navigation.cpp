#include "Navigation.h"
#include "Constants.h"

// Navigation constructor
Navigation::Navigation(Walker *pWalker, Camera *pCamera)
{
    this->pWalker = pWalker;
    this->pCamera = pCamera;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// P-Field Algorithm
//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////

// ROAD & CROWD
void Navigation::checkForObstacles()
{
    std::map<std::string, int> obstacleCount; // Count occurrences of each type of obstacle

    for (const Obstacle &obs : this->pCamera->getObstacles()) {
        obstacleCount[obs.type]++;
    }

    // Check for three or more 'person'
    if (obstacleCount["person"] >= 3) {
        Serial.println("CROWD");
        emergencyStop();
    }

    // Check for three or more 'car'
    if (obstacleCount["car"] >= 3) {
        Serial.println("ROAD");
        emergencyStop();
    }
}

// Sample sonar avoidance (unchanged)
void Navigation::Sample_Sonar_Avoidance()
{
    if (this->pWalker->pS2->distance < 100 || this->pWalker->pS3->distance < 100) {
        if (this->pWalker->pS2->distance > this->pWalker->pS3->distance) {
            pulseHaptic(3, 'R');
            veer(45.0, 'L');
            pulseHaptic(3, 'R');
        } else {
            pulseHaptic(3, 'L');
            veer(45.0, 'R');
            pulseHaptic(3, 'L');
        }
    }
}

void Navigation::emergencyStop()
{
    // Stop the wheels immediately
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();

    // Define pulse pattern for haptics (like a ringtone)
    int pattern[] = {100, 200, 100, 400}; // Duration of buzz pulses in milliseconds
    int numBuzzes = sizeof(pattern) / sizeof(pattern[0]);

    // Buzz haptics on both sides with the pulse pattern
    for (int i = 0; i < numBuzzes; ++i) {
        this->pWalker->pHapticL->startHaptic();
        this->pWalker->pHapticR->startHaptic();
        delay(pattern[i]);
        this->pWalker->pHapticL->stopHaptic();
        this->pWalker->pHapticR->stopHaptic();
        delay(100); // Short pause between pulses
    }

    Serial.println("Emergency Stop Activated: Haptics Buzzing");
    delay(100); // Ensure haptics finish buzzing
}

// Haptic pulse patterns (unchanged)
void Navigation::pulseHaptic(int urgency, char direction)
{
    int delayTime;

    // Determine frequency of haptic pulses based on urgency
    switch (urgency) {
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

// Veer function (unchanged, uses pivot)
void Navigation::veer(float aspect, char direction)
{
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();

    // Pivot to desired direction
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
    } else {
        this->pWalker->pWheelL->startWheel(350, 'F');
        this->pWalker->pWheelR->startWheel(350, 'F');
        while (this->pWalker->pS2->distance < 100) {
            this->pWalker->pS2->updateDistance();
        }
    }

    // Stop and pivot back
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();
    pivot(-aspect, direction == 'L' ? 'R' : 'L');

    // Retart wheels
    this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
    this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
}

// Pivot function (unchanged)
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

void Navigation::tiltWarning()
{
  if(this->pWalker->pImu->roll > 1)
  {
    // print to network
    this->pWalker->pNetworking->pushSerialData("You are leaning too heavily to the right. Please don't tip the rollator.\n");

  }
  if(this->pWalker->pImu->roll < -1)
  {
    // print to network
    this->pWalker->pNetworking->pushSerialData("You are leaning too heavily to the left. Please don't tip the rollator.\n");
  }
}