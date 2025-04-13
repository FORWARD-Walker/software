#include <iostream>
#include <cmath>
#include "Constants.h"

// Utilities Class for simple mathematical operations
class utils
{
public:
  static double distToObstacleX(double pixel_x_location)
  {
    return (pixel_x_location - PRINCIPAL_POINT_X);
  }

  static double distToObstacleY(double pixel_y_location)
  {
    return (pixel_y_location - PRINCIPAL_POINT_Y);
  }

  // Calculate detection box area
  static double calculateArea(float y2, float y1, float x2, float x1)
  {
    return (y2 - y1) * (x2 - x1); // pixels
  }

  static double obstacleCentroidX(float x2, float x1)
  {
    return {(x2 + x1) / 2.0};
  }

  static double obstacleCentroidY(float y2, float y1)
  {
    return {(y2 + y1) / 2.0};
  }

  // Convert pixel coordinates to line-of-sight (LOS) vector
  void pixel2los(double i, double j, double rLOScam[3])
  {
    // Normalized camera coordinates
    double camXnorm = (i - PRINCIPAL_POINT_X) / FOCAL_LENGTH;
    double camYnorm = (j - PRINCIPAL_POINT_Y) / FOCAL_LENGTH;

    // Form unnormalized LOS vector
    rLOScam[0] = camXnorm;
    rLOScam[1] = camYnorm;
    rLOScam[2] = 1.0;

    // Normalize
    double magnitude = std::sqrt(
        rLOScam[0] * rLOScam[0] +
        rLOScam[1] * rLOScam[1] +
        rLOScam[2] * rLOScam[2]);

    rLOScam[0] /= magnitude;
    rLOScam[1] /= magnitude;
    rLOScam[2] /= magnitude;
  }
};