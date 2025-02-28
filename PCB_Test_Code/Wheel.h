#ifndef WHEEL_H
#define WHEEL_H

#include <Arduino.h>

// Wheel Class
class Wheel
{
private:
    int motorPinForward;
    int motorPinReverse;

public:
    Wheel(int motorPinForward, int motorPinReverse);
    void setup();                             // Setup function
    void startWheel(int speed, bool forward); // Start the haptic motor
    void stopWheel();                         // Stop the haptic motor
    String printPins();                         // Print pin out

    // State Variables
    bool spinning;
    int direction; // -1 is resverse, 0 it not moving, 1 is forward
    int curSpeed;
};

#endif