#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "Walker.h"
#include "Environment.h"
#include "Networking.h"
#include <Arduino.h>
#include "utils.cpp"
#include "Constants.h"

struct Frame
{
    std::vector<int> xPPs;
    std::vector<int> yPPs;
    std::vector<String> object_names;
};

// Walker structure
class Navigation
{

private:
    void pivot(float aspect, char direction);
    void veer(float aspect, char direction);
    void emergencyStop();
    void pulseHaptic(int urgency, char direction);

public:
    Navigation(Walker *pWalker, Networking *pNetworking, Environment *pEnvironment); // Constructor
    void Sample_Sonar_Avoidance();
    void navigate();
    int setSpeed();
    void steer(Vector2D direction_vector, int speed);
    void saveNewFrame();
    void tiltWarning();

    Walker *pWalker;           // Walker Pointer
    Networking *pNetworking;   // Network pointer
    Environment *pEnvironment; // Environment Pointer

    Frame frames[5]; // 5 Frames of Environment
};

#endif
