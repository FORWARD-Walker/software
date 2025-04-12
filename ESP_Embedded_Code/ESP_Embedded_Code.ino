// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code serves to drive all process on the ESP32 MCU for our project
// Features include: WiFi connectivity, data transmission over wifi, Locally 
// hosted network, sensor data processing, motor controls, basic coding functionality
// Located remotely in https://github.com/FORWARD-Walker/software

// # Include
#include <Wire.h> // I2C
#include "Networking.h"
#include "utils.cpp"
#include "Constants.h"
#include "Navigation.h"
#include "Walker.h"
#include "Pins.h"

struct Camera_Data_Struct
{
  int x1 = 0;
  int x2 = 0;
  int y1 = 0;
  int y2 = 0;
  String name;
};

struct Sensor_Data_Struct
{
  // Sonar Data
  long S1_Distance = 0;
  long S2_Distance = 0;
  long S3_Distance = 0;
  long S4_Distance = 0;

  // IMU Data
  float yaw = 0.0;
  float pitch = 0.0;
  float roll = 0.0; 
  float accx = 0.0;
  float accy = 0.0;
  float accz = 0.0;
  float velx = 0.0;
  float vely = 0.0;
  float velz = 0.0;
  float posx = 0.0;
  float posy = 0.0;
  float posz = 0.0;

  // Camera Data
  int objCount = 0;
  std::vector<Camera_Data_Struct> objects;

} Sensor_Data;

// Walker structure
Walker* pWalker = nullptr;

// Network Ref
Networking* pNetworking = nullptr;

// Navigation Ref
Navigation* pNavigation = nullptr;

// Timer Objects
hw_timer_t *Timer_1Hz = nullptr;
hw_timer_t *Timer_10Hz = nullptr;
hw_timer_t *Timer_30Hz = nullptr;

// Helper Function Prototypes
void Update_Data();
void Send_Sensor_Data();
Sensor_Data_Struct parseCameraData(Sensor_Data_Struct Sensor_Data, String &input);

// ISR prototypes
static void IRAM_ATTR Timer_1Hz_ISR();
static void IRAM_ATTR Timer_10Hz_ISR();
static void IRAM_ATTR Timer_30Hz_ISR();

// Boolean ISR Flags
bool Timer_1HZ_FG = false;
bool Timer_10HZ_FG = false;
bool Timer_30HZ_FG = false;

// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(115200); // Init Serial

  // Set up I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Setup heartbeat
  pinMode(yLED, OUTPUT); // Set up LED as output
  digitalWrite(yLED, HIGH); // Init to high
  pinMode(bLED, OUTPUT); // Set up LED as output
  digitalWrite(bLED, LOW); // Init to high

  // Setup Timers and Interrupts
  // Timer 1 Hz
  Timer_1Hz = timerBegin(1000000);
  timerAttachInterrupt(Timer_1Hz, &Timer_1Hz_ISR);
  timerAlarm(Timer_1Hz, 1000000, true, 0);

  // Timer 10 Hz
  Timer_10Hz = timerBegin(10000000);
  timerAttachInterrupt(Timer_10Hz, &Timer_10Hz_ISR);
  timerAlarm(Timer_10Hz, 1000000, true, 0);  
  
  // Timer 30 Hz
  Timer_30Hz = timerBegin(30000000);
  timerAttachInterrupt(Timer_30Hz, &Timer_30Hz_ISR);
  timerAlarm(Timer_30Hz, 1000000, true, 0);

  // Setup ESP32 network
  pNetworking = new Networking();  // Init object
  pNetworking->pushSerialData("Network Initialized!\n");

  // Init Walker
  pWalker = new Walker();
  pNetworking->pushSerialData("Walker Initialized!\n");

  // Init Navigation
  pNavigation = new Navigation(pWalker);
  pNetworking->pushSerialData("Navigation Initialized!\n");

}

// Main loop
void loop()
{ 

  // 30 HZ ISR
  if(Timer_30HZ_FG)
  {

    Timer_30HZ_FG = false; // Reset ISR
  }

  // 10 HZ ISR
  if(Timer_10HZ_FG)
  {
    // Check Speed
    pNavigation->setSpeed();
    pNetworking->pushSerialData(String(pWalker->pPotentiometer->value));

    // Update Sensor Data
    Update_Data(); 
    
    Timer_10HZ_FG = false; // Reset ISR
  }

  // 1 HZ ISR
  if(Timer_1HZ_FG)
  {
    // Push Serial Data
    Send_Sensor_Data();

    // Blink Heartbeat
    Blink_Heartbeat();

    Timer_1HZ_FG = false;  // Reset ISR
  }
}

// ISRs
static void IRAM_ATTR Timer_1Hz_ISR()
{
  Timer_1HZ_FG = true;
}

static void IRAM_ATTR Timer_10Hz_ISR()
{
  Timer_10HZ_FG = true;
}

static void IRAM_ATTR Timer_30Hz_ISR()
{
  Timer_30HZ_FG = true;
}

void Blink_Heartbeat()
{
    digitalWrite(yLED, digitalRead(yLED)^1); // Flash heartbeat
    digitalWrite(bLED, digitalRead(bLED)^1); // Flash heartbeat
}


// Update Sensor Data
void Update_Data()
{
    // Update Sonar Distances
    pWalker->pS1->updateDistance();
    Sensor_Data.S1_Distance = pWalker->pS1->distance;
    pWalker->pS2->updateDistance();
    Sensor_Data.S2_Distance = pWalker->pS2->distance;
    pWalker->pS3->updateDistance();
    Sensor_Data.S3_Distance = pWalker->pS3->distance;
    pWalker->pS4->updateDistance();
    Sensor_Data.S4_Distance = pWalker->pS4->distance;

    // Update IMU readings
    pWalker->pIMU->updateData();
    Sensor_Data.yaw = pWalker->pIMU->yaw;
    Sensor_Data.pitch = pWalker->pIMU->pitch;
    Sensor_Data.roll = pWalker->pIMU->roll;
    Sensor_Data.accx = pWalker->pIMU->accx;
    Sensor_Data.accy = pWalker->pIMU->accy;
    Sensor_Data.accz = pWalker->pIMU->accz;
    Sensor_Data.velx = pWalker->pIMU->velx;
    Sensor_Data.vely = pWalker->pIMU->vely;
    Sensor_Data.velz = pWalker->pIMU->velz;
    Sensor_Data.posx = pWalker->pIMU->posx;
    Sensor_Data.posy = pWalker->pIMU->posy;
    Sensor_Data.posz = pWalker->pIMU->posz;

    // Update Camera Data
    char temp[1024];
    pNetworking->getUDPPacket(temp, sizeof(temp));
    String camDataStr = String(temp);
    Sensor_Data = parseCameraData(Sensor_Data, camDataStr);
}

Sensor_Data_Struct parseCameraData(Sensor_Data_Struct Sensor_Data, String &input)
{
  // Reset Struct
  Sensor_Data.objects.clear();
  Sensor_Data.objCount = 0;

  int pos = 0;
  int newlineIndex = input.indexOf('\n', pos);
  if (newlineIndex == -1) {
    newlineIndex = input.length();
  }
  
  // Parse the first line to extract the object count
  String firstLine = input.substring(pos, newlineIndex);
  int colonIndex = firstLine.indexOf(':');
  if (colonIndex != -1) {
    String countStr = firstLine.substring(colonIndex + 1);
    countStr.trim();
    Sensor_Data.objCount = countStr.toInt();
  }
  
  // Process remaining lines for each object
  pos = newlineIndex + 1;
  while (pos < input.length()) {
    newlineIndex = input.indexOf('\n', pos);
    if (newlineIndex == -1) {
      newlineIndex = input.length();
    }
    String line = input.substring(pos, newlineIndex);
    line.trim();
    
    if (line.startsWith("Object:")) {
      // Remove the "Object:" prefix
      String data = line.substring(7);
      data.trim();
      
      // Parse the object's name and coordinates using commas as delimiters
      int firstComma = data.indexOf(',');
      if (firstComma == -1) {
        break;
      }
      
      Camera_Data_Struct obj;
      obj.name = data.substring(0, firstComma);
      obj.name.trim();
      
      int posNum = firstComma + 1;
      int comma = data.indexOf(',', posNum);
      if (comma == -1) break;
      obj.x1 = data.substring(posNum, comma).toInt();
      
      posNum = comma + 1;
      comma = data.indexOf(',', posNum);
      if (comma == -1) break;
      obj.x2 = data.substring(posNum, comma).toInt();
      
      posNum = comma + 1;
      comma = data.indexOf(',', posNum);
      if (comma == -1) break;
      obj.y1 = data.substring(posNum, comma).toInt();
      
      posNum = comma + 1;
      obj.y2 = data.substring(posNum).toInt();
      
      Sensor_Data.objects.push_back(obj);
    }
    pos = newlineIndex + 1;
  }

  return Sensor_Data;
}

// send Sensor data to website
void Send_Sensor_Data()
{
  String sensorData = "";

  // Sonar Sensors
  sensorData += "S1: ";
  sensorData += Sensor_Data.S1_Distance;
  sensorData += " S2: ";
  sensorData += Sensor_Data.S2_Distance;
  sensorData += " S3: ";
  sensorData += Sensor_Data.S3_Distance;
  sensorData += " S4: ";
  sensorData += Sensor_Data.S4_Distance;
  sensorData += '\n';

  // IMU Data
  sensorData += "Yaw: ";
  sensorData += Sensor_Data.yaw;
  sensorData += " Pitch: ";
  sensorData += Sensor_Data.pitch;
  sensorData += " Roll: ";
  sensorData += Sensor_Data.roll;
  sensorData += '\n';
  sensorData += "Accx: ";
  sensorData += Sensor_Data.accx;
  sensorData += " Accy: ";
  sensorData += Sensor_Data.accy;
  sensorData += " Accz: ";
  sensorData += Sensor_Data.accz;
  sensorData += '\n';
  sensorData += "Velx: ";
  sensorData += Sensor_Data.velx;
  sensorData += " Vely: ";
  sensorData += Sensor_Data.vely;
  sensorData += " Velz: ";
  sensorData += Sensor_Data.velz;
  sensorData += '\n';
  sensorData += "Posx: ";
  sensorData += Sensor_Data.posx;
  sensorData += " Posy: ";
  sensorData += Sensor_Data.posy;
  sensorData += " Posz: ";
  sensorData += Sensor_Data.posz;
  sensorData += '\n';

  // CV Data
  sensorData += "Object Count: ";
  sensorData += Sensor_Data.objCount;
  sensorData += "\n";
  for(int i = 0; i < Sensor_Data.objCount; i++)
  {
    sensorData += "Object: ";
    sensorData += Sensor_Data.objects[i].name;
    sensorData += " [x1: ";
    sensorData += Sensor_Data.objects[i].x1;
    sensorData += " x2: ";
    sensorData += Sensor_Data.objects[i].x2;
    sensorData += " y1: ";
    sensorData += Sensor_Data.objects[i].y1;
    sensorData += " y2: ";
    sensorData += Sensor_Data.objects[i].y2;
    sensorData += "]\n";

  }

  pNetworking->pushSerialData(sensorData);
  pNetworking->update();
}
