#include <SoftwareSerial.h>

// SoftwareSerial myserial(D1, D2); // RX, TX
SoftwareSerial myserial(D2, D4); // RX, TX

void setup() {
  Serial.begin(9600);
  myserial.begin(9600);
}

void loop() {
  if (myserial.available() > 0) {
    String dataString = myserial.readStringUntil('\n');  // Membaca string dari Arduino hingga karakter newline
    Serial.print("Data Serial: ");
    Serial.println(dataString);
  }
  
}
