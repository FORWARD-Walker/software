#include "Sonar.h"

// Create object
Sonar::Sonar(int trigPin, int echoPin)
{
    this->trigPin = trigPin;
    this->echoPin = echoPin;
    this->setup();
}

// Set up object
void Sonar::setup()
{
    pinMode(this->trigPin, OUTPUT);
    pinMode(this->echoPin, INPUT);
}

// Get reading
long Sonar::readDistance()
{
    return (readSonarDuration() * 0.034326 / 2.0);
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
