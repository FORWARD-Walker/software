const int pingPin = 13;
int inPin = 12;

void setup() {
  Serial.begin(9600);
}

void loop()
{
  long duration, cm;
  pinMode(pingPin, OUTPUT);

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  cm = microsecondsToCentimeters(duration);

  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}
