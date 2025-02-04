#include "Haptic.h"

#define freq 30000
#define pwmChannel 0
#define resolution 8

// Create object
Haptic::Haptic(int motorPin1, int motorPin2, int enPin)
{
    this->motorPin1 = motorPin1;
    this->motorPin2 = motorPin2;
    this->enPin = enPin;
    this->setup();
}

// Set up object
void Haptic::setup()
{
    // Set up haptic motors
    pinMode(this->motorPin1, OUTPUT);
    pinMode(this->motorPin2, OUTPUT);
    pinMode(this->enPin, OUTPUT);

    // configure LEDC PWM
    ledcAttachChannel(this->enPin, freq, resolution, pwmChannel);
}

void Haptic::startHaptic(int level)
{
    // Determine duty cycles
    if (level == 1)
    {
        dutyCycle = 150;
    }
    else if (level == 2)
    {
        dutyCycle = 200;
    }
    else if (level == 3)
    {
        dutyCycle = 255;
    }

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
    delay(50);
}

// Print to serial the current pinouts
void Haptic::printPins()
{
    Serial.print("Motor Pin 1: ");
    Serial.print(this->motorPin1);
    Serial.print(" Motor Pin 2: ");
    Serial.print(this->motorPin2);
    Serial.print(" Enable Pin: ");
    Serial.print(this->enPin);
    Serial.println();
}