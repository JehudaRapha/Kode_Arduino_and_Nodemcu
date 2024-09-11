#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Sts";
const char* password = "12345678";

// Initialize Telegram BOT
#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"

// Array of authorized chat IDs
const String CHAT_IDS[] = {
  "7214692262",  // Jehuda
};
const int NUM_CHAT_IDS = sizeof(CHAT_IDS) / sizeof(CHAT_IDS[0]);

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 3000;
unsigned long lastTimeBotRan;
unsigned long lastForcedOpenMsgTime = 0;
int forcedOpenMsgInterval = 5000;
unsigned long lastForceOpenMessageTime = 0;

const int relayPin = D2;
const int buttonPin = D3;  // New button pin
const int sensor = D5;

bool relayState = LOW;
int doorState;
int lastDoorState = -1;
bool doorOpenedByBot = false;
bool doorClosedByBot = false;
bool messageSent = false;

// New variables for button control
unsigned long buttonPressTime = 0;
const unsigned long relayActivationTime = 15000;  // 10 seconds
bool relayActivatedByButton = false;
bool doorOpenedByButton = false;

String chat_id;

bool isAuthorized(String chat_id) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    if (chat_id == CHAT_IDS[i]) {
      return true;
    }
  }
  return false;
}

void sendWelcomeMessage() {
  String welcome = "Bot telah terhubung ke Telegram.\n";
  welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu:\n\n";
  welcome += "/buka_pintu untuk membuka kunci pintu\n";
  welcome += "/tutup_pintu untuk menutup kunci pintu\n";
  welcome += "/cek_status_pintu untuk cek kondisi pintu\n";

  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], welcome, "");
  }
  Serial.println("Sent welcome message to all authorized users");
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    if (!isAuthorized(chat_id)) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu.\n\n";
      welcome += "/buka_pintu untuk membuka kunci pintu \n";
      welcome += "/tutup_pintu untuk menutup kunci pintu \n";
      welcome += "/cek_status_pintu untuk cek kondisi pintu \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/buka_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu Telah terbuka!", "");
      relayState = HIGH;
      doorOpenedByBot = true;
      digitalWrite(relayPin, relayState);
      lastForcedOpenMsgTime = millis();
    }

    if (text == "/tutup_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu telah terkunci!", "");
      relayState = LOW;
      digitalWrite(relayPin, relayState);
      doorClosedByBot = true;
    }

    if (text == "/cek_status_pintu") {
      if (digitalRead(sensor) == HIGH) {
        bot.sendMessage(chat_id, "Pintu sedang terbuka!", "");
      } else {
        bot.sendMessage(chat_id, "Pintu sedang tertutup!", "");
      }
    }
  }
}

void notifyAllUsers(String message) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], message, "");
  }
}

void magnetic_door() {
  doorState = digitalRead(sensor);
  if (doorState != lastDoorState) {
    if (doorState == LOW) {
      bot.sendMessage(chat_id, "Terimakasih telah menutup pintu kembali!", "");
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
  pinMode(buttonPin, INPUT_PULLUP);  // Set button pin as input with pull-up

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
  } else {
    Serial.println("Failed to connect to Telegram.");
    sendWelcomeMessage();
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

  // Handle button press
  if (digitalRead(buttonPin) == LOW && !relayActivatedByButton) {
    relayActivatedByButton = true;
    buttonPressTime = millis();
    digitalWrite(relayPin, HIGH);
    Serial.println("Button pressed, relay activated");
  }

  // Check if it's time to deactivate the relay after button press
  if (relayActivatedByButton && millis() - buttonPressTime >= relayActivationTime) {
    digitalWrite(relayPin, LOW);
    relayActivatedByButton = false;
    Serial.println("Relay deactivated after button press");
  }

  magnetic_door();

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages > 0) {
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }

  // Automatically close the door after 5 seconds if relay is on and the door is detected as closed
  if (relayState == HIGH && digitalRead(sensor) == HIGH && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)) {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    bot.sendMessage(chat_id, "Jangan lupa tutup pintu lagi ya!", "");
    doorClosedByBot = true;
    messageSent = true;
  }

  // New logic: if sensor is HIGH, deactivate relay if activated by button
  if (digitalRead(sensor) == HIGH && relayActivatedByButton) {
    digitalWrite(relayPin, LOW);
    relayActivatedByButton = false;
    doorOpenedByButton = true;  // Indicate that the door was opened by the button
    Serial.println("Sensor HIGH, relay deactivated");
  }

  // Avoid sending forced open message if the door was opened by the button
  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && !doorOpenedByButton && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    bot.sendMessage(chat_id, "Pintu dibuka paksa!, diharapkan untuk periksa kondisi pintu sekarang!", "");
    lastForceOpenMessageTime = millis();
    doorClosedByBot = false;
  }

  // Reset the flag when the door is properly closed
  if (digitalRead(sensor) == LOW) {
    doorOpenedByButton = false;  // Reset the flag when door is closed
    messageSent = false;
    doorClosedByBot = false;
  }
}
