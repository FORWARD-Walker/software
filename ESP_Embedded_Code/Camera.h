#ifndef CAMERA_H
#define CAMERA_H

#include <array>

const int FOCAL_LENGTH = 22; // mm
const int PLATFORM_WIDTH = 500; // mm
const int PRINCIPAL_POINT_X = 960; // pixel
const int PRINCIPAL_POINT_Y = 540; // pixel
//Constants();
std::array<double, 3> pixel2los(double i, double j);

#endif