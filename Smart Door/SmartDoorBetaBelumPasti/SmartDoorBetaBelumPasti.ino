#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Ganti dengan kredensial jaringan Anda
const char* ssid = "Sts";
const char* password = "12345678";

// Inisialisasi Telegram BOT dengan token BOT Anda
#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"

// Array untuk ID chat yang diotorisasi (semua dalam format string)
const String CHAT_IDS[] = {
  "7214692262",  // Jehuda
  "1166902768",  // ka Wida
  "1516484328",  // ko Marhadi
  "727857551",   // ko Johan
  "266029748",   // ko Cendy
  "884465995",   // ko Bagas
  "6322703298",  // ko Tengku
};
const int NUM_CHAT_IDS = sizeof(CHAT_IDS) / sizeof(CHAT_IDS[0]);

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
const int buttonPin = D3;
const int sensor = D5;

bool relayState = LOW;
int doorState;
int lastDoorState = -1;
bool doorOpenedByBot = false;
bool doorClosedByBot = false;
bool messageSent = false;

unsigned long buttonPressTime = 0;
const unsigned long relayActivationTime = 15000;
bool relayActivatedByButton = false;
bool doorOpenedByButton = false;

String chat_id;
String lastAuthorizedChatId = "";

bool isAuthorized(String chat_id) {
  Serial.println("Checking authorization for chat_id: " + chat_id);
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    Serial.println("Comparing with authorized ID: " + CHAT_IDS[i]);
    if (chat_id == CHAT_IDS[i]) {
      lastAuthorizedChatId = chat_id;
      Serial.println("Authorization successful for chat_id: " + chat_id);
      return true;
    }
  }
  Serial.println("Authorization failed for chat_id: " + chat_id);
  return false;
}

void sendWelcomeMessage() {
  String welcome = "Bot telah terhubung ke Telegram.\n";
  welcome += "/start untuk memulai bot\n";
  
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], welcome, "");
    Serial.println("Sent welcome message to: " + CHAT_IDS[i]);
  }
  Serial.println("Sent welcome message to all authorized users");
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println("Number of new messages: " + String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    Serial.println("Received message from chat_id: " + chat_id);
    
    if (!isAuthorized(chat_id)) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      Serial.println("Unauthorized access attempt from: " + chat_id);
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println("Message: " + text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu.\n\n";
      welcome += "/buka_pintu untuk membuka kunci pintu \n";
      welcome += "/tutup_pintu untuk menutup kunci pintu \n";
      welcome += "/status_pintu untuk cek kondisi pintu \n";
      welcome += "/status_kunci untuk cek status kunci pintu \n";
      welcome += "/cek_id untuk memeriksa ID chat Anda \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/buka_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu telah terbuka", "");
      relayState = HIGH;
      doorOpenedByBot = true;
      digitalWrite(relayPin, relayState);
      lastForcedOpenMsgTime = millis();
    }

    if (text == "/tutup_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu telah terkunci", "");
      relayState = LOW;
      digitalWrite(relayPin, relayState);
      doorClosedByBot = true;
    }

    if (text == "/status_pintu") {
      if (digitalRead(sensor) == HIGH) {
        bot.sendMessage(chat_id, "Pintu sedang terbuka", "");
      } else {
        bot.sendMessage(chat_id, "Pintu sedang tertutup", "");
      }
    }

    if (text == "/status_kunci") {
      if (relayState == HIGH) {
        bot.sendMessage(chat_id, "Kunci pintu sedang terbuka", "");
      } else {
        bot.sendMessage(chat_id, "Kunci pintu sedang terkunci", "");
      }
    }

    if (text == "/cek_id") {
      String idInfo = "ID chat Anda adalah: " + chat_id + "\n";
      idInfo += "Status otorisasi: " + String(isAuthorized(chat_id) ? "Berhasil" : "Gagal");
      bot.sendMessage(chat_id, idInfo, "");
    }
  }
}

void notifyAllUsers(String message) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], message, "");
    Serial.println("Notified user: " + CHAT_IDS[i]);
  }
}

void magnetic_door() {
  doorState = digitalRead(sensor);
  if (doorState != lastDoorState) {
    if (doorState == LOW) {
      bot.sendMessage(lastAuthorizedChatId, "Terimakasih telah menutup pintu kembali!", "");
      doorOpenedByBot = false;
      doorClosedByBot = true;
    }
    lastDoorState = doorState;
  }
}

void setup() {
  Serial.begin(115200);

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);
#endif

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);

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

  if (bot.getMe()) {
    Serial.println("Bot is connected to Telegram successfully!");
    sendWelcomeMessage();
    
    // Tambahkan pengecekan otorisasi untuk setiap ID chat
    for (int i = 0; i < NUM_CHAT_IDS; i++) {
      if (isAuthorized(CHAT_IDS[i])) {
        Serial.println("Authorized ID: " + CHAT_IDS[i]);
      } else {
        Serial.println("Failed to authorize ID: " + CHAT_IDS[i]);
      }
    }
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

  if (digitalRead(buttonPin) == LOW && !relayActivatedByButton) {
    relayActivatedByButton = true;
    buttonPressTime = millis();
    digitalWrite(relayPin, HIGH);
    relayState = HIGH;
    Serial.println("Button pressed, relay activated");
  }

  if (relayActivatedByButton && millis() - buttonPressTime >= relayActivationTime) {
    digitalWrite(relayPin, LOW);
    relayState = LOW;
    relayActivatedByButton = false;
    Serial.println("Relay deactivated after button press");
  }

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages > 0) {
      Serial.println("New messages received: " + String(numNewMessages));
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }

  if (relayState == HIGH && digitalRead(sensor) == HIGH && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)) {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    bot.sendMessage(lastAuthorizedChatId, "Jangan lupa tutup pintu lagi ya!", "");
    doorClosedByBot = true;
    messageSent = true;
  }

  if (digitalRead(sensor) == HIGH && relayActivatedByButton) {
    digitalWrite(relayPin, LOW);
    relayState = LOW;
    relayActivatedByButton = false;
    doorOpenedByButton = true;
    Serial.println("Sensor HIGH, relay deactivated");
  }

  magnetic_door();

  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && !doorOpenedByButton && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    notifyAllUsers("Pintu dibuka paksa!, diharapkan untuk periksa kondisi pintu sekarang!");
    lastForceOpenMessageTime = millis();
    messageSent = true;
    doorClosedByBot = false;
  }

  if (digitalRead(sensor) == LOW) {
    doorOpenedByButton = false;
    messageSent = false;
    doorClosedByBot = false;
  }
}