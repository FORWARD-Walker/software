#include "Networking.h"

#define IP_ADDR "192.168.4.2"
#define PORT 12345

// Create object
Networking::Networking()
{
    this->setup();
}

// Set up object
void Networking::setup()
{
    WebServer server(80);

    const char *ssid = "FORWARD_Network";  // Network name
    const char *password = "Forward?0525"; // Network password

    // Initialize access point
    WiFi.softAP(ssid, password);

    // Print AP details
    Serial.println("Access Point Initialized!");
    Serial.printf("AP IP Address: %s\n", WiFi.softAPIP().toString().c_str());

    // Start the UDP server
    udp.begin(PORT);
    Serial.printf("UDP Server listening on port %d\n", PORT);
}

// Simplified function to read Networking distance over I2C
void Networking::getUDPPacket(char *data, size_t dataSize)
{
    // Assert bit to signal receive a packet
    udp.beginPacket(IP_ADDR, PORT);
    uint8_t rxFG = 1;
    udp.write(rxFG);
    udp.endPacket();

    // Wait for the packet
    delay(3);
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
        int len = udp.read(data, dataSize - 1);
        if (len > 0)
        {
            data[len] = '\0';
            return; // Exit on successful read
        }
    }
}
