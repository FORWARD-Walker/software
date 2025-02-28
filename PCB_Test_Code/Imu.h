#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// IMU Class
class Imu
{
private:
    sensors_event_t event; // Event object
    Adafruit_BNO055 bno;   // sensor object

public:
    Imu();        // Clas constructor
    void setup(); // Setup function
    void updateData();

    float yaw;
    float pitch;
    float roll;
    float accx;
    float accy;
    float accz;
};

#endif