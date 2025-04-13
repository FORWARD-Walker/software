#include <iostream>
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

  static std::array<double, 2> obstacleCentroid(float y2, float y1, float x2, float x1) 
  {
      return { (x2 + x1) / 2.0, (y2 + y1) / 2.0 };
  }

  // // Linear model for range given object type and detection area
  // double estimateDepth(float area, char objectType)
  // {
  //   if (objectType == 'person')
  //   {
  //     if (area >= 0 && area <= zone1)
  //     {
  //       return;
  //     }
  //     else if (area <= zone1)
  //     {
  //     }
  //   }
  //   else if (objectType == 'car' || objectType == 'truck')
  //   {
  //     if (area)
  //     {
  //     }
  //   }
  // }
};