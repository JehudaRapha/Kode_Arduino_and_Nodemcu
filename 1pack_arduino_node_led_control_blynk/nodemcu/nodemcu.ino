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
#define BLYNK_TEMPLATE_ID "TMPL6YPMzB9As"
#define BLYNK_TEMPLATE_NAME "IoT Smart LED"
#define BLYNK_AUTH_TOKEN "ytlzxzZjuUcoy_UTssjdIDz_U2eeXrO0"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <SoftwareSerial.h>
SoftwareSerial myserial(D1, D2); // RX TX

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "JXE";
char pass[] = "tantan124";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
}

BLYNK_WRITE(V0) {
  int stateButton = param.asInt(); // Baca nilai dari pin virtual V2
  Serial.print("virtual pin state : ");
  Serial.println(stateButton);
  myserial.print("state|");
  myserial.print(stateButton);

}
