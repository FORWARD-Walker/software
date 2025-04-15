#include "Navigation.h"
#include "Constants.h"

#include <vector>

std::vector<double> norm_rLOS_list;
std::vector<double> distCam_list;

// Navigation Algorithms and Helper functions
Navigation::Navigation(Walker *pWalker, Networking *pNetworking, Environment *pEnvironment)
{
    this->pWalker = pWalker;
    this->pNetworking = pNetworking;
    this->pEnvironment = pEnvironment;
}

void Navigation::navigate()
{
    // Check Safe zone
    if (this->pEnvironment->safezoneViolation)
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
    else if (this->pEnvironment->road)
    {
        this->emergencyStop();
    }
    // Check crowd
    else if (this->pEnvironment->crowd)
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL - CROWD_THROTTLE_VALUE, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR - CROWD_THROTTLE_VALUE, 'F');
    }

    // Navigate user through world
    this->saveNewFrame(); // update data
    for (int i = 0; i < 5; i++)
    {
        this->pNetworking->pushSerialData("Frame :");
        Frame frame = this->frames[i];
        for (int j = 0; j < frame.object_names.size(); j++)
        {
            String framestr = "Object: " + frame.object_names.at(j) + " Xpp: " + frame.xPPs.at(j) + " Ypp: " + frame.yPPs.at(j) + "\n";
            this->pNetworking->pushSerialData(framestr);
        }
    }
    this->pNetworking->update();

    //////////////////////////////////////////////////////////////////////////////////////////////
    // Artificial Potential Field Algorithm for multi-obstacle avoidance
    // note** might want to increase update frequency
    //////////////////////////////////////////////////////////////////////////////////////////////
    
    // Iterate through all obstacles in view
    for (int i = 0; i < frame.object_names.size(); i++) {
        Vector2D rV_i = utils::repulsionVector(frame.xPPs(i), frame.yPPs(i)); // Calculate repulsion vector
        rV_total.x += rV_i.x; // accumulate x-direction commands
        rV_total.y += rV_i.y; // accumulate y-direction commands

        Vector3D rLOS_i = utils::pixel2los(frame.xPPs(i), frame.yPPs(i)); // Calculate 3D LOS vector

        double norm_rLOS_i = utils::mag3d(rLOS_i.normalize()); // Nornmalized 3D LOS vector

        double distCam_i = utils::mag3d(rLOS_i); // Range estimate from camera to obstacle

        norm_rLOS_list.push_back(norm_rLOS_i); // Store normalized LOS magnitudes
        distCam_list.push_back(distCam_i); // Store distance estimates
    }

    Vector2D vectAPF = {rV_total.x, rV_total.y} + {0, 1}; // Combine direction vector with forward vector
    
    Vector2D normVectAPF = vectAPF.normalize(); // Normalize for stability
   
    this->pWalker->forward = utils::C2R(normVectAPF, this->pWalker->pIMU->yaw); // Transform to Rollator frame of reference

    int speed = setSpeed(); // Grab speed command from potentiometer

    this->pWalker->steer(this->pWalker->forward, speed); // Tell the rollator the motor differential
}

int Navigation::setSpeed()
{

    if (!pEnvironment->safezoneViolation && !pEnvironment->crowd && !pEnvironment->road)
    {
        this->pWalker->pPotentiometer->readValue();
        int potVal = this->pWalker->pPotentiometer->value;
        if (potVal > 3500)
        {
            this->pWalker->pWheelL->stopWheel();
            this->pWalker->pWheelR->stopWheel();
            this->pWalker->curSpeedL = 0;
            this->pWalker->curSpeedR = 0;
            this->pWalker->curOffset = 0;
            return 0;
        }
        else if (potVal > 1000)
        {
            this->pWalker->curSpeedL = SPEED_1;
            this->pWalker->curSpeedR = SPEED_1 + SPEED_1_RIGHT_WHEEL_OFFSET;
            this->pWalker->curOffset = SPEED_1_RIGHT_WHEEL_OFFSET;
            this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
            this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');
            return SPEED_1;
        }
        else
        {
            this->pWalker->curSpeedL = SPEED_2;
            this->pWalker->curSpeedR = SPEED_2 + SPEED_2_RIGHT_WHEEL_OFFSET;
            this->pWalker->curOffset = SPEED_2_RIGHT_WHEEL_OFFSET;
            this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
            this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');
            return SPEED_2;
        }
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
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');

    // Wait for appropriate sensor to be no longer close range
    case 'L':
        while (this->pWalker->pS3->distance < 100)
        {
            this->pWalker->pS3->updateDistance();
        }
        break;
    case 'R':
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');
        while (this->pWalker->pS2->distance < 100)
        {
            this->pWalker->pS2->updateDistance();
        }
        break;
    }
    default:
        break;

        // Stop and pivot back
        this->pWalker->pWheelL->stopWheel();
        this->pWalker->pWheelR->stopWheel();
    }

    pivot(-aspect, direction == 'L' ? 'R' : 'L');

    // Retart wheels
    this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
    this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');
}

// Pivot function (unchanged)
void Navigation::pivot(float aspect, char direction)
{
    float initAngle = this->pWalker->pIMU->yaw;
    float deltaAngle = 0.0;

    // Pivot Walker
    if (direction == 'L')
    {
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');
        while (fabs(deltaAngle) < aspect)
        {
            this->pWalker->pIMU->updateData();
            deltaAngle = fmod((initAngle - this->pWalker->pIMU->yaw) + 180.0, 360.0) - 180.0;
        }
        this->pWalker->pWheelR->stopWheel();
    }
    else
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
        while (fabs(deltaAngle) < aspect)
        {
            this->pWalker->pIMU->updateData();
            deltaAngle = fmod((initAngle - this->pWalker->pIMU->yaw) + 180.0, 360.0) - 180.0;
        }
        this->pWalker->pWheelL->stopWheel();
    }
}

// Calculate precise differential drive to follow direction vector
void Navigation::steer(Vector2D direction_vector, int speed)
{
    // Scale the normalized direction vector by the speed
    double vx = direction_vector.x;
    double vy = direction_vector.y;

    // Convert to wheel speeds using differential drive logic
    this->pWalker->curSpeedL = static_cast<int>((vy + vx) * speed); // right turn, left wheel faster
    this->pWalker->curSpeedR = static_cast<int>((vy - vx) * speed); // left turn, right wheel faster

    // Apply motor offset correction to the right wheel
    this->pWalker->curSpeedR += this->pWalker->curOffset;

    // Start the wheels (assuming 'F' = forward)
    this->pWalker->pWheelL->startWheel(this->pWalker->curSpeedL, 'F');
    this->pWalker->pWheelR->startWheel(this->pWalker->curSpeedR, 'F');
}

// Save Frame
void Navigation::saveNewFrame()
{
    // Shift older frames down the array
    for (int i = 4; i > 0; i--)
    {
        frames[i] = this->frames[i - 1];
    }

    // Create and store the newest frame at index 0
    Frame newFrame;

    // Pull data from pEnvironment
    newFrame.xPPs = this->pEnvironment->xPPs;
    newFrame.yPPs = this->pEnvironment->yPPs;

    // Store object names from camera
    newFrame.object_names = this->pEnvironment->object_names;

    // Assign the new frame to the first index
    this->frames[0] = newFrame;
}

void Navigation::tiltWarning()
{
    if (this->pWalker->pIMU->roll > 1)
    {
        // print to network
        this->pNetworking->pushSerialData("You are leaning too heavily to the right. Please don't tip the rollator.\n");
    }
    if (this->pWalker->pIMU->roll < -1)
    {
        // print to network
        this->pNetworking->pushSerialData("You are leaning too heavily to the left. Please don't tip the rollator.\n");
    }
}
