#include <Pins.h>
#include "Headlight.h"

Headlight::Headlight(int lightPin)
{
    this->lightPin = lightPin;
    this->setup();
}

void Headlight::setup()
{
    pinMode(this->lightPin, OUTPUT);
}

void Headlight::control(int threshold, int sensorValue)
{
    if (sensorValue < threshold)
    {
        digitalWrite(HEADLIGHT, HIGH);
    }
    else
    {
        digitalWrite(HEADLIGHT, LOW);
    }
}