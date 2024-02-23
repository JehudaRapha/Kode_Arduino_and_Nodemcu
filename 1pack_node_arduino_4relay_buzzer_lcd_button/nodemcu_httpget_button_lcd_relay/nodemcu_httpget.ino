#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D1, D2); // RX, TX

const char* ssid = "iot";
const char* password = "12345678";

String serverName = "http://192.168.137.1:8000/api/data";

unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  mySerial.begin(9600); // Initialize serial communication with Arduino
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
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

void loop() {
  // Send an HTTP POST request depending on timerDelay
  if (myserial.available() > 0) {
    String data = myserial.readStringUntil('\n');
    Serial.println(data);
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String serverPath = serverName + "/" + data;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());

      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}
