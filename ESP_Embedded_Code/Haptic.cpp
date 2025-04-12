#include "Haptic.h"

// Create object
Haptic::Haptic(int motorPin1, int motorPin2, int enPin)
{
    this->motorPin1 = motorPin1;
    this->motorPin2 = motorPin2;
    this->enPin = enPin;
    this->setup();
    this->buzzing = false;
}

// Set up object
void Haptic::setup()
{
    // Set up haptic motors
    pinMode(this->motorPin1, OUTPUT);
    pinMode(this->motorPin2, OUTPUT);
    pinMode(this->enPin, OUTPUT);

}

void Haptic::startHaptic()
{
    this->buzzing = true;

    ledcWrite(this->enPin, dutyCycle);

    // while the distance is within a little over a foot, vibrate haptics
    // Move the haptic
    digitalWrite(this->motorPin1, LOW);
    digitalWrite(this->motorPin2, HIGH);
}

void Haptic::stopHaptic()
{
    // Stop the haptic motor
    digitalWrite(this->motorPin1, LOW);
    digitalWrite(this->motorPin2, LOW);
    this->buzzing = false;
}

// Print to serial the current pinouts
String Haptic::printPins()
{
    return String("Motor Pin 1: ") + this->motorPin1 + String(" Motor Pin 2: ") + this->motorPin2 + String(" Enable Pin: ") + this->enPin + "\n";
}