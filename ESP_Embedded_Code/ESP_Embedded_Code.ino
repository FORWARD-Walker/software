// EEL 4914 Senior Design MCU Code
// Developers: Tobiah Bower, Morgan Snyder, Matthew Morello
// The Code serves to drive all process on the ESP32 MCU for our project
// Features include: WiFi connectivity, data transmission over wifi, Locally 
// hosted network, sensor data processing, motor controls, basic coding functionality
// Located remotely in https://github.com/FORWARD-Walker/software

// # Include
#include <Wire.h> // I2C
#include "Networking.h"
#include "Constants.h"
#include "Navigation.h"
#include "Walker.h"
#include "Environment.h"
#include "Pins.h"

// Network Reference
Networking* pNetworking = nullptr;

// Walker structure
Walker* pWalker = nullptr;

// Environment object
Environment* pEnvironment = nullptr;

// Navigation Reference
Navigation* pNavigation = nullptr;

// Timer Objects
hw_timer_t *Timer_1Hz = nullptr;
hw_timer_t *Timer_5Hz = nullptr;

// ISR prototypes
static void IRAM_ATTR Timer_1Hz_ISR();
static void IRAM_ATTR Timer_5Hz_ISR();

// Boolean ISR Flags
volatile bool Timer_1HZ_FG = false;
volatile bool Timer_5HZ_FG = false;

// Setup Code
void setup()
{
  // Set up serial
  Serial.begin(115200); // Init Serial

  // Set up I2C (using defined pins from Pins.h)
  Wire.begin(I2C_SDA, I2C_SCL);

  // Setup heartbeat LEDs
  pinMode(yLED, OUTPUT);
  digitalWrite(yLED, HIGH);
  pinMode(bLED, OUTPUT);
  digitalWrite(bLED, LOW);

  // --- Timer 1 Hz ---
  Timer_1Hz = timerBegin(1000000);
  timerAttachInterrupt(Timer_1Hz, &Timer_1Hz_ISR);
  timerAlarm(Timer_1Hz, 1000000, true, 0);

  // --- Timer 5 Hz ---
  Timer_5Hz = timerBegin(5000000);
  timerAttachInterrupt(Timer_5Hz, &Timer_5Hz_ISR);
  timerAlarm(Timer_5Hz, 1000000, true, 0);


  // Setup ESP32 network
  pNetworking = new Networking();
  pNetworking->pushSerialData("Network Initialized!\n");

  // Init Walker
  pWalker = new Walker(pNetworking);
  pNetworking->pushSerialData("Walker Initialized!\n");

  // Init Environment
  pEnvironment = new Environment(pWalker, pNetworking);
  pNetworking->pushSerialData("Environment Initialized!\n");

  // Init Navigation
  pNavigation = new Navigation(pWalker, pNetworking, pEnvironment);
  pNetworking->pushSerialData("Navigation Initialized!\n");
}

// Main loop
void loop()
{
  // Handle 1 Hz tasks
  if (Timer_1HZ_FG) 
  {
    Blink_Heartbeat(); // Blink Heartbeat
    pNetworking->update(); // Service Network

    Timer_1HZ_FG = false;  // Reset flag
  }

  // Handle 5 Hz tasks
  if (Timer_5HZ_FG)
  {
    // Update environment data and run navigation algorithms
    pEnvironment->updateEnvironment();
    pNavigation->navigate();

    Timer_5HZ_FG = false;  // Reset flag
    
  }
}

// Timer ISRs
static void IRAM_ATTR Timer_1Hz_ISR()
{
  Timer_1HZ_FG = true;
}

static void IRAM_ATTR Timer_5Hz_ISR()
{
  Timer_5HZ_FG = true;
}

void Blink_Heartbeat()
{
    digitalWrite(yLED, digitalRead(yLED) ^ 1); // Toggle yLED
    digitalWrite(bLED, digitalRead(bLED) ^ 1); // Toggle bLED
}
