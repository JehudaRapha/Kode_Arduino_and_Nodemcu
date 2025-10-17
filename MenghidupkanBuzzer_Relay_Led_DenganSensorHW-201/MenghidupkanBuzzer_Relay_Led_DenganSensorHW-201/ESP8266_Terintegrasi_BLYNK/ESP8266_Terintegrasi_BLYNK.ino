#define BLYNK_TEMPLATE_ID "TMPL6REGJZh8j"
#define BLYNK_TEMPLATE_NAME "IoT SerKom"
#define BLYNK_AUTH_TOKEN "WetRDJZrgS6nOVBUPWiPVlrybj8exMH9"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "TOMMY GAS";     
char pass[] = "SHENG_TAN";  

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("ESP8266 Connecting to Blynk.. .");
}

BLYNK_WRITE(V0) {
  int value = param.asInt();
  if (value == 1) {
    Serial.println("LED_ON");  // Kirim ke Arduino
  } else {
    Serial.println("LED_OFF");
  }
}

BLYNK_WRITE(V1) { // Buzzer
  int value = param.asInt();
  if (value == 1) Serial.println("BUZZER_ON");  
  else Serial.println("BUZZER_OFF");
}

void loop() {
  Blynk.run();
}
