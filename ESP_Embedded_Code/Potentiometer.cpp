#include <Pins.h>
#include "Potentiometer.h"

void Potentiometer::Potentiometer(int potPin)
{
    this->potPin = potPin
    this->setup();
    this->value = 0;
}

void Potentiometer::setup()
{
    pinMode(this->potPin, INPUT);
}

void Potentiometer::readValue()
{
    this->value = analogRead(this->potPin);
}