#ifndef SONAR_H
#define SONAR_H

#include <Arduino.h>

// Sonar Class
class Sonar
{
private:
    int trigPin;              // trigger pin
    int echoPin;              // echo pin
    long readSonarDuration(); // Read sonar duration function

public:
    Sonar(int trigPin, int echoPin); // Clas constructor
    void setup();                    // Setup function
    void readDistance();             // Get reading function
    long distance;                   // distance in cm
    String printPins();                // Print to serial the current pinouts
};

#endif