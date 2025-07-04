#include <Pins.h>
#include "Constants.h"
#include "Enviroment.h"

Enviroment::Enviroment(Walker *pWalker, Networking *pNetworking)
{
    this->pWalker = pWalker;
    this->pNetworking = pNetworking;
}

void Enviroment::updateEnviroment()
{
    // Update Sonar
    this->pWalker->pS1->updateDistance();
    this->pWalker->pS2->updateDistance();
    this->pWalker->pS3->updateDistance();
    this->pWalker->pS4->updateDistance();

    // Update IMU
    this->pWalker->pIMU->updateData();

    // Update Camera
    this->pWalker->pCamera->update();

    // Run Checks
    detectSafeZoneViolation();
    detectRoad();
    detectCrowd();
}

void Enviroment::detectSafeZoneViolation()
{
    // Set stop conditions
    this->safezoneViolation = false;
    if ((this->pWalker->pS1->distance < SONAR_SIDE_SAFEZONE) ||
        (this->pWalker->pS2->distance < SONAR_FRONT_SAFEZONE) ||
        (this->pWalker->pS3->distance < SONAR_FRONT_SAFEZONE) ||
        (this->pWalker->pS4->distance < SONAR_SIDE_SAFEZONE))
    {
        this->safezoneViolation = true;
    }
}

void Enviroment::detectRoad()
{
    // Road Detection
    this->road = false;
    int ctr = 0; // Counter for matching object names
    for (int i = 0; i < this->pWalker->pCamera->objects.size(); i++)
    {
        String name = this->pWalker->pCamera->objects.at(i).name;
        name.toLowerCase();
        if (name == "car" || name == "truck" || name == "traffic light" || name == "stop sign")
        {
            ctr++;
        }
        if (ctr >= 2)
        {
            this->road = true;
            pNetworking->pushSerialData("ROAD SITUATION DETECTED!!!\n");
            break;
        }
    }
}

void Enviroment::detectCrowd()
{
    // Person Detection
    this->crowd = false;
    int personCount = 0;
    for (int i = 0; i < this->pWalker->pCamera->objects.size(); i++)
    {
        String name = this->pWalker->pCamera->objects[i].name;
        name.toLowerCase();
        if (name == "person")
        {
            personCount++;
        }

        if (personCount > 2)
        {
            this->crowd = true;
            pNetworking->pushSerialData("CROWD SITUATION DETECTED!!!\n");
            break;
        }
    }
}

void Enviroment::postEnviromentData()
{
    String sensorData = "";

    sensorData += "S1: ";
    sensorData += this->pWalker->pS1->distance;
    sensorData += " S2: ";
    sensorData += this->pWalker->pS2->distance;
    sensorData += " S3: ";
    sensorData += this->pWalker->pS3->distance;
    sensorData += " S4: ";
    sensorData += this->pWalker->pS4->distance;
    sensorData += '\n';

    sensorData += "Yaw: ";
    sensorData += this->pWalker->pIMU->yaw;
    sensorData += " Pitch: ";
    sensorData += this->pWalker->pIMU->pitch;
    sensorData += " Roll: ";
    sensorData += this->pWalker->pIMU->roll;
    sensorData += '\n';
    sensorData += "Accx: ";
    sensorData += this->pWalker->pIMU->accx;
    sensorData += " Accy: ";
    sensorData += this->pWalker->pIMU->accy;
    sensorData += " Accz: ";
    sensorData += this->pWalker->pIMU->accz;
    sensorData += '\n';

    sensorData += "Object Count: ";
    sensorData += this->pWalker->pCamera->objCount;
    sensorData += "\n";
    for (int i = 0; i < this->pWalker->pCamera->objCount; i++)
    {
        sensorData += "Object: ";
        sensorData += this->pWalker->pCamera->objects[i].name;
        sensorData += " [x1: ";
        sensorData += this->pWalker->pCamera->objects[i].x1;
        sensorData += " x2: ";
        sensorData += this->pWalker->pCamera->objects[i].x2;
        sensorData += " y1: ";
        sensorData += this->pWalker->pCamera->objects[i].y1;
        sensorData += " y2: ";
        sensorData += this->pWalker->pCamera->objects[i].y2;
        sensorData += "]\n";
    }

    pNetworking->pushSerialData(sensorData);
    pNetworking->update();
}