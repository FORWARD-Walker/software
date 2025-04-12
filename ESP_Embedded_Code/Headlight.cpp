#include <Pins.h>
#include "Headlight.h"

void Headlight::Headlight(int lightPin)
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
    if (sensorValue < threshold) {
        digitalWrite(LIGHT, HIGH);
    } else {
        digitalWrite(LIGHT, LOW);
    }
}