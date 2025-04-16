#include <iostream>
#include <cmath>
#include "Constants.h"

// Utilities Class for simple mathematical operations
class utils
{
public:
  // Magnitude of a 2D vector
  double mag2d(Vector2D vec)
  {
      return std::sqrt(vec.x * vec.x + vec.y * vec.y);
  }

  // Magnitude of a 3D vector
  double mag3d(Vector3D vec)
  {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
  }

  // Calculate detection box area
  double calculateArea(float y2, float y1, float x2, float x1)
  {
    return (y2 - y1) * (x2 - x1); // unit pixels
  }

  // Calculate center of a detection box along one dimension
  // when calling, repeat for both camera image dimensions
  int obstacleCentroid(int Pix1, int Pix2)
  {
    return {(Pix1 + Pix2) / 2}; // coordinate
  }

  // Convert 2D pixel coordinates to 3D line-of-sight vector
  Vector3D pixel2los(int xPix, int yPix)
  {
      // Normalized camera coordinates
      double camXnorm = (xPix - PRINCIPAL_POINT_X) / FOCAL_LENGTH;
      double camYnorm = (yPix - PRINCIPAL_POINT_Y) / FOCAL_LENGTH;
  
      // LOS vector in camera frame
      Vector3D rLOScam = {camXnorm, camYnorm, 1.0};
  
      return rLOScam;
  }

  // Calculate a repulsion vector. attraction vector = {0,1}
  Vector2D repulsionVector(int xPix, int yPix)
  {
    Vector2D pos = utils::normObstaclePosition(xPix, yPix); // Returns (x_obs, y_obs)
    double distSq = std::pow(mag2d(pos), 2);

    Vector2D repulsion = {0.0, 0.0};

    if (distSq > 1e-6) // Avoid divide by zero
    {
        repulsion.x = -K_REPULSION * pos.x / distSq;
        repulsion.y = -K_REPULSION * pos.y / distSq;
    }
    return repulsion;
  }

  // Normalized obstacle position given coordinate in camera frame of reference
  Vector2D normObstaclePosition(int xPix, int yPix)
  {
    double normX = (xPix - CAM_RESOLUTION_X / 2.0) / (CAM_RESOLUTION_X / 2.0);
    double normY = (yPix - CAM_RESOLUTION_Y / 2.0) / (CAM_RESOLUTION_X / 2.0);

    return Vector2D {normX, normY};
  }

  // GNC direction cosine matrix to transform camera frame to rollator frame
  // yaw in degrees, vec {x,y}
  Vector2D C2R(Vector2D vec, double yaw)
  {
    double cosYaw = std::cos(yaw);
    double sinYaw = std::sin(yaw);

    Vector2D transformedVec = {0.0, 0.0};
    transformedVec.x = vec.x * cosYaw - vec.y * sinYaw;
    transformedVec.y = vec.x * sinYaw + vec.y * cosYaw;

    return transformedVec;
  }

};