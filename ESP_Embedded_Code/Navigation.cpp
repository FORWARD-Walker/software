#include "Navigation.h"
#include "Constants.h"
#include "Field.h"

// Navigation Algorithms and Helper functions
Navigation(Walker *pWalker, Networking *pNetworking, Enviroment *pEnviroment)
{
    this->pWalker = pWalker;
    this->pNetworking = pNetworking;
    this->pEnviroment = pEnviroment;
}

void Navigation::navigate()
{
    // Check Safe zone
    if (this->pEnviroment->safezoneViolation)
    {
        this->pWalker->pWheelL->stopWheel();
        this->pWalker->pWheelR->stopWheel();
        pulseHaptic(3, 'R');
        pulseHaptic(3, 'L');
        pulseHaptic(3, 'R');
        pulseHaptic(3, 'L');
        pulseHaptic(3, 'R');
        pulseHaptic(3, 'L');
    }
    // Check Road
    else if (this->pEnviroment->road)
    {
        this->emergencyStop();
    }
    // Check crowd
    else if (this->pEnviroment->crowd)
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed - CROWD_THROTTLE_VALUE, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset - CROWD_THROTTLE_VALUE, 'F');
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// P-Field Algorithm
//////////////////////////////////////////////////////////////////////////////////////////////

// ROAD & CROWD
void Navigation::checkForObstacles()
{
    if (!pEnviroment->safezoneViolation && !pEnviroment->crowd && !pEnviroment->road)
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
    std::map<std::string, int> obstacleCount; // Count occurrences of each type of obstacle

    for (const Obstacle &obs : this->pCamera->getObstacles())
    {
        obstacleCount[obs.type]++;
    }

    // Check for three or more 'person'
    if (obstacleCount["person"] >= 3)
    {
        Serial.println("CROWD");
        emergencyStop();
    }

    // Check for three or more 'car'
    if (obstacleCount["car"] >= 3)
    {
        Serial.println("ROAD");
        emergencyStop();
    }
}

// Sample sonar avoidance (unchanged)
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

void Navigation::emergencyStop()
{
    // Stop the wheels immediately
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();
    delay(100);
    this->pWalker->pWheelR->startWheel(500, 'R');
    this->pWalker->pWheelL->startWheel(500, 'R');
    delay(100);
    this->pWalker->pWheelL->stopWheel();
    this->pWalker->pWheelR->stopWheel();
    delay(5000);
}

// Haptic pulse patterns (unchanged)
void Navigation::pulseHaptic(int urgency, char direction)
{
    int delayTime;

    // Determine frequency of haptic pulses based on urgency
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
    }
    else
    {
        this->pWalker->pWheelL->startWheel(350, 'F');
        this->pWalker->pWheelR->startWheel(350, 'F');
        while (this->pWalker->pS2->distance < 100)
        {
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

// Save Frame
void Navigation::saveNewFrame()
{
    // Shift older frames
    for (int i = 0; i < 4; i++)
    {
        frames[i] = frames[i + 1];
    }

    // Create and store the newest frame
    Frame newFrame;

    // Pull data from pEnviroment
    newFrame.xPPs = pEnviroment->xPPs;
    newFrame.yPPs = pEnviroment->yPPs;

    // Store object names from camera
    newFrame.object_names = pEnviroment->object_names;

    // Assign the new frame to the last index
    frames[4] = newFrame;
}

void Navigation::tiltWarning()
{
    if (this->pWalker->pImu->roll > 1)
    {
        // print to network
        this->pNetworking->pushSerialData("You are leaning too heavily to the right. Please don't tip the rollator.\n");
    }
    if (this->pWalker->pImu->roll < -1)
    {
        // print to network
        this->pNetworking->pushSerialData("You are leaning too heavily to the left. Please don't tip the rollator.\n");
    }
}
