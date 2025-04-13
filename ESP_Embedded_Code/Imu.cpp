#include "Imu.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h> // I2C
#include <Constants.h>

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

    // Initialize position history and reference frame
    for (int i=0; i<3; i++)
    {
        origin[i] = 0.0;
        currPos[i] = 0.0;
        prevPos1[i] = 0.0;
        prevPos2[i] = 0.0;
    }
}

// Simplified function to read Imu distance over I2C
void Imu::updateData()
{
    bno.getEvent(&this->event);        // Get current data 9DOF
    this->yaw = event.orientation.x;   // Extract yaw
    this->pitch = event.orientation.y; // Extract pitch
    this->roll = event.orientation.z;  // Extract roll

    imu::Vector<3> linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    this->accx = linearAccel.x(); // x accel
    this->accy = linearAccel.y(); // y accel
    this->accz = linearAccel.z(); // z accel

    // velocity is accumulation of acceleration over operation frequency timestep
    this->velx += accx * 1 / FREQ_UPDATE_DATA;
    this->vely += accy * 1 / FREQ_UPDATE_DATA;
    this->velz += accz * 1 / FREQ_UPDATE_DATA;

    // position is accumulation of velocity
    this->posx += velx * 1 / FREQ_UPDATE_DATA;
    this->posy += vely * 1 / FREQ_UPDATE_DATA;
    this->posz += velz * 1 / FREQ_UPDATE_DATA;

    // Update position history
    for (int i=0; i<3; i++)
    {
        currPos[i] = { posx, posy, posz };
        prevPos1[i] = currPos[i];
        prevPos2[i] = prevPos[i];
    }
}