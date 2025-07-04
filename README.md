# FORWARD Software
<img src="https://raw.githubusercontent.com/FORWARD-Walker/website-fa24sp25g25/main/images/FORWARD_logo.png" alt="FORWARD Logo" width="200"/>

The following repository file structure is:
/ESP_Embedded_Code   
/AMB82_Cam_Code  
/Cam_Network_Stream  

ESP_Embedded_Code: Code needed to flash the ESP32 chip for running the full system

  The modeled peripherals are:
  - Sonar Sensors  
  - Haptic Motors  
  - Wheel Brush Motors  
  - Potentiometer  
  - IMU  
  
  The code also contains all needed models and algorithms for:
  - FORWARD environment  
  - Utilities  
  - Networking  
  
  This code base implements many principles of object-oriented programming.

AMB82_Cam_Code: Code needed to flash the Ameba Camera Board for object detection and transmitting on FORWARD network

Cam_Network_Stream: Code needed to flash the Ameba Camera Board for object detection live stream over local network
