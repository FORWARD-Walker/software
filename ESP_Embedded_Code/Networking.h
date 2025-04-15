#ifndef Networking_H
#define Networking_H

#include <WiFiUdp.h>
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define CAM_IP_ADDR "192.168.4.107"
#define PORT 12345

// Networking Class
class Networking
{
private:
    WiFiUDP udp; // UDP object
    WebServer server;
    String dataBuffer;
    IPAddress lastCameraIP; // Store last camera's IP address for sending data

public:
    Networking();                                   // Clas constructor
    void setup();                                   // Setup function
    void getUDPPacket(char *data, size_t dataSize); // Read UDP
    void pushSerialData(String data);
    void clearSerialData();
    void handleRoot();
    void update();
};

#endif