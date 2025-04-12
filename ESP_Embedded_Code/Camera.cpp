#include "Camera.h"
#include <cmath>

// Camera constructor
Camera::Camera() {
    this->camXnorm = 0;
    this->camYnorm = 0;
    this->magnitude = 0;
    this->setup();
}

// Camera setup placeholder
void Camera::setup() {
    // Any initialization code for the camera can be placed here.
}

// Convert pixel coordinates to line-of-sight (LOS) vector
std::array<double, 3> Camera::pixel2los(double i, double j) {
    // Calculate normalized camera coordinates
    double camXnorm = (i - PRINCIPAL_POINT_X) / FOCAL_LENGTH;
    double camYnorm = (j - PRINCIPAL_POINT_Y) / FOCAL_LENGTH;

    // Form LOS vector
    std::array<double, 3> rLOScam = {camXnorm, camYnorm, 1.0};

    // Magnitude calculation
    double magnitude = std::sqrt(static_cast<double>(
        rLOScam[0] * rLOScam[0] +
        rLOScam[1] * rLOScam[1] +
        rLOScam[2] * rLOScam[2]
    ));

    // Return normalized LOS vector
    return {
        rLOScam[0] / magnitude,
        rLOScam[1] / magnitude,
        rLOScam[2] / magnitude
    };
}

void Camera::scenario() {
    // Placeholder for camera-specific scenarios
}