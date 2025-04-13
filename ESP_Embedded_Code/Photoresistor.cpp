#include <Pins.h>
#include "Photoresistor.h"

Photoresistor::Photoresistor(int photoresPin, int resolution)
{
    this->photoresPin = photoresPin;
    this->resolution = resolution;
    this->setup();
    this->value = 0;
}

void Photoresistor::setup()
{
    pinMode(this->photoresPin, INPUT);
}

void Photoresistor::readValue()
{
    analogReadResolution(this->resolution);
    this->value = analogRead(this->photoresPin);
}