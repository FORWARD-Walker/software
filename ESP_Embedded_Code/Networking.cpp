#include "Networking.h"

#define CAM_IP_ADDR "192.168.4.2"
#define TCP_PORT 12346
#define WEB_PORT 80


// Create object
Networking::Networking() : webServer(WEB_PORT), wifiServer(TCP_PORT)
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
    wifiServer.begin();

    // Set up the HTTP server route(s)
    webServer.on("/", HTTP_GET, std::bind(&Networking::handleRoot, this));
    webServer.begin();
}

// Get TCP Data
void Networking::getTCPStream(char* data, size_t dataSize)
{
    client = wifiServer.available();
    if (client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                String receivedData = client.readStringUntil('\0');
                receivedData.toCharArray(data, dataSize);
            }
        }
        client.stop();
    }
}

// Handle new connection
void Networking::handleRoot()
{
    String html = "<html><head><title>ESP32 Data</title></head><body>";
    html += "<h1>Serial Data:</h1>";
    html += "<pre>" + this->dataBuffer + "</pre>";
    html += "</body></html>";
    webServer.send(200, "text/html", html);
}

// Push Serial data
void Networking::pushSerialData(String data)
{
    this->dataBuffer = data + "\n" + this->dataBuffer; // Append new data with a newline
}

// Update server
void Networking::update()
{
    webServer.handleClient();
}
