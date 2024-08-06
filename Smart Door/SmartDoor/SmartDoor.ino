#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "TOMMY GAS";
const char* password = "SHENG_TAN";

// Initialize Telegram BOT
#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "7214692262"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
unsigned long lastForcedOpenMsgTime = 0;
int forcedOpenMsgInterval = 2000; // 2 seconds

const int relayPin = D2;
bool relayState = LOW;
const int sensor = D3;  // Pin for the magnetic contact switch
int doorState;
int lastDoorState = -1;

String chat_id;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/buka_pintu untuk membuka pintu \n";
      welcome += "/tutup_pintu untuk menutup pintu \n";
      welcome += "/cek_pintu untuk cek kondisi pintu \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/buka_pintu") {
      bot.sendMessage(chat_id, "Pintu Telah diBuka!", "");
      relayState = HIGH;
      digitalWrite(relayPin, relayState);
      
      // Check if the sensor detects the door is closed
      if (digitalRead(sensor) == LOW) {
        bot.sendMessage(chat_id, "Pintu telah tertutup kembali", "");
        relayState = LOW;
        digitalWrite(relayPin, relayState);
      }
    }
    
    if (text == "/tutup_pintu") {
      bot.sendMessage(chat_id, "Pintu telah diTutup!", "");
      relayState = LOW;
      digitalWrite(relayPin, relayState);
    }
    
    if (text == "/cek_pintu") {
      if (digitalRead(sensor) == HIGH){
        bot.sendMessage(chat_id, "Pintu sedang terbuka!", "");
      }
      else{
        bot.sendMessage(chat_id, "Pintu sedang tertutup!", "");
      }
    }
  }
}

void magnetic_door(){
  doorState = digitalRead(sensor);
  if (doorState != lastDoorState) {
    if (doorState == LOW) {
      bot.sendMessage(chat_id, "Pintu telah tertutup kembali", "");
    }
    lastDoorState = doorState;
  }
}

void setup() {
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
  Serial.println(WiFi.localIP());
}

void loop() {
  magnetic_door();
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  if(relayState == LOW && doorState == 1 && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)){  
    bot.sendMessage(chat_id, "Pintu Dibuka Paksa", "");
    lastForcedOpenMsgTime = millis();
  }
}
