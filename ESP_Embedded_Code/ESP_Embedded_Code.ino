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
#include "Enviroment.h"
#include "Pins.h"

// Network Ref
Networking* pNetworking = nullptr;

// Walker structure
Walker* pWalker = nullptr;

// Enviroment object
Enviroment* pEnviroment = nullptr;

// Navigation Ref
Navigation* pNavigation = nullptr;

// Timer Objects
hw_timer_t *Timer_1Hz = nullptr;
hw_timer_t *Timer_10Hz = nullptr;
hw_timer_t *Timer_30Hz = nullptr;

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
  pWalker = new Walker(pNetworking);
  pNetworking->pushSerialData("Walker Initialized!\n");

  // Init Enviroment
  pEnviroment = new Enviroment(pWalker, pNetworking);
  pNetworking->pushSerialData("Enviroment Initialized!\n");

  // Init Navigation
  pNavigation = new Navigation(pWalker, pEnviroment);
  pNetworking->pushSerialData("Navigation Initialized!\n");
}

// Main loop
void loop()
{ 

if (Timer_30HZ_FG)
{
    Timer_30HZ_FG = false; // Reset ISR
}


  // 10 HZ ISR
  if(Timer_10HZ_FG)
  {
    pNavigation->setSpeed(); // Poll to adjust speed based on potentiometer
    pEnviroment->updateEnviroment(); // Update enviroment data
    pNavigation->navigate(); // Navigate

    Timer_10HZ_FG = false; // Reset ISR
  }

  // 1 HZ ISR
  if(Timer_1HZ_FG)
  {
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


