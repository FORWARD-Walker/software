#ifndef CAMERA_H
#define CAMERA_H

// Includes
#include <vector> // Use vector, not array
#include "Constants.h"
#include "Networking.h"

// Defines the camera data of interest
struct Camera_Data_Struct
{
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
    String name;
};

class Camera
{
public:
    // Constructor
    Camera(Networking *pNetworking);

    // Public Methods
    void update();
    Camera_Data_Struct parseCameraData(String &input);

    // Attributes
    Networking *pNetworking;
    int objCount;
    std::vector<Camera_Data_Struct> objects; // camData is a dynamic list of object data
};

#endif
