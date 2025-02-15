#ifndef Networking_H
#define Networking_H

#include <Arduino.h>
#include <WebServer.h>
#include "WiFiServer.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Networking Class
class Networking
{
private:
    WiFiClient client; // TCP Client
    WiFiServer wifiServer; // TCP Server
    WebServer webServer; // HTTP server
    String dataBuffer;

public:
    Networking();                                   // Clas constructor
    void setup();                                   // Setup function
    void getTCPStream(char* data, size_t dataSize); // Read TCP
    void pushSerialData(String data);
    void handleRoot();
    void update();
};

#endif