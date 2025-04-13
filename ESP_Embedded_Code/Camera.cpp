#include "Camera.h"
#include <cmath>

Camera::Camera(Networking *pNetworking)
{
    this->pNetworking = pNetworking;
    this->objCount = 0;
}

void Camera::update()
{
    char temp[1024];
    this->pNetworking->getUDPPacket(temp, sizeof(temp));
    String camDataStr = String(temp);

    // Clear old data
    this->objects.clear();

    int pos = 0;
    int newlineIndex = camDataStr.indexOf('\n', pos);
    if (newlineIndex == -1)
    {
        newlineIndex = camDataStr.length();
    }

    // Parse the first line to extract the object count
    String firstLine = camDataStr.substring(pos, newlineIndex);
    int colonIndex = firstLine.indexOf(':');
    int expectedCount = 0;

    if (colonIndex != -1)
    {
        String countStr = firstLine.substring(colonIndex + 1);
        countStr.trim();
        expectedCount = countStr.toInt();
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
            String data = line.substring(7);
            data.trim();

            int firstComma = data.indexOf(',');
            if (firstComma == -1)
                break;

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

            // Store the parsed object
            this->objects.push_back(obj);
        }

        pos = newlineIndex + 1;
    }

    // Update actual object count
    this->objCount = this->objects.size();
}
