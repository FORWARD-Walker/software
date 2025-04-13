#ifndef FIELD_H
#define FIELD_H

#include "Constants.h"
#include <vector>
#include <array>

class Field {
private:
    int k_repulsion; // Repulsion scaling factor
    int repulsion_x;
    int repulsion_y;

    // Fixed APF matrix storing forward vectors
    std::array<std::array<std::array<double, 2>, APF_WIDTH>, APF_HEIGHT> apf;

public:
    Field(); // Constructor

    void setup(); // Initializes the pixel map and APF matrix

    std::array<double, 2> calculateRepulsion(); // Calculates repulsion vector from obstacles

    std::array<double, 3> pixel2los(int x, int y); // Converts pixel coordinates to LOS vector
};

#endif