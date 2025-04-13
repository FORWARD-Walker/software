#ifndef CONSTANTS_H
#define CONSTANTS_H

// Object Avoidance Constants
#define CAR_HALFWIDTH 1000     // mm
#define PERSON_HALFWIDTH 200   // mm
#define ROLLATOR_HALFWIDTH 250 // mm
#define ROLLATOR_WIDTH 500     // mm

#define FREQ_UPDATE_DATA 30    // Hz

#define IMU_I2C_ADDR 0x29 // IMU I2C addr
#define IMU_DEV_ID 55

#define PHOTORESISTOR_RES 12

// Speed Constants
#define SPEED_1 325
#define SPEED_2 350
#define SPEED_1_RIGHT_WHEEL_OFFSET 20
#define SPEED_2_RIGHT_WHEEL_OFFSET 30

// Camera Constants
#define FOCAL_LENGTH 22       // mm
#define PRINCIPAL_POINT_X 960 // pixel
#define PRINCIPAL_POINT_Y 540 // pixel
#define CAM_RESOLUTION_X 1920 // pixel
#define CAM_RESOLUTION_Y 1080 // pixel

// Artificial Potential Field, Downsampled
#define APF_WIDTH = 192; // Example width
#define APF_HEIGHT = 108; // Example height

#endif