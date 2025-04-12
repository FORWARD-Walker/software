#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino.h>
#include <Constants.h>

class Light
{
public:
    void setup();
    void control(int threshold, int sensorValue);
};

#endif