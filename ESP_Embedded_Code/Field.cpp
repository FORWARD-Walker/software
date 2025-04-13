#include "Field.h"
#include "Constants.h"
#include <cmath>
#include "utils.cpp"

Field::Field()
{
    this->k_repulsion = 1.0;
    this->repulsion_x = 0;
    this->repulsion_y = 0;
}

void Field::setup()
{
    // Get from the camera;
    //  obstacle positions
    //  amount of obstacles
    //  type of obstacles
}


// The way this works is that in navigation class, we will call this function to get the repulsion vector and then subtract
// it from the forward vector to get the new direction vector. then in walker class, we will call the steer function to get the new speed commands for the motors.
std::array<double, 2> Field::calculateRepulsion()
{
    // for amount of obstacles 
        obstacle.pixel_location = utils::obstacleCentroid(obstacle.y2, obstacle.y1, obstacle.x2, obstacle.x1);
        obstacle.rangeX = utils::distToObstacleX(obstacle.pixel_x_location);
        obstacle.rangeY = utils::distToObstacleY(obstacle.pixel_y_location);
        

        if (obstacle.range > 0)
        { // Avoid division by zero
            repulsion.x = this->k_repulsion / obstacle.rangeX; // Larger repulsion as distance decreases
            repulsion.y = this->k_repulsion / obstacle.rangeY; // Larger repulsion as distance decreases
        }
        
 
    std::array<double, 2> repulsion_vector = {this->repulsion_x, this->repulsion_y};
    return repulsion_vector;
}