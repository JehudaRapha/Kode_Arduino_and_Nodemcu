#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "JXE";
const char* password = "tantan124";

const char* serverIP = "192.168.204.179"; // Removed "/percobaan" from serverIP
const int serverPort = 80;
const String endpoint = "/percobaan/percobaan.php"; // Adjusted endpoint

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Send HTTP GET request to the PHP server
  sendGETRequest();
}

void loop() {
  // Nothing to do here in this example
}

void sendGETRequest() {
  WiFiClient client;
  HTTPClient http;

  // Construct the URL
  String url = "http://" + String(serverIP) + endpoint;

  // Add your data parameters to the URL if needed
   url += "?value=" + String(60);

  Serial.print("URL: ");
  Serial.println(url);

  // Send GET request
  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
    }
  } else {
    Serial.println("Error in HTTP GET request");
  }
  http.end();
}
