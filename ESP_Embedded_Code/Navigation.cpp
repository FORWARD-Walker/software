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

    // Print Frame
    String frameData = "Frame: ";
    for (int i = 0; i < this->curFrame.object_names.size(); i++)
    {
        frameData += "Object Name: " + String(this->curFrame.object_names[i]) + " X: " + String(this->curFrame.xPPs[i]) + " Y: " + String(this->curFrame.yPPs[i]) + "\n";
    }
    this->pNetworking->pushSerialData(frameData);

    // Get Sonar Values
    int S1_dist = this->pWalker->pS1->distance;
    int S2_dist = this->pWalker->pS2->distance;
    int S3_dist = this->pWalker->pS3->distance;
    int S4_dist = this->pWalker->pS4->distance;

    this->setSpeed(); // Grab speed command from potentiometer

    // Check Safe zone
    if (this->pWalker->curSpeed == 0)
    {
        this->pWalker->pWheelL->stopWheel();
        this->pWalker->pWheelR->stopWheel();
        return;
    }
    else if (S2_dist < SONAR_FRONT_SAFEZONE && S3_dist < SONAR_FRONT_SAFEZONE)
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
    // Sonar Navigate
    else if (S2_dist < SONAR_NAVIGATION_ZONE || S3_dist < SONAR_NAVIGATION_ZONE && (abs(S2_dist - S3_dist) > 10))
    {
        // If S2 has priority
        int speedOffset = 0;
        if (S2_dist < S3_dist)
        {
            if (S2_dist < SONAR_NAV_ZONE_1)
                speedOffset = SONAR_NAV_ZONE_1_OFFSET;
            if (S2_dist < SONAR_NAV_ZONE_2)
                speedOffset = SONAR_NAV_ZONE_2_OFFSET;
            if (S2_dist < SONAR_NAV_ZONE_3)
                speedOffset = SONAR_NAV_ZONE_3_OFFSET;

            this->pWalker->pWheelR->startWheel(MIN_SPEED + this->pWalker->curOffset, 'F');
            this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed + speedOffset, 'F');
        }
        else
        {
            if (S3_dist < SONAR_NAV_ZONE_1)
                speedOffset = SONAR_NAV_ZONE_1_OFFSET;
            if (S3_dist < SONAR_NAV_ZONE_2)
                speedOffset = SONAR_NAV_ZONE_2_OFFSET;
            if (S3_dist < SONAR_NAV_ZONE_3)
                speedOffset = SONAR_NAV_ZONE_3_OFFSET;

            this->pWalker->pWheelL->startWheel(MIN_SPEED, 'F');
            this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset + speedOffset, 'F');
        }
        this->hapticPattern();
    }
    // Bump walker right
    else if (S1_dist < SONAR_SIDE_SAFEZONE && !(S4_dist < SONAR_SIDE_SAFEZONE))
    {
        this->pWalker->pWheelR->startWheel(MIN_SPEED + this->pWalker->curOffset, 'F');
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed + TURN_BOOST, 'F');
        pulseHaptic(2, 'L');
    }
    // Bump walker left
    else if (S4_dist < SONAR_SIDE_SAFEZONE && !(S1_dist < SONAR_SIDE_SAFEZONE))
    {
        this->pWalker->pWheelL->startWheel(MIN_SPEED, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset + TURN_BOOST, 'F');
        pulseHaptic(2, 'R');
    }
    // Check Road
    else if (this->pEnvironment->road)
    {
        this->emergencyStop();
        pulseHaptic(3, 'R');
        pulseHaptic(3, 'L');
        pulseHaptic(3, 'R');
        pulseHaptic(3, 'L');
        pulseHaptic(3, 'R');
        pulseHaptic(3, 'L');
    }
    // Check crowd
    else if (this->pEnvironment->crowd)
    {
        this->pWalker->pWheelL->startWheel(this->pWalker->curSpeed - CROWD_THROTTLE_VALUE, 'F');
        this->pWalker->pWheelR->startWheel(this->pWalker->curSpeed + this->pWalker->curOffset - CROWD_THROTTLE_VALUE, 'F');
        pulseHaptic(1, 'L');
        pulseHaptic(1, 'R');
        pulseHaptic(1, 'L');
        pulseHaptic(1, 'R');
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

    if (potVal > 2100)
    {
        this->pWalker->curSpeed = 0;
        this->pWalker->curOffset = 0;
    }
    else if (potVal > 600)
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

void Navigation::hapticPattern()
{
    int i;
    int distanceL = this->pWalker->pS2->distance / 10;
    int distanceR = this->pWalker->pS3->distance / 10;

    if (distanceL < distanceR)
    {
        for (i = 0; i < 5 - distanceL; i++)
        {
            pulseHaptic(3, 'L');
        }
    }

    else if (distanceR < distanceL)
    {
        for (i = 0; i < 5 - distanceR; i++)
        {
            pulseHaptic(3, 'R');
        }
    }

    else
    {
        for (i = 0; i > distanceL; i++)
        {
            pulseHaptic(3, 'L');
            pulseHaptic(3, 'R');
        }
    }
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
