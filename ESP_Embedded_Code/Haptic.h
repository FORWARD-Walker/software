#ifndef HAPTIC_H
#define HAPTIC_H

#include <Arduino.h>

// Haptic Class
class Haptic
{
private:
    int motorPin1;
    int motorPin2;
    int enPin;
    int dutyCycle;

public:
    Haptic(int motorPin1, int motorPin2, int enPin);
    void setup(); // Setup function
    void startHaptic(int level);
    void stopHaptic();
};

#endif