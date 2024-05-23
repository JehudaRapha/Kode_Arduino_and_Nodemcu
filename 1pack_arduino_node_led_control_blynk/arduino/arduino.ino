#include <SoftwareSerial.h>
SoftwareSerial myserial(2, 3); // RX TX

const int ledPin = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  receivedData();
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void receivedData() {
  if (myserial.available() > 0) {
    String data = myserial.readStringUntil('\n');
    String state = getValue(data, '|', 0);
    String value = getValue(data, '|', 1);

    if (state == "state" && value == "1") {
      digitalWrite(ledPin, HIGH);
    } else if (state == "state" && value == "0") {
      digitalWrite(ledPin, LOW);
    } else {
      return;
    }
    Serial.print("data blynk : ");
    Serial.println(state);
  }
}