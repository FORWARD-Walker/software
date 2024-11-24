#ifndef Networking_H
#define Networking_H

#include <WiFiUdp.h>
#include <Arduino.h>

// Sonar Class
class Networking
{
private:
    WiFiUDP udp;      // UDP object

public:
    Networking();         // Clas constructor
    void setup();         // Setup function
    void getUDPPacket(char* data, size_t dataSize); // Read UDP
};

#endif