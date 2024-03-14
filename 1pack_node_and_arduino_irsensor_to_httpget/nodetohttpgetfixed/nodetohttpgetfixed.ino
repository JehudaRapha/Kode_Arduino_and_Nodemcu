#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h> // Include the WiFiClient library
#include <SoftwareSerial.h>

const char* ssid = "nanda";
const char* password = "nanda123";

SoftwareSerial myserial(D1, D2);

void setup() {
  Serial.begin(115200); // Starts the serial communication
  WiFi.begin(ssid, password); // Connects to the WiFi
  myserial.begin(9600);

  while (WiFi.status() != WL_CONNECTED) { // Waits for the connection to establish
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  while (!Serial) {
    ;
  }

  Serial.println("Connected to the WiFi network");
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
  if (myserial.available() > 0) {
    String data = myserial.readStringUntil('\n'); // Read the data
    data.trim(); // Trim the data to remove possible newline or carriage return characters

    String statusSlot = getValue(data, '|', 0); // Get the namaSlot value
    statusSlot.trim(); // Trim the namaSlot value

    String idSlot = getValue(data, '|', 1); // Get the value
    idSlot.trim(); // Trim the value

    // Debug prints
    Serial.print("status Slot: ");
    Serial.println(statusSlot);
    Serial.print("Value: ");
    Serial.println(idSlot);

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String url = "http://192.168.1.28:8000/api/sensorIr/parked/" + statusSlot + "/" + idSlot;

      // Debug print for the URL
      Serial.print("URL: ");
      Serial.println(url);

      http.begin(client, url); // Specify request destination

      int httpCode = http.GET(); // Send the request

      if (httpCode > 0) {
        String payload = http.getString(); // Get the request response payload
        Serial.println(httpCode); // Prints the HTTP status code
        Serial.println(payload); // Prints the response payload
      } else {
        Serial.println("Error on HTTP request");
      }
      http.end(); // Close connection
    }
  }
}
