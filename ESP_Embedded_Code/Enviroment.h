#ifndef ENVIROMENT_H
#define ENVIROMENT_H

#include <Arduino.h>
#include "Walker.h"
#include "Networking.h"

class Enviroment
{
private:
    void detectRoad();
    void detectCrowd();
    void detectSafeZoneViolation();

public:
    // Constructor
    Enviroment(Walker *pWalker, Networking *pNetworking);

    // Functions
    void updateEnviroment();
    void postEnviromentData();

    // Attributes
    Walker *pWalker;
    Networking *pNetworking;

    // Flags
    bool safezoneViolation;
    bool crowd;
    bool road;

    // Data
    std::vector<int> xPPs;
    std::vector<int> yPPs;
    std::vector<String> object_names;
};

#endif