#include "Imu.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h> // I2C

#define IMU_I2C_ADDR 0x29 // IMU I2C addr
#define IMU_DEV_ID 55

// Create object
Imu::Imu()
{
    this->bno = Adafruit_BNO055(IMU_DEV_ID, IMU_I2C_ADDR, &Wire);
    this->setup();
}

// Set up object
void Imu::setup()
{
    if (!bno.begin())
    {
        Serial.println("Failed to initialize BNO055! Check connections.");
        while (1)
        {
        }
    }
    bno.setExtCrystalUse(true);
}

// Simplified function to read Imu distance over I2C
void Imu::updateData()
{
    bno.getEvent(&this->event);        // Get current data
    this->yaw = event.orientation.x;   // Extract yaw
    this->pitch = event.orientation.y; // Extract pitch
    this->roll = event.orientation.z;  // Extract roll

    imu::Vector<3> linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    this->accx = linearAccel.x(); // x accel
    this->accy = linearAccel.y(); // y accel
    this->accz = linearAccel.z(); // z accel

    // velocity is accumulation of acceleration over operation frequency timestep
    this->velx += accx * 1 / 30;
    this->vely += accy * 1 / 30;
    this->velz += accz * 1 / 30;

    // position is accumulation of velocity
    this->posx += velx * 1 / 30;
    this->posy += vely * 1 / 30;
    this->posz += velz * 1 / 30;
}