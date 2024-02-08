#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
SoftwareSerial myserial(D1, D2);


const char* ssid = "JXE";
const char* password = "tantan124";

const char* serverIP = "192.168.204.179"; // Removed "/percobaan" from serverIP
const int serverPort = 80;
const String endpoint = "/percobaan/percobaan.php"; // Adjusted endpoint


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  myserial.begin(115200);
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

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  // Iterate over the characters in the string
  for (int i = 0; i <= maxIndex; i++) {
    // If the current character is the separator
    if (data.charAt(i) == separator) {
      found++;
      // If we have found the desired index
      if (found == index) {
        strIndex[1] = i; // Set the end index of the substring
        break; // Exit the loop
      }
      strIndex[0] = i + 1; // Set the start index of the next substring
    }
  }
  // If the index was found and it's not the last character
  if (found > 0 && strIndex[1] < maxIndex) {
    // Return the substring from the last separator to the end of the string
    return data.substring(strIndex[0], strIndex[1]);
  } else {
    // Return an empty string if the index was not found or it's the last character
    return "";
  }
}


void sendGETRequest(String node, int dataSensor) {
  WiFiClient client;
  HTTPClient http;

  // Construct the URL
  String url = "http://" + String(serverIP) + endpoint;

  // Add your data parameters to the URL if needed
  url += "/" + node + "/" + String(dataSensor);

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


void loop() { // run over and over
  if (myserial.available()) {
    String data = myserial.readStringUntil('\n');
    String node1 = getValue(data, '|', 0); //sensor 1
    String node2 = getValue(data, '|', 1); //sensor 2

    if (node1.equals("1")) {
      ("node1", node1.toInt());
    }
  }
}
