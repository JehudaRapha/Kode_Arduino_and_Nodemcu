// Arduino Uno
#include <SoftwareSerial.h>
SoftwareSerial myserial(4, 3); // RX TX

void setup() {
  // Memulai komunikasi serial pada baud rate 9600 untuk monitor serial
  Serial.begin(9600);

  // Memulai SoftwareSerial pada baud rate 115200 untuk komunikasi dengan ESP32
  myserial.begin(115200);
  Serial.println("Arduino Ready to receive data");
}

void loop() {
  // Cek apakah ada data yang masuk dari ESP32
  if (myserial.available()) {
    String receivedData = myserial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(receivedData);
  }
}
