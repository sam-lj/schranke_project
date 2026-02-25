#include <Servo.h>

const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 6;

Servo gateServo;

long duration;
float distance;
bool gateOpen = false;

void setup() {
  Serial.begin(9600);
  gateServo.attach(servoPin);
  gateServo.write(0); // Schranke geschlossen

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("System gestartet. Schranke geschlossen.");
}

void loop() {
  // --- Ultraschallmessung ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms (ca. 5m)
  distance = duration * 0.0343 / 2;         // cm

  Serial.print("Gemessene Entfernung: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- Schranke öffnen, wenn Objekt erkannt ---
  if (distance > 0 && distance <= 20 && !gateOpen) {
    Serial.println("Objekt erkannt! Schranke öffnet...");
    gateServo.write(90);  // Öffnen
    gateOpen = true;
  }

  // --- Schranke schließen, wenn Objekt weg ---
  static unsigned long closeTime = 0;
  
  if ((distance > 25 || distance == 0) && gateOpen) {
    if (closeTime == 0) {
      closeTime = millis();  // Start timer when object leaves
    } else if (millis() - closeTime > 2000) {  // Wait 2 seconds
      Serial.println("Kein Objekt mehr erkannt. Schranke schließt...");
      gateServo.write(0);   // Schließen
      gateOpen = false;
      closeTime = 0;  // Reset timer
    }
  } else {
    closeTime = 0;  // Reset timer if object detected again
  }

  delay(300); // Messintervall
}
