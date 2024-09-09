#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Sts";
const char* password = "12345678";

// Initialize Telegram BOT
#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"  // your Bot Token (Get from Botfather)

// Array of authorized chat IDs
const String CHAT_IDS[] = {
  "7214692262",  // Jehuda
  "1516484328",  // ko Marhadi
  "727857551",   // ko Johan
  "266029748",   // ko Cendy
  "884465995",   // ko Bagas
};
const int NUM_CHAT_IDS = sizeof(CHAT_IDS) / sizeof(CHAT_IDS[0]);

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 3000;
unsigned long lastTimeBotRan;
unsigned long lastForcedOpenMsgTime = 0;
int forcedOpenMsgInterval = 5000; // 5 seconds
unsigned long lastForceOpenMessageTime = 0; // Timer for sending forced open message

const int relayPin = D2;
bool relayState = LOW;
const int sensor = D5;  // Pin for the magnetic contact switch
int doorState;
int lastDoorState = -1; 
bool doorOpenedByBot = false;  // Flag to track if the door was opened by the bot
bool doorClosedByBot = false;
bool messageSent = false;

String chat_id;

// Function to check if a chat ID is authorized
bool isAuthorized(String chat_id) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    if (chat_id == CHAT_IDS[i]) {
      return true;
    }
  }
  return false;
}

// Untuk mengirimkan pesan welcome pada saat terkoneksi
void sendWelcomeMessage() {
  String welcome = "Bot telah terhubung ke Telegram.\n";
  welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu:\n\n";
  welcome += "/buka_pintu untuk membuka kunci pintu\n";
  welcome += "/tutup_pintu untuk menutup kunci pintu\n";
  welcome += "/cek_status_pintu untuk cek kondisi pintu\n";
  welcome += "/ping untuk mengecek koneksi dan latensi\n";
  
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], welcome, "");
  }
  Serial.println("Sent welcome message to all authorized users");
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    chat_id = String(bot.messages[i].chat_id);
    if (!isAuthorized(chat_id)) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
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
      bot.sendMessage(chat_id, "Pintu Telah terbuka!", "");
      relayState = HIGH;
      doorOpenedByBot = true;  // Set flag to true
      digitalWrite(relayPin, relayState);
      lastForcedOpenMsgTime = millis(); // Reset timer when the door is opened
    }
    
    if (text == "/tutup_pintu") {
      bot.sendMessage(chat_id, "Pintu telah terkunci!", "");
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
      doorOpenedByBot = false;  // Reset the flag after the door is closed
      doorClosedByBot = true;
    }
    lastDoorState = doorState;
  }
}

void setup() {
  delay(3000); // Wait for a second to allow the serial monitor to initialize
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);
  pinMode(sensor, INPUT_PULLUP);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());
  
  // Test Telegram Bot Connection
  Serial.println("Testing Telegram Bot Connection...");
  if (bot.getMe()) {
    Serial.println("Bot is connected to Telegram successfully!");
  } else {
    Serial.println("Failed to connect to Telegram.");
  }
}

void loop() {
  // Check WiFi connection
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

  // Automatically close the door after 5 seconds if relay is on and the door is detected as closed
  if (relayState == HIGH && digitalRead(sensor) == HIGH && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)) {
    relayState = LOW;
    digitalWrite(relayPin, relayState);
    bot.sendMessage(chat_id, "Jangan lupa tutup pintu lagi ya!", "");
    doorClosedByBot = true;
    messageSent = true;  // Set flag to indicate the message was sent
  }

  // Check if the door is forced open only if no recent close reminder message has been sent
  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    bot.sendMessage(chat_id, "Pintu dibuka paksa!, diharapkan untuk periksa kondisi pintu sekarang!", "");
    lastForceOpenMessageTime = millis(); // Reset the timer for the next forced open message
    doorClosedByBot = false;
  }

  // Reset the messageSent flag when the door is properly closed
  if (digitalRead(sensor) == LOW && doorClosedByBot) {
    messageSent = false; // Reset flag as the door is properly closed
    doorClosedByBot = false; // Reset the flag
  }
}