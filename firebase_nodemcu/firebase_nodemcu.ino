  #include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <Firebase_ESP_Client.h>

// Additional includes for token and RTDB helpers
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "CYBORG_IT"
#define WIFI_PASSWORD "12341234"

/* 2. Define the API Key */
#define API_KEY "AIzaSyDeTcRwTVF3KmyIhuLJQIZaZ8jZBP80my8"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://test-3c493-default-rtdb.asia-southeast1.firebasedatabase.app/"

/* 4. Define the user Email and password that already registered or added in your project */
#define USER_EMAIL "jehudarapha653@gmail.com"
#define USER_PASSWORD "12345678"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

const int ledPin = D1;

void setup()
{
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the API key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign-in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long-running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Comment or pass a false value when WiFi reconnection will be controlled by your code or third-party library, e.g., WiFiManager
  Firebase.reconnectNetwork(true);

  // Set SSL buffer sizes
  fbdo.setBSSLBufferSize(4096, 1024);

  // Begin Firebase
  Firebase.begin(&config, &auth);

  // Begin Realtime Database stream
  if (!Firebase.RTDB.beginStream(&fbdo, "/value"))
    Serial.printf("Stream begin error: %s\n\n", fbdo.errorReason().c_str());

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop()
{
  // Call Firebase.ready() repeatedly to handle authentication tasks.
  if (Firebase.ready())
  {
    // Read the Realtime Database stream
    if (!Firebase.RTDB.readStream(&fbdo))
      Serial.printf("Stream read error: %s\n\n", fbdo.errorReason().c_str());

    if (fbdo.streamTimeout())
    {
      Serial.println("Stream timed out, resuming...\n");

      if (!fbdo.httpConnected())
        Serial.printf("Error code: %d, reason: %s\n\n", fbdo.httpCode(), fbdo.errorReason().c_str());
    }

    if (fbdo.streamAvailable())
    {
      // Extract the value from the Realtime Database
      String value = fbdo.stringData();
      Serial.println(value);
      if(value == "1"){
        Serial.println("on");
        digitalWrite(ledPin, HIGH);
      }
      else {
        Serial.println("off");
        digitalWrite(ledPin, LOW);
      }
    }
  }

  // After calling stream.keepAlive, now we can track the server connecting status
  if (!fbdo.httpConnected())
  {
    // Server was disconnected!
  }
}
