#include <SoftwareSerial.h>
#define mySerial3 Serial3
#define mySerial1 Serial1
#define mySerial2 Serial2

String currentCode = "";

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
const int buzzer = 44;

bool alreadySent = false; // Variable to indicate whether data has been sent

void setup() {
  Serial.begin(9600);
  mySerial3.begin(9600); // node
  mySerial1.begin(9600); // QR Code 1
  mySerial2.begin(9600); // QR Code 2

  Serial.println("init");

  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  pinMode(sensorPin4, INPUT);
  pinMode(sensorPin5, INPUT);
  pinMode(sensorPin6, INPUT);
  pinMode(sensorPin7, INPUT);
  pinMode(sensorPin8, INPUT);
  pinMode(sensorPin9, INPUT);
  pinMode(sensorPin10, INPUT);
  pinMode(buzzer, OUTPUT);

}

void loop() {
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

  String statusSlot = "tidak tersedia";
  bool validasiSlot = false;
  if (validasiSlot) {
    tone(buzzer, 1000); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec
  }

  Serial.println(sensorValue1);

  if (sensorValue1 == LOW && !alreadySent) {
    sendSerial(statusSlot, "1");
    alreadySent = true; // After sending, mark as sent
    Serial.println("send");
  }

  if (sensorValue2 == LOW && !alreadySent) {
    sendSerial(statusSlot, "2");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue3 == LOW && !alreadySent) {
    sendSerial(statusSlot, "3");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue4 == LOW && !alreadySent) {
    sendSerial(statusSlot, "4");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue5 == LOW && !alreadySent) {
    sendSerial(statusSlot, "5");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue6 == LOW && !alreadySent) {
    sendSerial(statusSlot, "6");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue7 == LOW && !alreadySent) {
    sendSerial(statusSlot, "7");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue8 == LOW && !alreadySent) {
    sendSerial(statusSlot, "8");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue9 == LOW && !alreadySent) {
    sendSerial(statusSlot, "9");
    alreadySent = true; // After sending, mark as sent
  }
  if (sensorValue10 == LOW && !alreadySent) {
    sendSerial(statusSlot, "10");
    alreadySent = true; // After sending, mark as sent
  }
}

void qrCodeMasuk() {
  String resp = "";
  if (mySerial3.available() > 0) {
    String data = mySerial3.readStringUntil('\n');
    resp = data;
  }
  if (mySerial1.available()) {
    char dataReceive = mySerial1.read();
    String url1 = String(dataReceive);
    sendSerial(url1, resp);
    Serial.print(url1);
  }
}

void qrCodeKeluar() {
  String resp = "";
  if (mySerial3.available() > 0) {
    String data = mySerial3.readStringUntil('\n');
    resp = data;
  }
  if (mySerial2.available()) {
    char dataReceive = mySerial2.read();
    String url2 = String(dataReceive);
    sendSerial(url2, resp);
    Serial.print(url2);
  }
}

void sendSerial(String valueMsg, String ids) {
  mySerial3.print(valueMsg);
  mySerial3.print("|");
  mySerial3.println(ids);
  delay(500); // Add a short delay to avoid rapid readings
}
