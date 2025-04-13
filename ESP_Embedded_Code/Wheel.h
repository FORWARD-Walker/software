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
    void setup();                               // Setup function
    void startWheel(int speed, char direction); // Start the haptic motor
    void stopWheel();                           // Stop the haptic motor
    String printPins();                         // Print pin out
    
    // State Variables
    bool spinning;
    int curDirection; // -1 is reverse, 0 it not moving, 1 is forward
    int curSpeed;
};

#endif