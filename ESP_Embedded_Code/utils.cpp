#include <iostream>
#include <cmath>
#include "Constants.h"

// Utilities Class for simple mathematical operations
class utils
{
public:
  static int distToObstacle(int pixel_location, char xory)
  {
    if (xory == 'X')
      return (pixel_location - PRINCIPAL_POINT_X);
    else
      return (pixel_location - PRINCIPAL_POINT_Y);
  }

  // Calculate detection box area
  static double calculateArea(float y2, float y1, float x2, float x1)
  {
    return (y2 - y1) * (x2 - x1); // pixels
  }

  static int obstacleCentroid(int cor1, int cor2)
  {
    return {(cor2 + cor1) / 2};
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

  // The way this works is that in navigation class, we will call this function to get the repulsion vector and then subtract
  // it from the forward vector to get the new direction vector. then in walker class, we will call the steer function to get the new speed commands for the motors.
  double calculateRepulsion(int cor1, int cor2, char xory)
  {
    // for amount of obstacles
    int pixel_location = utils::obstacleCentroid(cor1, cor2);
    int range = utils::distToObstacle(pixel_location, xory);

    double repulsion = 0.0;
    if (range > 0)
    {                                  // Avoid division by zero
      repulsion = K_REPULSION / range; // Larger repulsion as distance decreases
    }

    return repulsion;
  }
};