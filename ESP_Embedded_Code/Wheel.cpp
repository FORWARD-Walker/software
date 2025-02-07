#include "Wheel.h"

// Create object
Wheel::Wheel(int motorPinForward, int motorPinReverse)
{
    this->motorPinForward = motorPinForward;
    this->motorPinReverse = motorPinReverse;
    this->setup();
    this->spinning = false;
    this->direction = 0;
    this->curSpeed = 0;
}

// Set up object
void Wheel::setup()
{
    pinMode(this->motorPinForward, OUTPUT);
    pinMode(this->motorPinReverse, OUTPUT);
}

void Wheel::startWheel(int speed, bool forward)
{
    // If we want to go forward
    if (forward)
    {
        analogWrite(this->motorPinForward, speed);
        analogWrite(this->motorPinReverse, LOW);
        this->direction = 1;
    }
    else
    {
        analogWrite(this->motorPinForward, LOW);
        analogWrite(this->motorPinReverse, speed);
        this->direction = -1;
    }
    this->spinning = true;
    this->curSpeed = speed;
}

void Wheel::stopWheel()
{
    analogWrite(this->motorPinForward, LOW);
    analogWrite(this->motorPinReverse, LOW);
    this->spinning = false;
    this->direction = 0;
    this->curSpeed = 0;
}

// Print to serial the current pinouts
void Wheel::printPins()
{
    Serial.print("Wheel Pin Forward: ");
    Serial.print(this->motorPinForward);
    Serial.print("Wheel Pin Reverse: ");
    Serial.print(this->motorPinReverse);
    Serial.println();
}