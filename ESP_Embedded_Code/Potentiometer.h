#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <Arduino.h>

class Potentiometer {
private:
    int potPin;  // Pin connected to the potentiometer
    int value;   // Current value read from the potentiometer

    void setup();  // Private method to initialize the potentiometer

public:
    // Constructor
    Potentiometer(int potPin);

    // Method to read the current value from the potentiometer
    void readValue();

    // Method to retrieve the last read value
    int getValue() const { return value; }
};

#endif