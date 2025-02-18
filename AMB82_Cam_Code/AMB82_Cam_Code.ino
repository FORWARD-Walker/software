// # includes
#include "WiFi.h"
#include "StreamIO.h"
#include "VideoStream.h"
#include "NNObjectDetection.h"
#include "ObjectClassList.h"
#include "WiFiUdp.h"

// # Defines
#define CHANNEL 0
#define CHANNELNN 3 
#define NNWIDTH  576
#define NNHEIGHT 320

// Global Setup, objects, and vars
VideoSetting config(VIDEO_FHD, 30, VIDEO_H264, 0);
VideoSetting configNN(NNWIDTH, NNHEIGHT, 10, VIDEO_RGB, 0);
NNObjectDetection ObjDet;
StreamIO videoStreamer(1, 1);
StreamIO videoStreamerNN(1, 1);

char ssid[] = "FORWARD_Network";   // your network SSID (name)
char pass[] = "Forward?0525";       // your network password
WiFiUDP udp; // Websocket object
const int localPort = 12345; // Port to listen too

// Set up code
void setup() {
    Serial.begin(115200); // Start serial

    // Attempt to connect to the access point
    Serial.println("Connecting to Access Point...");
    WiFi.begin(ssid, pass);

    // Wait until the ESP32 is connected to the AP
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    // Display local IP address of the client
    Serial.println("Connected to the AP");
    Serial.print("Client IP Address: ");
    Serial.println(WiFi.localIP());

    // UDP client setup
    udp.begin(localPort);
    Serial.print("UDP server started at port: ");
    Serial.println(localPort);

    // Configure camera video channels with video format information
    config.setBitrate(2 * 1024 * 1024);     // Recommend to use 2Mbps for RTSP streaming to prevent network congestion
    Camera.configVideoChannel(CHANNEL, config);
    Camera.configVideoChannel(CHANNELNN, configNN);
    Camera.videoInit();

    // Configure object detection with corresponding video format information
    // Select Neural Network(NN) task and models
    ObjDet.configVideo(configNN);
    ObjDet.modelSelect(OBJECT_DETECTION, DEFAULT_YOLOV4TINY, NA_MODEL, NA_MODEL);
    ObjDet.begin();

    // Configure StreamIO object to stream data from RGB video channel to object detection
    videoStreamerNN.registerInput(Camera.getStream(CHANNELNN));
    videoStreamerNN.setStackSize();
    videoStreamerNN.setTaskPriority();
    videoStreamerNN.registerOutput(ObjDet);
    if (videoStreamerNN.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Start video channel for NN
    Camera.channelBegin(CHANNELNN);

}

void loop() {

  // Wait for flag to be asserted (1 byte)
  uint8_t txFG = 0;
  while(txFG != 1){ udp.read(&txFG, 1); }
  
  // Get results
  std::vector<ObjectDetectionResult> results = ObjDet.getResult();

  uint16_t im_h = config.height(); //1080
  uint16_t im_w = config.width(); //1920

  // Convert the result count to string and send it
  char resultStr[1024];
  sprintf(resultStr, "# of Objects: %d\n", ObjDet.getResultCount());

  if (ObjDet.getResultCount() > 0) {
      for (uint32_t i = 0; i < ObjDet.getResultCount(); i++) {
          int obj_type = results[i].type();
          if (itemList[obj_type].filter) {    // check if item should NOT be ignored

              ObjectDetectionResult item = results[i];
              // Result coordinates are floats ranging from 0.00 to 1.00
              // Multiply with RTSP resolution to get coordinates in pixels
              int xmin = (int)(item.xMin() * im_w);
              int xmax = (int)(item.xMax() * im_w);
              int ymin = (int)(item.yMin() * im_h);
              int ymax = (int)(item.yMax() * im_h);

              // Prepare the message string
              char msg[64]; // Adjust size as needed
              sprintf(msg, "Object: %s, %d, %d, %d, %d\n", item.name(), xmin, xmax, ymin, ymax);
              strcat(resultStr, msg); // Send the formatted results
          }
      }
  }

  // Send results packet
  strcat(resultStr, "\n");
  writeMsg(resultStr);
}

/////////////// Functions ///////////////
void writeMsg(char* msg)
{
  udp.beginPacket("192.168.4.1", localPort); // IP of destination device
  udp.write((const char*)msg);
  udp.endPacket();
}