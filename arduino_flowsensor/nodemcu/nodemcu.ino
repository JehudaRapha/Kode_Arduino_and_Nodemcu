/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  NOTE: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6D_iQaX4b"
#define BLYNK_TEMPLATE_NAME "IoT Keakuratan Debit Air"
#define BLYNK_AUTH_TOKEN "Bo2uU0rfqnQU259Nl4iqx5PNPpu0_Mx2"

f#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <SoftwareSerial.h>
SoftwareSerial myserial(D1, D2); // RX TX

const int relayPin = 5; // Pin yang terhubung ke relay

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "CYBORG_IT";
char pass[] = "12341234";

void setup()
{
  // Debug console
  Serial.begin(9600);
  myserial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(relayPin, OUTPUT);
}

void totalAir() {
  String data = myserial.readStringUntil('\n'); // Read data from Arduino until newline character
  Serial.print("Total Yang Mengalir: ");
  Serial.println(data);

  //  Kirim data total yang mengalir ke pin virtual V1 di aplikasi Blynk
  Blynk.virtualWrite(V1, data.toInt()); // Mengubah data menjadi integer sebelum dikirim ke Blynk
}

void kecepatanAir() {
//  String data = myserial.readStringUntil('\n'); // Read data from Arduino until newline character
//  Serial.print("Kecepatan air Mengalir: ");
//  Serial.println(data);

  if (myserial.available() > 0) {
    String data = myserial.readStringUntil('\n');
    Serial.print("data myserial : ");
    Serial.println(data);
    // Kirim data kecepatan air yang mengalir ke pin virtual V3 di aplikasi Blynk
    Blynk.virtualWrite(V0, data.toInt());
    //    Serial.print("Terkirim : ");
    //    Serial.println(data);
  }
}


void loop() {
  if (myserial.available() > 0) {
    totalAir();
    kecepatanAir();
  }
  Blynk.run();
}


// Fungsi untuk mengontrol relay berdasarkan data yang diterima dari pin virtual V2 di aplikasi Blynk
BLYNK_WRITE(V2) {
  int stateButton = param.asInt(); // Baca nilai dari pin virtual V2
  Serial.print("virtual pin state : ");
  Serial.println(stateButton);
  myserial.print("state|");
  myserial.print(stateButton);

  // Sesuaikan pengontrolan relay sesuai  dengan nilai yang diterima
  //  if (relayState == 0) {
  //    digitalWrite(relayPin, HIGH); // Matikan relay
  //    Serial.println("Relay dimatikan");
  //  } else {
  //    digitalWrite(relayPin, LOW); // Nyalakan relay
  //    Serial.println("Relay dinyalakan");
  //  }
}
