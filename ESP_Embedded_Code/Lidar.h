#ifndef LIDAR_H
#define LIDAR_H

#include <Arduino.h>

// Sonar Class
class Lidar
{
private:
public:
    Lidar();      // Clas constructor
    void setup(); // Setup function
    uint16_t readDistance();
};

#endif