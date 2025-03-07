#include <iostream>

class utils
{
    public:
    // Calculate unit vector to obstacle
    void calculateLOS(){

    }

    // Calculate detection box area
    double calculateArea(float y2, float y1, float x2, float x1){
        return (y2 - y1) * (x2 - x1); // pixels
    }

    // Linear model for range given object type and detection area
    double estimateDepth(float area, char objectType)
        if(objectType=='person'){
            if (area>=0 && area<= zone1){
                return 
            }
            elseif (area<= zone1)
        }
        elseif(objectType=='car' || objectType=='truck'){
            if (area)
        }
};