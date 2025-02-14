#include "Camera.h"
#include <cmath>

std::array<double, 3> pixel2los(double i, double j){
    // calculate normalized camera coordinates
    double camXnorm = (i - PRINCIPAL_POINT_X)/FOCAL_LENGTH;
    double camYnorm = (j - PRINCIPAL_POINT_Y)/FOCAL_LENGTH;

    // form LOS vector
    std::array<double, 3> rLOScam = {camXnorm, camYnorm, 1.0};

    double magnitude = std::sqrt(rLOScam[0]*rLOScam[0] + rLOScam[1]*rLOScam[1] + rLOScam[2]*rLOScam[2]);

    return {rLOScam[0]/magnitude, rLOScam[1]/magnitude, rLOScam[2]/magnitude};
}