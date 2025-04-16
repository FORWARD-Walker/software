#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

// Object Avoidance Constants
#define CAR_HALFWIDTH 1000     // mm
#define PERSON_HALFWIDTH 200   // mm
#define ROLLATOR_HALFWIDTH 250 // mm
#define ROLLATOR_WIDTH 500     // mm

#define FREQ_UPDATE_DATA 30 // Hz

#define IMU_I2C_ADDR 0x29 // IMU I2C addr
#define IMU_DEV_ID 55

#define PHOTORESISTOR_RES 12

// Speed Constants
#define MIN_SPEED 300
#define SPEED_1 315
#define SPEED_2 330
#define TURN_BOOST 30
#define SPEED_1_RIGHT_WHEEL_OFFSET 20
#define SPEED_2_RIGHT_WHEEL_OFFSET 25
#define CROWD_THROTTLE_VALUE 14

// Navigation Constants
#define SONAR_FRONT_SAFEZONE 50
#define SONAR_SIDE_SAFEZONE 30
#define SONAR_NAVIGATION_ZONE 140
#define SONAR_NAV_ZONE_1 140
#define SONAR_NAV_ZONE_1_OFFSET 20
#define SONAR_NAV_ZONE_2 110
#define SONAR_NAV_ZONE_2_OFFSET 30
#define SONAR_NAV_ZONE_3 80
#define SONAR_NAV_ZONE_3_OFFSET 40

// Camera Constants
#define FOCAL_LENGTH 22       // mm
#define PRINCIPAL_POINT_X 960 // pixel
#define PRINCIPAL_POINT_Y 1   // pixel
#define CAM_RESOLUTION_X 1920 // pixel
#define CAM_RESOLUTION_Y 1080 // pixel

// Artificial Potential Field, Downsampled
#define K_REPULSION 2.0

struct Vector2D
{
    double x, y;

    Vector2D() : x(0.0), y(0.0) {} // default constructor
    Vector2D(double _x, double _y) : x(_x), y(_y) {}

    Vector2D normalize() const
    {
        double magnitude = std::sqrt(x * x + y * y);
        return (magnitude > 0) ? Vector2D(x / magnitude, y / magnitude) : Vector2D(0, 0);
    }
};

struct Vector3D
{
    double x, y, z;

    Vector3D() : x(0.0), y(0.0), z(0.0) {} // default constructor
    Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    Vector3D normalize() const
    {
        double magnitude = std::sqrt(x * x + y * y + z * z);
        return (magnitude > 0) ? Vector3D(x / magnitude, y / magnitude, z / magnitude) : Vector3D(0, 0, 0);
    }
};

#endif