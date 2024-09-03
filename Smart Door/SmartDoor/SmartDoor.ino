#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "CYBORG";
const char* password = "12341234";

#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"
#define CHAT_ID "7214692262"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
unsigned long lastForcedOpenMsgTime = 0;
int forcedOpenMsgInterval = 5000;
unsigned long lastForceOpenMessageTime = 0;

const int relayPin = D2;
bool relayState = LOW;
const int sensor = D5;
int doorState;
int lastDoorState = -1;
bool doorOpenedByBot = false;
bool doorClosedByBot = false;
bool messageSent = false;
bool botConnected = false;

String chat_id;

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
      bot.sendMessage(chat_id, welcome, "");
      Serial.println("Sent welcome message to " + from_name);
    }

    if (text == "/buka_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu telah terbuka!", "");
      relayState = HIGH;
      doorOpenedByBot = true;
      digitalWrite(relayPin, relayState);
      lastForcedOpenMsgTime = millis();
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
  }
}

void magnetic_door() {
  doorState = digitalRead(sensor);
  if (doorState != lastDoorState) {
    if (doorState == LOW) {
      bot.sendMessage(chat_id, "Terimakasih telah menutup pintu kembali!", "");
      doorOpenedByBot = false;
      doorClosedByBot = true;
      Serial.println("Door closed manually.");
    }
    lastDoorState = doorState;
  }
}

void setup() {
  delay(3000);
  Serial.begin(115200);

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
    botConnected = true;
  } else {
    Serial.println("Failed to connect to Telegram.");
    botConnected = false;
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

  magnetic_door();

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages > 0) {
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }

  if (relayState == HIGH && digitalRead(sensor) == HIGH && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)) {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    bot.sendMessage(chat_id, "Jangan lupa tutup pintu lagi ya!", "");
    Serial.println("Door automatically closed after forced open.");
    doorClosedByBot = true;
    messageSent = true;
  }

  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    bot.sendMessage(chat_id, "Pintu dibuka paksa!, diharapkan untuk periksa kondisi pintu sekarang!", "");
    lastForceOpenMessageTime = millis();
    Serial.println("Forced door open detected.");
    doorClosedByBot = false;
  }

  if (digitalRead(sensor) == LOW && doorClosedByBot) {
    messageSent = false;
    doorClosedByBot = false;
  }

  if (!botConnected && bot.getMe()) {
    Serial.println("Bot is reconnected to Telegram successfully!");
    bot.sendMessage(chat_id, "Bot sudah terhubung kembali ke Telegram.", "");
    botConnected = true;
  }

  if (botConnected && !bot.getMe()) {
    Serial.println("Bot lost connection to Telegram.");
    bot.sendMessage(chat_id, "Bot terputus dari Telegram harap menunggu...", "");
    botConnected = false;
  }
}
