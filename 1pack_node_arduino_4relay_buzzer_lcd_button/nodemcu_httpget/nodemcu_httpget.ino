#include <SoftwareSerial.h>

SoftwareSerial mySerial(D1, D2); // RX, TX

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  mySerial.begin(9600); // Initialize serial communication with Arduino
}

void loop() {
  if (mySerial.available()) { // Check if there is data available from Arduino
    String receivedMessage = mySerial.readStringUntil('\n'); // Read message from Arduino
    Serial.print("Menu: "); // Print a label for clarity
    Serial.println(receivedMessage); // Print the received message
  }
}
