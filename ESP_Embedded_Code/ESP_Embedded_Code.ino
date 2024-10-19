// # Include

// # Defines
#define LED 2 // For LED Heartbeat

// Wifi setup

// Define sensor pins

// Variable for distance

// Setup Code
void setup()
{

  Serial.begin(115200); // Init Serial
  Serial.println("\nSerial Initialized");  // Print confirmation
  
  pinMode(LED, OUTPUT); // Set up LED as output
  digitalWrite(LED, HIGH); // Init to high
  Serial.println("\nHeartbeat Initialized"); // Print confirmation

}

// Main loop
void loop()
{

  digitalWrite(LED, digitalRead(LED) ^ 1);  // Heartbeat



  delay(500); // 500 msec delay

}

// Functions

