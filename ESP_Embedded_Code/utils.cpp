#include <iostream>

// Utilitys Class
class utils
{
public:
  // Calculate unit vector to obstacle
  static void calculateLOS()
  {
  }

  // Calculate detection box area
  static double calculateArea(float y2, float y1, float x2, float x1)
  {
    return (y2 - y1) * (x2 - x1); // pixels
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