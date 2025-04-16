#include "Navigation.h"
#include "Constants.h"
#include "utils.cpp"
#include <vector>

// Navigation Algorithms and Helper functions
Navigation::Navigation(Walker *pWalker, Networking *pNetworking, Environment *pEnvironment)
{
    this->pWalker = pWalker;
    this->pNetworking = pNetworking;
    this->pEnvironment = pEnvironment;
}

void Navigation::navigate()
{
    // Get Environment Frame
    this->curFrame.object_names = this->pEnvironment->object_names;
    this->curFrame.xPPs = this->pEnvironment->xPPs;
    this->curFrame.yPPs = this->pEnvironment->yPPs;

    this->setSpeed(); // Grab speed command from potentiometer

    // Check Safe zone
    if (this->pWalker->curSpeed == 0)
    {
        this->pWalker->pWheelL->stopWheel();
        this->pWalker->pWheelR->stopWheel();
        return;
    }
    else if (this->pEnvironment->S2Trig && this->pEnvironment->S3Trig)
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
    // Bump walker right
    else if (this->pEnvironment->S2Trig)
    {
        this->pWalker->pWheelR->startWheel(MIN_SPEED + this->pWalker->curOffset, 'F');
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
    }
    // Bump walker left
    else if (this->pEnvironment->S3Trig)
    {
        this->pWalker->pWheelL->startWheel(MIN_SPEED, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
    }
    // Bump walker right
    else if (this->pEnvironment->S1Trig && !this->pEnvironment->S4Trig)
    {
        this->pWalker->pWheelR->startWheel(MIN_SPEED + this->pWalker->curOffset, 'F');
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed, 'F');
    }
    // Bump walker left
    else if (this->pEnvironment->S4Trig && !this->pEnvironment->S1Trig)
    {
        this->pWalker->pWheelL->startWheel(MIN_SPEED, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset, 'F');
    }
    // Check Road
    else if (this->pEnvironment->road)
    {
        this->emergencyStop();
    }
    // Check crowd
    else if (this->pEnvironment->crowd)
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed - CROWD_THROTTLE_VALUE, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset - CROWD_THROTTLE_VALUE, 'F');
    }
    else
    {
        // Navigate
        Vector2D rV_total = {0.0, 0.0};
        Vector3D rLOS_i = {0.0, 0.0, 0.0};
        double norm_rLOS_i = 0.0;
        double distCam_i = 0.0;

        // Iterate through all obstacles in view (max 5)
        for (int i = 0; i < this->curFrame.object_names.size(); i++)
        {
            Vector2D rV_i = utils::repulsionVector(this->curFrame.xPPs[i], this->curFrame.yPPs[i]); // Calculate repulsion vector

            Vector2D rV_i_norm = rV_i.normalize(); // Normalize repulsion vector

            rV_total.x += rV_i_norm.x; // accumulate x-direction commands
            rV_total.y += rV_i_norm.y; // accumulate y-direction commands

            Vector3D rLOS_i = utils::pixel2los(this->curFrame.xPPs[i], this->curFrame.yPPs[i]); // Calculate 3D LOS vector

            norm_rLOS_i = utils::mag3d(rLOS_i.normalize()); // Normalized 3D LOS vector

            distCam_i = utils::mag3d(rLOS_i); // Range estimate from camera to obstacle
        }

        // Combine direction vector with forward vector
        Vector2D vectAPF = {0.0, 0.0};
        vectAPF.x = rV_total.x;
        vectAPF.y = rV_total.y + 1.0;

        Vector2D normVectAPF = vectAPF.normalize(); // Normalize for stability

        this->pWalker->forward = utils::C2R(normVectAPF, this->pWalker->pIMU->yaw); // Transform to Rollator frame of reference
        this->pWalker->forward.y = max(0.0, this->pWalker->forward.y);

        this->steer(this->pWalker->forward); // Tell the rollator the motor differential
    }
}

void Navigation::setSpeed()
{
    this->pWalker->pPotentiometer->readValue();
    int potVal = this->pWalker->pPotentiometer->value;
    if (potVal > 3500)
    {
        this->pWalker->curSpeed = 0;
        this->pWalker->curOffset = 0;
    }
    else if (potVal > 1000)
    {
        this->pWalker->curSpeed = SPEED_1;
        this->pWalker->curOffset = SPEED_1_RIGHT_WHEEL_OFFSET;
    }
    else
    {
        this->pWalker->curSpeed = SPEED_2;
        this->pWalker->curOffset = SPEED_2_RIGHT_WHEEL_OFFSET;
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

// Calculate precise differential drive to follow direction vector
void Navigation::steer(Vector2D direction_vector)
{
    // Speed avg
    int speedAvgL = (MIN_SPEED + this->pWalker->curSpeed) / 2;
    int speedAvgR = (MIN_SPEED + this->pWalker->curOffset + this->pWalker->curSpeed + this->pWalker->curOffset) / 2;

    // Scale the normalized direction vector by the speed
    double vx = direction_vector.x;
    double vy = direction_vector.y;

    // Convert to wheel speeds using differential drive logic
    int speedL = static_cast<int>((vy + vx) * ((this->pWalker->curSpeed - MIN_SPEED) / 2) + speedAvgL); // right turn, left wheel faster
    int speedR = static_cast<int>((vy - vx) * ((this->pWalker->curSpeed - MIN_SPEED) / 2) + speedAvgR); // left turn, right wheel faster

    // Start the wheels (assuming 'F' = forward)
    this->pWalker->pWheelL->startWheel(speedL, 'F');
    this->pWalker->pWheelR->startWheel(speedR, 'F');
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
