#include "Wheel.h"

// Create object
Wheel::Wheel(int motorPinForward, int motorPinReverse)
{
    this->motorPinForward = motorPinForward;
    this->motorPinReverse = motorPinReverse;
    this->setup();
    this->spinning = false;
    this->curDirection = 0;
    this->curSpeed = 0;
}

// Set up object
void Wheel::setup()
{
    pinMode(this->motorPinForward, OUTPUT);
    pinMode(this->motorPinReverse, OUTPUT);
}

void Wheel::startWheel(int speed, char direction)
{
    // If we want to go forward
    if (direction == 'F')
    {
        analogWrite(this->motorPinForward, speed);
        analogWrite(this->motorPinReverse, LOW);
        this->curDirection = 1;
    }
    else
    {
        analogWrite(this->motorPinForward, LOW);
        analogWrite(this->motorPinReverse, speed);
        this->curDirection = -1;
    }
    this->spinning = true;
    this->curSpeed = speed;
}

void Wheel::stopWheel()
{
    analogWrite(this->motorPinForward, LOW);
    analogWrite(this->motorPinReverse, LOW);
    this->spinning = false;
    this->curDirection = 0;
    this->curSpeed = 0;
}

// Print to serial the current pinouts
String Wheel::printPins()
{
    return String("Wheel Pin Forward: ") + this->motorPinForward + String(" Wheel Pin Reverse: ") + this->motorPinReverse + "\n";
}