#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

// Replace with your network credentials
const char* ssid = "Sts";
const char* password = "12345678";

// Initialize Telegram BOT
#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"

// Array of authorized chat IDs
const String CHAT_IDS[] = {
  "7214692262",  // Jehuda Done
  // "1166902768",  // ka Wida Done
  // "1516484328",  // ko Marhadi Done
  // "727857551",   // ko Johan
  // "266029748",   // ko Cendy Done
  // "884465995",   // ko Bagas Done
  // "6322703298",  // ko Tengku Done
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
bool forcedOpenDetected = false;

unsigned long buttonPressTime = 0;
const unsigned long relayActivationTime = 15000;
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
  String welcome = "Bot has been connected to Telegram.\n";
  welcome += "Use /start to begin\n";
  
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
      welcome += "Use these commands to control the door lock.\n\n";
      welcome += "/open_door to unlock the door \n";
      welcome += "/close_door to lock the door \n";
      welcome += "/door_status to check the door condition \n";
      welcome += "/lock_status to check the door lock status \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/open_door") {
      bot.sendMessage(chat_id, "Door lock has been opened", "");
      relayState = HIGH;
      doorOpenedByBot = true;
      digitalWrite(relayPin, relayState);
      lastForcedOpenMsgTime = millis();
    }

    if (text == "/close_door") {
      bot.sendMessage(chat_id, "Door has been locked", "");
      relayState = LOW;
      digitalWrite(relayPin, relayState);
      doorClosedByBot = true;
    }

    if (text == "/door_status") {
      if (digitalRead(sensor) == HIGH) {
        bot.sendMessage(chat_id, "Door is currently open", "");
      } else {
        bot.sendMessage(chat_id, "Door is currently closed", "");
      }
    }

    if (text == "/lock_status") {
      if (relayState == HIGH) {
        bot.sendMessage(chat_id, "Door lock is currently open", "");
      } else {
        bot.sendMessage(chat_id, "Door is currently locked", "");
      }
    }
  }
}

void notifyAllUsers(String message) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], message, "");
  }
}

String getFormattedTime() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", p_tm);
  return String(buffer);
}

void magnetic_door() {
  doorState = digitalRead(sensor);
  if (doorState != lastDoorState) {
    if (doorState == LOW && (doorOpenedByBot || doorOpenedByButton || forcedOpenDetected)) {
      String timeStr = getFormattedTime();
      String message = "Thank you for closing the door! Closed at: " + timeStr;
      notifyAllUsers(message);
      doorOpenedByBot = false;
      doorOpenedByButton = false;
      forcedOpenDetected = false;
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
  } else {
    Serial.println("Failed to connect to Telegram.");
  }

  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
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
    doorOpenedByButton = true;
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
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }

  if (relayState == HIGH && digitalRead(sensor) == HIGH && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)) {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    notifyAllUsers("Don't forget to close the door!");
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

  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && !doorOpenedByButton && !doorOpenedByBot && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    String timeStr = getFormattedTime();
    String message = "Door opened forcefully at " + timeStr + "! Please check the door condition immediately!";
    notifyAllUsers(message);
    lastForceOpenMessageTime = millis();
    messageSent = true;
    doorClosedByBot = false;
    forcedOpenDetected = true;
  }

  if (digitalRead(sensor) == LOW) {
    messageSent = false;
    doorClosedByBot = false;
  }
}