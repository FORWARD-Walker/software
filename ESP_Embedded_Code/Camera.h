#ifndef CAMERA_H
#define CAMERA_H

// Includes
#include <array>
#include "Constants.h"

// Constant defines
#define FOCAL_LENGTH 22       // mm
#define PRINCIPAL_POINT_X 960 // pixel
#define PRINCIPAL_POINT_Y 540 // pixel

// Constants();
std::array<double, 3> pixel2los(double i, double j);

#endif