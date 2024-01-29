// nodemcu
#include<SoftwareSerial.h>
SoftwareSerial myserial(D7,D8);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);  // Introduce a small delay

   String data = Serial.read();
   Serial.print("Data Serial : ");
   Serial.println(data);
}

