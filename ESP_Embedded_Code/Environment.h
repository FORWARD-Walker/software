#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Arduino.h>
#include "Walker.h"
#include "Networking.h"
#include "Constants.h"

class Environment
{
private:
    void detectRoad();
    void detectCrowd();
    void detectSonarPath();
    void updateFrame();

public:
    // Constructor
    Environment(Walker *pWalker, Networking *pNetworking);

    // Functions
    void updateEnvironment();
    void postEnvironmentData();

    // Attributes
    Walker *pWalker;
    Networking *pNetworking;

    // Flags
    bool S1Trig;
    bool S2Trig;
    bool S3Trig;
    bool S4Trig;
    bool crowd;
    bool road;

    // Data
    std::vector<int> xPPs;
    std::vector<int> yPPs;
    std::vector<String> object_names;
};

#endif