# FORWARD Software

<img src="https://raw.githubusercontent.com/FORWARD-Walker/website-fa24sp25g25/main/images/FORWARD_logo.png" alt="FORWARD Logo" width="200"/>

---

## Repository Structure

### `/ESP_Embedded_Code`  
Code to flash the ESP32 chip and run the full FORWARD system.

#### Modeled Peripherals:
- Sonar Sensors  
- Haptic Motors  
- Wheel Brush Motors  
- Potentiometer  
- IMU

#### Core Features:
- Environment modeling  
- Utility functions  
- Networking protocols

> This code base follows object-oriented programming principles.

---

### `/AMB82_Cam_Code`  
Firmware for the Ameba Camera Board to enable object detection and communication on the FORWARD network.

---

### `/Cam_Network_Stream`  
Firmware for the Ameba Camera Board to stream object detection video over a local network.
