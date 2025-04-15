#include "Networking.h"

// Create object
Networking::Networking() : server(80)
{
    this->setup();
    this->dataBuffer = "";
}

// Set up object
void Networking::setup()
{
    const char *ssid = "FORWARD_Network";  // Network name
    const char *password = "Forward?0525"; // Network password

    // Initialize access point
    WiFi.softAP(ssid, password);

    // Start the UDP server
    udp.begin(PORT);

    // Set up the HTTP server route(s)
    server.on("/", HTTP_GET, std::bind(&Networking::handleRoot, this));
    server.begin();
}

// Simplified function to read Networking distance over I2C
void Networking::getUDPPacket(char *data, size_t dataSize)
{
    // Assert bit to signal receive a packet
    udp.beginPacket(CAM_IP_ADDR, PORT);
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
            Serial.println(data);
            return; // Exit on successful read
        }
    }
}

// Handle new connection
void Networking::handleRoot()
{
    String html = "<html><head><title>FORWARD Information Console</title></head><body>";
    html += "<h1>FORWARD Notifcations:</h1>";
    html += "<pre>" + this->dataBuffer + "</pre>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

// Push Serial data
void Networking::pushSerialData(String data)
{
    if(this->dataBuffer.length() > 100000) ESP.getFreeHeap();
    this->dataBuffer = data + "\n" + this->dataBuffer; // Append new data with a newline
}

// Clear Serial data
void Networking::clearSerialData()
{
    this->dataBuffer = ""; // Append new data with a newline
}

// Update server
void Networking::update()
{
    server.handleClient();
}
