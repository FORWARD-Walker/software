#ifndef HEADLIGHT_H
#define HEADLIGHT_H

#include <Arduino.h>
#include <Constants.h>

class Headlight
{
public:
    Headlight(int lightPin);
    void setup();
    void control(int threshold, int sensorValue);

    int lightPin;
};

#endif