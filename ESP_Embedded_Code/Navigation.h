#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "Walker.h"
#include "Enviroment.h"
#include "Networking.h"
#include <Arduino.h>

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
    Navigation(Walker *pWalker, Networking *pNetworking, Enviroment *pEnviroment); // Constructor
    void Sample_Sonar_Avoidance();
    void navigate();
    void setSpeed();
    void saveNewFrame();

    Walker *pWalker;         // Walker Pointer
    Networking *pNetworking; // Network pointer
    Enviroment *pEnviroment; // Enviroment Pointer

    Frame frames[5]; // 5 Frames of enviroment
};

#endif
