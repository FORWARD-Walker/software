#include "Lidar.h"
#include <Wire.h> // I2C

#define TFLUNA_I2C_ADDR 0x10

// Create object
Lidar::Lidar()
{
    // currently nothing here eithr
    this->setup();
}

// Set up object
void Lidar::setup()
{

    this->distance = 0;

}

// Simplified function to read LiDAR distance over I2C
void Lidar::readDistance()
{
    Wire.beginTransmission(TFLUNA_I2C_ADDR); // Open data line
    Wire.write(0x00);                        // Request distance data
    Wire.endTransmission();                  // Close data line

    // Request 2 bytes (distance low and high bytes)
    if (Wire.requestFrom(TFLUNA_I2C_ADDR, (uint8_t)2) == 2)
    {
        uint16_t distance = Wire.read(); // Low byte of distance
        distance |= Wire.read() << 8;    // High byte of distance
        this->distance = distance;
    }
    else
    {
        Serial.print("Error reading Lidar"); // Error code for insufficient bytes received
        this->distance = -1;
    }
}
