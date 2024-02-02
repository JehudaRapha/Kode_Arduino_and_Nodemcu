#include <SoftwareSerial.h>

SoftwareSerial myserial(10, 11); // RX, TX
const int ledPin = 13; // Ganti dengan pin yang sesuai pada board Anda
const int relayPin = 7; // Ganti dengan pin relay pada board Anda

void setup() {
  Serial.begin(115200);
  myserial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(relayPin, HIGH);
}
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };

  do {
    // Find the next separator in the data
    strIndex[0] = strIndex[1] + 1;
    strIndex[1] = data.indexOf(separator, strIndex[0]);

    // If the separator is not found, use the length of the string
    if (strIndex[1] == -1) {
      strIndex[1] = data.length();
    }

    // Extract the substring between separators
    String value = data.substring(strIndex[0], strIndex[1]);

    // If the current index matches the target index, return the value
    if (found == index) {
      return value;
    }

    // Increment the found counter if a separator is found
    found++;
  } while (strIndex[1] < data.length());

  return "";
}

void loop() {
  //  String incomingString = myserial.readStringUntil('\n');
  //  Serial.print("Received Data: ");
  //    Serial.println(incomingString);


  if (myserial.available() > 0) {
    String dataSerial = myserial.readStringUntil('\n');
    dataSerial.trim();
    String data1 = getValue(dataSerial, '|', 0);
    String data2 = getValue(dataSerial, '|', 1);
    if (data1 == "1" ) {
      Serial.println("led on");
      digitalWrite(ledPin, HIGH); // LED ON
    }else if(data1 == "0"){
      digitalWrite(ledPin, LOW);
    }
    
    if (data2 == "1") {
      Serial.println("relay on");
      digitalWrite(relayPin, LOW);
    }
    else if(data2 == "0"){
      digitalWrite(relayPin, HIGH);
    }
  }
}
