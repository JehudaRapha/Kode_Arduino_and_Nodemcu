#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "TOMMY GAS";
const char* password = "SHENG_TAN";

#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"
#define CHAT_ID "7214692262"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
unsigned long lastPingTime = 0;
int pingInterval = 60000;  // Ping interval in milliseconds (e.g., 60000 ms = 1 minute)

const int relayPin = D2;
bool relayState = LOW;
const int sensor = D5;
int doorState;
int lastDoorState = -1;
bool doorOpenedByBot = false;
bool doorClosedByBot = false;
bool forcedOpenMessageSent = false;
unsigned long lastForcedOpenDetectionTime = 0;
const unsigned long forcedOpenDetectionDelay = 1000; // 1 second delay

String chat_id;

void sendWelcomeMessage() {
  String welcome = "Bot telah terhubung ke Telegram.\n";
  welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu:\n\n";
  welcome += "/buka_pintu untuk membuka kunci pintu\n";
  welcome += "/tutup_pintu untuk menutup kunci pintu\n";
  welcome += "/cek_status_pintu untuk cek kondisi pintu\n";
  welcome += "/ping untuk mengecek koneksi dan latensi\n";
  bot.sendMessage(CHAT_ID, welcome, "");
  Serial.println("Sent welcome message");
}

void sendPing() {
  unsigned long startTime = millis();
  if (bot.getMe()) {
    unsigned long elapsedTime = millis() - startTime;
    bot.sendMessage(chat_id, "Ping: " + String(elapsedTime) + " ms", "");
    Serial.println("Ping sent: " + String(elapsedTime) + " ms");
  } else {
    Serial.println("Failed to get bot info for ping.");
  }
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      Serial.println("Unauthorized user attempted to access the bot.");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println("Message received: " + text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu.\n\n";
      welcome += "/buka_pintu untuk membuka kunci pintu \n";
      welcome += "/tutup_pintu untuk menutup kunci pintu \n";
      welcome += "/cek_status_pintu untuk cek kondisi pintu \n";
      welcome += "/ping untuk mengecek koneksi dan latensi \n";
      bot.sendMessage(chat_id, welcome, "");
      Serial.println("Sent welcome message to " + from_name);
    }

    if (text == "/buka_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu telah terbuka!", "");
      relayState = HIGH;
      doorOpenedByBot = true;
      digitalWrite(relayPin, relayState);
      Serial.println("Door opened by bot.");
    }

    if (text == "/tutup_pintu") {
      bot.sendMessage(chat_id, "Pintu telah terkunci!", "");
      relayState = LOW;
      digitalWrite(relayPin, relayState);
      doorClosedByBot = true;
      Serial.println("Door closed by bot.");
    }

    if (text == "/cek_status_pintu") {
      if (digitalRead(sensor) == HIGH) {
        bot.sendMessage(chat_id, "Pintu sedang terbuka!", "");
        Serial.println("Door status checked: Open");
      } else {
        bot.sendMessage(chat_id, "Pintu sedang tertutup!", "");
        Serial.println("Door status checked: Closed");
      }
    }

    if (text == "/ping") {
      sendPing();
      Serial.println("Ping command executed.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert);
  #endif

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);
  pinMode(sensor, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  Serial.println("Testing Telegram Bot Connection...");
  if (bot.getMe()) {
    Serial.println("Bot is connected to Telegram successfully!");
    sendWelcomeMessage();
  } else {
    Serial.println("Failed to connect to Telegram.");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost connection. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Reconnecting to WiFi...");
    }
    Serial.println("Reconnected to WiFi");
  }

  doorState = digitalRead(sensor);
  
  // Check for forced door opening
  if (doorState == HIGH && relayState == LOW && !doorOpenedByBot) {
    if (!forcedOpenMessageSent && (millis() - lastForcedOpenDetectionTime > forcedOpenDetectionDelay)) {
      bot.sendMessage(CHAT_ID, "Pintu dibuka paksa! Harap periksa kondisi pintu sekarang!", "");
      Serial.println("Forced door open detected.");
      forcedOpenMessageSent = true;
      lastForcedOpenDetectionTime = millis();
    }
  } else if (doorState == LOW) {
    forcedOpenMessageSent = false;
    doorOpenedByBot = false;
  }

  // Handle door closing after bot-initiated opening
  if (relayState == HIGH && doorState == HIGH) {
    delay(500);  // wait for 500 ms before closing the door
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    bot.sendMessage(CHAT_ID, "Jangan lupa tutup pintu lagi ya!", "");
    Serial.println("Door automatically closed after bot-initiated open.");
    doorClosedByBot = true;
  }

  // Reset doorOpenedByBot flag when door is closed
  if (doorState == LOW) {
    doorOpenedByBot = false;
    doorClosedByBot = false;
  }

  // Check for new Telegram messages
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages > 0) {
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }

  // Send ping message at a regular interval
  if (millis() - lastPingTime > pingInterval) {
    lastPingTime = millis(); 
    // Ping functionality removed as per previous code
  }

  if (doorState != lastDoorState) {
    if (doorState == LOW) {
      bot.sendMessage(CHAT_ID, "Terimakasih telah menutup pintu kembali!", "");
      Serial.println("Door closed manually.");
    }
    lastDoorState = doorState;
  }
}