#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "Walker.h"
#include <Arduino.h>

// Walker structure
class Navigation
{

private:
    void pivot(float aspect, char direction);
    void veer(float aspect, char direction);

public:
    Navigation(Walker *pWalker); // Constructor
    Walker *pWalker;
    void Sample_Sonar_Avoidance();
    void emergencyStop();
    void pulseHaptic(int urgency, char direction);
    void tiltWarning();
};

#endif
