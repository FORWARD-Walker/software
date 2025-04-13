#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include <Arduino.h>

class Photoresistor {
private:
    int photoresPin; // Pin connected to the photoresistor
    int resolution;  // ADC resolution
    int value;       // Current value read from the photoresistor

    void setup();    // Private method to initialize the photoresistor

public:
    // Constructor
    Photoresistor(int photoresPin, int resolution);

    // Method to read the current value from the photoresistor
    void readValue();

    // Method to retrieve the last read value
    int getValue() const { return value; }
};

#endif