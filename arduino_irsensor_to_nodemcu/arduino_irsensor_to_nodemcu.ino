#define mySerial Serial3

const int sensorPin1 = 9;
const int sensorPin2 = 4;
const int sensorPin3 = 5;
const int sensorPin4 = 6;
const int sensorPin5 = 7;
int dataSensor1 = 0;
int dataSensor2 = 0;
int dataSensor3 = 0;
int dataSensor4 = 0;
int dataSensor5 = 0;
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
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
    Serial.println("Objek terdeteksi pada Sensor 4");
    dataSensor4 = 1;
  } else {
    Serial.println("Tidak ada objek pada Sensor 4");
    dataSensor4 = 0;
  }

  if (sensorValue5 == LOW) {
    Serial.println("Objek terdeteksi pada Sensor 5");
    dataSensor5 = 1;
  } else {
    Serial.println("Tidak ada objek pada Sensor 5");
    dataSensor5 = 0;
  }

  mySerial.print(dataSensor1);
  mySerial.print('|');
  mySerial.print(dataSensor2);
  mySerial.print('|');
  mySerial.print(dataSensor3);
  mySerial.print('|');
  mySerial.print(dataSensor4);
  mySerial.print('|');
  mySerial.println(dataSensor5);

  delay(1000);  // Delay 1 detik

}
