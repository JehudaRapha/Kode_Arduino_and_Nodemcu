#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600); // Set the baud rate for SoftwareSerial
}

void loop() {
  // Send a message to the NodeMCU using SoftwareSerial
  mySerial.println("Hello from Arduino!");

  delay(1000); // Wait for a second
}
