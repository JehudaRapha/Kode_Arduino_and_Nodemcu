const int relayPin = D2;
#define sensor D3

void setup() {
  // Inisialisasi serial monitor untuk debugging
  Serial.begin(9600);

  // Set pin D3 sebagai input untuk membaca status sensor
  pinMode(sensor, INPUT);
}

void loop() {
  int sensorValue = digitalRead(sensor);
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  if (sensorValue == LOW) {
    digitalWrite(relayPin, HIGH); // Misalnya, hidupkan relay jika sensor LOW
  } else {
    digitalWrite(relayPin, LOW); // Matikan relay jika sensor HIGH
  }
  delay(1000);
}
