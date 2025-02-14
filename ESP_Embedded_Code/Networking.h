#ifndef Networking_H
#define Networking_H

#include <WiFiUdp.h>
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Networking Class
class Networking
{
private:
    WiFiUDP udp; // UDP object
    WebServer server;
    String dataBuffer;

public:
    Networking();                                   // Clas constructor
    void setup();                                   // Setup function
    void getUDPPacket(char *data, size_t dataSize); // Read UDP
    void pushSerialData(String data);
    void handleRoot();
    void update();

};

#endif