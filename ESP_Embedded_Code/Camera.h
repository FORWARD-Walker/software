#ifndef CAMERA_H
#define CAMERA_H

// Includes
#include <array>

// Constant defines
#define FOCAL_LENGTH 22       // mm
#define PRINCIPAL_POINT_X 960 // pixel
#define PRINCIPAL_POINT_Y 540 // pixel

class Camera {
private:
    double camXnorm; // Normalized X-coordinate
    double camYnorm; // Normalized Y-coordinate
    double magnitude; // Magnitude of LOS vector

public:
    Camera(); // Constructor

    void setup(); // Method to initialize camera settings

    // Convert pixel coordinates to line-of-sight (LOS) vector
    std::array<double, 3> pixel2los(double i, double j);

    void scenario(); // Placeholder for defining camera scenarios
};

#endif