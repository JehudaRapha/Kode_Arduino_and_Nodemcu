// #include <SoftwareSerial.h>
#define mySerial Serial3
// SoftwareSerial mySerial(2, 3); // RX, TX

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
void setup() {
  Serial.begin(9600);
  mySerial.begin(115200);
}

void loop() {
  sensorIr();
}

void sensorIr(){
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
    Serial.println("Objek terdeteksi pada Sensor 1");
    dataSensor1 = 1;
  } else {
    Serial.println("Tidak ada objek pada Sensor 1");
    dataSensor1 = 0;
  }

  if (sensorValue2 == LOW) {
    Serial.println("Objek terdeteksi pada Sensor 2");
    dataSensor2 = 1;

  } else {
    Serial.println("Tidak ada objek pada Sensor 2");
    dataSensor2 = 0;
  }

  if (sensorValue3 == LOW) {
    Serial.println("Objek terdeteksi pada Sensor 3");
    dataSensor3 = 1;
  } else {
    Serial.println("Tidak ada objek pada Sensor 3");
    dataSensor3 = 0;
  }

  if (sensorValue4 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 4");
    dataSensor4 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 4");
    dataSensor4 = 0;
  }

  if (sensorValue5 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 5");
    dataSensor5 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 5");
    dataSensor5 = 0;
  }

    if (sensorValue6 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 6");
    dataSensor6 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 6");
    dataSensor6 = 0;
  }

    if (sensorValue7 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 7");
    dataSensor7 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 7");
    dataSensor7 = 0;
  }

    if (sensorValue8 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 8");
    dataSensor8 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 8");
    dataSensor8 = 0;
  }

    if (sensorValue9 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 9");
    dataSensor9 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 9");
    dataSensor9 = 0;
  }

    if (sensorValue10 == LOW) {
    // Serial.println("Objek terdeteksi pada Sensor 10");
    dataSensor10 = 1;
  } else {
    // Serial.println("Tidak ada objek pada Sensor 10");
    dataSensor10 = 0;
  }

  mySerial.print(dataSensor1);
  mySerial.print('|');
  mySerial.print(dataSensor2);
  mySerial.print('|');
  mySerial.print(dataSensor3);
  mySerial.print('|');
  mySerial.print(dataSensor4);
  mySerial.print('|');
  mySerial.print(dataSensor5);
  mySerial.print('|');
  mySerial.print(dataSensor6);
  mySerial.print('|');
  mySerial.print(dataSensor7);
  mySerial.print('|');
  mySerial.print(dataSensor8);
  mySerial.print('|');
  mySerial.print(dataSensor9);
  mySerial.print('|');
  mySerial.println(dataSensor10);
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
