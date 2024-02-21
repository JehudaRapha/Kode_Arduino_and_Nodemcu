// #include <SoftwareSerial.h>
#define mySerial3 Serial3
#define mySerial1 Serial1
#include <ArduinoJson.h>
//SoftwareSerial mySerial(2, 3); // RX, TX

String currentCode = "";
bool finish = false;

const int sensorPin1 = 2;
const int sensorPin2 = 3;
const int sensorPin3 = 4;
const int sensorPin4 = 5;
const int sensorPin5 = 6;
const int sensorPin6 = 7;
const int sensorPin7 = 8;
const int sensorPin8 = 9;
const int sensorPin9 = 10;
const int sensorPin10 = 11;
int dataSensor1 = 0;
int dataSensor2 = 0;
int dataSensor3 = 0;
int dataSensor4 = 0;
int dataSensor5 = 0;
int dataSensor6 = 0;
int dataSensor7 = 0;
int dataSensor8 = 0;
int dataSensor9 = 0;
int dataSensor10 = 0;
DynamicJsonDocument jsonData(1024);
String jsonString;

void setup() {
  Serial.begin(9600);
  mySerial3.begin(9600);
  mySerial1.begin(9600);
}

void loop() {

  if (mySerial1.available() > 0 && !finish) {
    String kode = mySerial1.readString();
    if (kode == currentCode) {
      finish = true;
      Serial.println("Data yang sama diterima.");
    } else {
      currentCode = kode;
      Serial.println("Data baru diterima: " + currentCode);
      jsonData["barcode"] = currentCode;

      String jsonString;
      serializeJson(jsonData, jsonString);
      //      mySerial2.print(jsonString);
      Serial.print(jsonString);
    }
  }
  sensorIr();
}

void sensorIr() {
  int sensorValue1 = digitalRead(sensorPin1);
  int sensorValue2 = digitalRead(sensorPin2);
  int sensorValue3 = digitalRead(sensorPin3);
  int sensorValue4 = digitalRead(sensorPin4);
  int sensorValue5 = digitalRead(sensorPin5);
  int sensorValue6 = digitalRead(sensorPin6);
  int sensorValue7 = digitalRead(sensorPin7);
  int sensorValue8 = digitalRead(sensorPin8);
  int sensorValue9 = digitalRead(sensorPin9);
  int sensorValue10 = digitalRead(sensorPin10);

  if (sensorValue1 == LOW) {
    //    Serial.println("Objek terdeteksi pada Sensor 1");
    dataSensor1 = 1;
    //    mySerial3.print("node1");
    //    mySerial3.println(dataSensor1);
    jsonData["sensor1"] = dataSensor1;
    serializeJson(jsonData, jsonString);
    mySerial3.print(jsonString);
  }
  //  else {
  //    //    Serial.println("Tidak ada objek pada Sensor 1");
  //    dataSensor1 = 0;
  //    mySerial3.print("node1");
  //    mySerial3.println(dataSensor1);
  //  }

  if (sensorValue2 == LOW) {
    //    Serial.println("Objek terdeteksi pada Sensor 2");
    dataSensor2 = 1;
    //  mySerial.print("node2");
    //  mySerial.println(dataSensor2);
    jsonData["sensor1"] = dataSensor1;
    serializeJson(jsonData, jsonString);

  } else {
    //    Serial.println("Tidak ada objek pada Sensor 2");= 0;
  }

  if (sensorValue3 == LOW) {
    //    Serial.println("Objek terdeteksi pada Sensor 3");
    dataSensor3 = 1;
    //    mySerial.print("node3");
    //    mySerial.println(dataSensor3);
    jsonData["sensor3"] = dataSensor3;
    serializeJson(jsonData, jsonString);
  } else {
    //    Serial.println("Tidak ada objek pada Sensor 3");
    dataSensor3 = 0;
  }

  if (sensorValue4 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 4");
    dataSensor4 = 1;
    //    mySerial.print("node4");
    //    mySerial.println(dataSensor4);
    jsonData["sensor4"] = dataSensor4;
    serializeJson(jsonData, jsonString);
  } else {
    // Serial.println("Tidak ada objek pada Sensor 4");
    dataSensor4 = 0;
  }

  if (sensorValue5 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 5");
    dataSensor5 = 1;
    //    mySerial.print("node5");
    //    mySerial.println(dataSensor5);
    jsonData["sensor5"] = dataSensor5;
    serializeJson(jsonData, jsonString);
  } else {
    // Serial.println("Tidak ada objek pada Sensor 5");
    dataSensor5 = 0;
  }

  if (sensorValue6 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 6");
    dataSensor6 = 1;
    //    mySerial.print("node6");
    //    mySerial.println(dataSensor6);
    jsonData["sensor6"] = dataSensor6;
    serializeJson(jsonData, jsonString);
  } else {
    // Serial.println("Tidak ada objek pada Sensor 6");
    dataSensor6 = 0;
  }

  if (sensorValue7 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 7");
    dataSensor7 = 1;
    //    mySerial.print("node7");
    //    mySerial.println(dataSensor7);
    jsonData["sensor7"] = dataSensor7;
    serializeJson(jsonData, jsonString);
  } else {
    // Serial.println("Tidak ada objek pada Sensor 7");
    dataSensor7 = 0;
  }

  if (sensorValue8 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 8");
    dataSensor8 = 1;
    //    mySerial.print("node8");
    //    mySerial.println(dataSensor8);
    jsonData["sensor8"] = dataSensor8;
    serializeJson(jsonData, jsonString);
  } else {
    // Serial.println("Tidak ada objek pada Sensor 8");
    dataSensor8 = 0;
  }

  if (sensorValue9 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 9");
    dataSensor9 = 1;
    //    mySerial.print("node9");
    //    mySerial.println(dataSensor9);
    jsonData["sensor9"] = dataSensor9;
    serializeJson(jsonData, jsonString);
  } else {
    // Serial.println("Tidak ada objek pada Sensor 9");
    dataSensor9 = 0;
  }

  if (sensorValue10 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 10");
    dataSensor10 = 1;
    //     mySerial.print("node10");
    //    mySerial.println(dataSensor10);
    jsonData["sensor10"] = dataSensor10;
    serializeJson(jsonData, jsonString);


  } else {
    // Serial.println("Tidak ada objek pada Sensor 10");
    dataSensor10 = 0;
  }


  //serial print
  Serial.print(dataSensor1);
  Serial.print('|');
  Serial.print(dataSensor2);
  Serial.print('|');
  Serial.print(dataSensor3);
  Serial.print('|');
  Serial.print(dataSensor4);
  Serial.print('|');
  Serial.print(dataSensor5);
  Serial.print('|');
  Serial.print(dataSensor6);
  Serial.print('|');
  Serial.print(dataSensor7);
  Serial.print('|');
  Serial.print(dataSensor8);
  Serial.print('|');
  Serial.print(dataSensor9);
  Serial.print('|');
  Serial.println(dataSensor10);

  delay(1000);  // Delay 1 detik

}
