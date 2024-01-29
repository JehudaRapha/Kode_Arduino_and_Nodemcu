#include <SoftwareSerial.h>

SoftwareSerial mySerial(D1, D2); // RX, TX

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  mySerial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
 if (mySerial.available()) {
    Serial.println(mySerial.readStringUntil('\n'));
  }
}
