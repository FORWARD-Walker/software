#include "Camera.h"
#include <cmath>

Camera::Camera(Networking *pNetworking)
{
    this->pNetworking = pNetworking;
}

void Camera::update()
{
    char temp[1024];
    pNetworking->getUDPPacket(temp, sizeof(temp));
    String camDataStr = String(temp);
    
    this->objCount = 0;

    int pos = 0;
    int newlineIndex = camDataStr.indexOf('\n', pos);
    if (newlineIndex == -1)
    {
        newlineIndex = camDataStr.length();
    }

    // Parse the first line to extract the object count
    String firstLine = camDataStr.substring(pos, newlineIndex);
    int colonIndex = firstLine.indexOf(':');
    if (colonIndex != -1)
    {
        String countStr = firstLine.substring(colonIndex + 1);
        countStr.trim();
        this->objCount = countStr.toInt();
    }

    // Process remaining lines for each object
    pos = newlineIndex + 1;
    while (pos < camDataStr.length())
    {
        newlineIndex = camDataStr.indexOf('\n', pos);
        if (newlineIndex == -1)
        {
            newlineIndex = camDataStr.length();
        }
        String line = camDataStr.substring(pos, newlineIndex);
        line.trim();

        if (line.startsWith("Object:"))
        {
            // Remove the "Object:" prefix
            String data = line.substring(7);
            data.trim();

            // Parse the object's name and coordinates using commas as delimiters
            int firstComma = data.indexOf(',');
            if (firstComma == -1)
            {
                break;
            }

            Camera_Data_Struct obj;
            obj.name = data.substring(0, firstComma);
            obj.name.trim();

            int posNum = firstComma + 1;
            int comma = data.indexOf(',', posNum);
            if (comma == -1)
                break;
            obj.x1 = data.substring(posNum, comma).toInt();

            posNum = comma + 1;
            comma = data.indexOf(',', posNum);
            if (comma == -1)
                break;
            obj.x2 = data.substring(posNum, comma).toInt();

            posNum = comma + 1;
            comma = data.indexOf(',', posNum);
            if (comma == -1)
                break;
            obj.y1 = data.substring(posNum, comma).toInt();

            posNum = comma + 1;
            obj.y2 = data.substring(posNum).toInt();
        }
        pos = newlineIndex + 1;
    }
}
