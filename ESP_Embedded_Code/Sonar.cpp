#include "Sonar.h"

// Create object
Sonar::Sonar(int trigPin, int echoPin)
{
    this->trigPin = trigPin;
    this->echoPin = echoPin;
    this->setup();
    this->distance = 0;
}

// Set up object
void Sonar::setup()
{
    pinMode(this->trigPin, OUTPUT);
    pinMode(this->echoPin, INPUT);
}

// Get reading
void Sonar::readDistance()
{
    this->distance = (readSonarDuration() * 0.034326 / 2.0);
}

// Read sonar duration
long Sonar::readSonarDuration()
{
    // Send out pulse
    digitalWrite(this->trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(this->trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(this->trigPin, LOW);

    // Recieve return data
    return pulseIn(this->echoPin, HIGH);
}

// Print to serial the current pinouts
void Sonar::printPins()
{
    Serial.print("Trigger Pin: ");
    Serial.print(this->trigPin);
    Serial.print(" Echo Pin: ");
    Serial.print(this->echoPin);
    Serial.println();
}
