#ifndef HEADLIGHT_H
#define HEADLIGHT_H

#include <Arduino.h>
#include <Constants.h>

class Headlight
{
public:
    void setup();
    void control(int threshold, int sensorValue);
};

#endif