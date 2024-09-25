#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

// Ganti dengan kredensial jaringan Anda
const char* ssid = "SmartDoor";
const char* password = "12341234";

// Inisialisasi Telegram BOT
#define BOTtoken "7345692542:AAFvBg9diwYwYw38rHgwrH0r3JYxhodqwv4"

// Array ID chat yang diotorisasi
const String CHAT_IDS[] = {
  "7214692262",  // Jehuda Done
  "1619253814",  // Mami Done
  "727857551",   // ko Johan
  "1516484328",  // ko Marhadi Done
  "266029748",   // ko Cendy Done
  "884465995",   // ko Bagas Done
  "6322703298",  // ko Tengku Done
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

volatile bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

String chat_id;
String lastOpenedByChatId = "";

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
  welcome += "Gunakan /start untuk memulai\n";
  
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], welcome, "");
    Serial.println("ID Chat " + CHAT_IDS[i] + " berhasil tersambung");
  }
  Serial.println("Pesan selamat datang telah dikirim ke semua pengguna yang diotorisasi");
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    if (!isAuthorized(chat_id)) {
      bot.sendMessage(chat_id, "Pengguna tidak diotorisasi", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println("ID Chat: " + chat_id + " memerintahkan: " + text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Selamat datang, " + from_name + ".\n";
      welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu.\n\n";
      welcome += "/buka_pintu untuk membuka kunci pintu \n";
      welcome += "/kunci_pintu untuk mengunci pintu \n";
      welcome += "/status_pintu untuk memeriksa kondisi pintu \n";
      welcome += "/status_kunci untuk memeriksa status kunci pintu \n";
      bot.sendMessage(chat_id, welcome, "");
      Serial.println("Pesan selamat datang dikirim ke ID Chat: " + chat_id);
    }

    if (text == "/buka_pintu") {
      bot.sendMessage(chat_id, "Kunci pintu telah dibuka", "");
      relayState = HIGH;
      doorOpenedByBot = true;
      lastOpenedByChatId = chat_id;
      digitalWrite(relayPin, relayState);
      lastForcedOpenMsgTime = millis();
      Serial.println("Pintu dibuka oleh ID Chat: " + chat_id);
    }

    if (text == "/kunci_pintu") {
      bot.sendMessage(chat_id, "Pintu telah dikunci", "");
      relayState = LOW;
      digitalWrite(relayPin, relayState);
      doorClosedByBot = true;
      Serial.println("Pintu dikunci oleh ID Chat: " + chat_id);
    }

    if (text == "/status_pintu") {
      if (digitalRead(sensor) == HIGH) {
        bot.sendMessage(chat_id, "Pintu sedang terbuka", "");
        Serial.println("Status pintu terbuka dikirim ke ID Chat: " + chat_id);
      } else {
        bot.sendMessage(chat_id, "Pintu sedang tertutup", "");
        Serial.println("Status pintu tertutup dikirim ke ID Chat: " + chat_id);
      }
    }

    if (text == "/status_kunci") {
      if (relayState == HIGH) {
        bot.sendMessage(chat_id, "Kunci pintu sedang terbuka", "");
        Serial.println("Status kunci terbuka dikirim ke ID Chat: " + chat_id);
      } else {
        bot.sendMessage(chat_id, "Pintu sedang terkunci", "");
        Serial.println("Status kunci tertutup dikirim ke ID Chat: " + chat_id);
      }
    }
  }
}

void notifyAllUsers(String message) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], message, "");
    Serial.println("Notifikasi dikirim ke ID Chat: " + CHAT_IDS[i]);
  }
}

String getFormattedTime() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char buffer[64];
  const char* hari[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
  const char* bulan[] = {"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
  sprintf(buffer, "%s, %02d %s %04d, Jam: %02d:%02d:%02d", 
          hari[p_tm->tm_wday], 
          p_tm->tm_mday, 
          bulan[p_tm->tm_mon], 
          1900 + p_tm->tm_year,
          p_tm->tm_hour,
          p_tm->tm_min,
          p_tm->tm_sec);
  return String(buffer);
}

void magnetic_door() {
  doorState = digitalRead(sensor);
  if (doorState != lastDoorState) {
    if (doorState == LOW && (doorOpenedByBot || doorOpenedByButton || forcedOpenDetected)) {
      String timeStr = getFormattedTime();
      String message = "Terima kasih telah menutup pintu kembali pada " + timeStr;
      if (forcedOpenDetected) {
        message = "Pintu telah ditutup kembali pada " + timeStr;
        notifyAllUsers(message);
        Serial.println("Notifikasi pintu paksa ditutup dikirim ke semua pengguna");
      } else {
        bot.sendMessage(lastOpenedByChatId, message, "");
        Serial.println("Pesan terima kasih dikirim ke ID Chat: " + lastOpenedByChatId);
      }
      Serial.println("Pintu ditutup pada: " + timeStr);
      doorOpenedByBot = false;
      doorOpenedByButton = false;
      forcedOpenDetected = false;
      doorClosedByBot = true;
      lastOpenedByChatId = "";
    }
    lastDoorState = doorState;
  }
}

void IRAM_ATTR buttonInterrupt() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonPressed = true;
    lastDebounceTime = millis();
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
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi..");
  }
  Serial.println("Terhubung ke WiFi");
  Serial.println("Alamat IP: " + WiFi.localIP().toString());

  if (bot.getMe()) {
    Serial.println("Bot berhasil terhubung ke Telegram!");
    sendWelcomeMessage();
  } else {
    Serial.println("Gagal terhubung ke Telegram.");
    sendWelcomeMessage();
  }

  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi kehilangan koneksi. Menghubungkan kembali...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Menghubungkan kembali ke WiFi...");
    }
    Serial.println("Terhubung kembali ke WiFi");
  }

  if (buttonPressed) {
    buttonPressed = false;
    if (!relayActivatedByButton) {
      relayActivatedByButton = true;
      buttonPressTime = millis();
      digitalWrite(relayPin, HIGH);
      relayState = HIGH;
      doorOpenedByButton = true;
      Serial.println("Tombol ditekan, relay diaktifkan");
    }
  }

  if (relayActivatedByButton && millis() - buttonPressTime >= relayActivationTime) {
    digitalWrite(relayPin, LOW);
    relayState = LOW;
    relayActivatedByButton = false;
    Serial.println("Relay dinonaktifkan setelah tombol ditekan");
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
    if (!lastOpenedByChatId.isEmpty()) {
      bot.sendMessage(lastOpenedByChatId, "Jangan lupa untuk menutup pintu!", "");
      Serial.println("Peringatan menutup pintu dikirim ke ID Chat: " + lastOpenedByChatId);
    }
    doorClosedByBot = true;
    messageSent = true;
  }

  if (digitalRead(sensor) == HIGH && relayActivatedByButton) {
    digitalWrite(relayPin, LOW);
    relayState = LOW;
    relayActivatedByButton = false;
    doorOpenedByButton = true;
    Serial.println("Sensor HIGH, relay dinonaktifkan");
  }

  magnetic_door();

  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && !doorOpenedByButton && !doorOpenedByBot && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    String timeStr = getFormattedTime();
    String message = "Pintu dibuka paksa pada " + timeStr + "! Harap segera periksa kondisi pintu!";
    notifyAllUsers(message);
    Serial.println("Pintu dibuka paksa pada: " + timeStr);
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