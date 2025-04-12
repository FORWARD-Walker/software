#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <Arduino.h>

class Potentiometer
{
private:
    int potPin;   // Pin connected to the potentiometer
    void setup(); // Private method to initialize the potentiometer

public:
    // Constructor
    Potentiometer(int potPin);
    int value;
    void readValue();
};

#endif