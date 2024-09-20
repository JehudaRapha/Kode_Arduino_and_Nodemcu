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

// Array untuk ID chat yang diotorisasi
const String CHAT_IDS[] = {
  "7214692262",  // Jehuda Done
  "1166902768",  // ka Wida Done
  "1516484328",  // ko Marhadi 
  "727857551",   // ko Johan
  "266029748",   // ko Cendy
  "884465995",   // ko Bagas Done
  "6322703298",  // ko Tengku Done
};
const int NUM_CHAT_IDS = sizeof(CHAT_IDS) / sizeof(CHAT_IDS[0]);

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT); // Sertifikat untuk ESP8266
#endif

WiFiClientSecure client; // Client untuk koneksi WiFi aman
UniversalTelegramBot bot(BOTtoken, client); // Inisialisasi bot telegram

int botRequestDelay = 3000; // Delay waktu untuk polling bot
unsigned long lastTimeBotRan; // Waktu terakhir bot berjalan
unsigned long lastForcedOpenMsgTime = 0; // Waktu terakhir pesan pintu paksa
int forcedOpenMsgInterval = 5000; // Interval untuk pesan pintu paksa
unsigned long lastForceOpenMessageTime = 0; // Waktu terakhir pesan pintu paksa dikirim

const int relayPin = D2; // Pin untuk relay
const int buttonPin = D3;  // Pin baru untuk tombol
const int sensor = D5; // Pin untuk sensor pintu

bool relayState = LOW; // Status relay
int doorState; // Status pintu
int lastDoorState = -1; // Status pintu terakhir
bool doorOpenedByBot = false; // Apakah pintu dibuka oleh bot
bool doorClosedByBot = false; // Apakah pintu ditutup oleh bot
bool messageSent = false; // Status pesan terkirim

// Variabel baru untuk kontrol tombol
unsigned long buttonPressTime = 0; // Waktu tombol ditekan
const unsigned long relayActivationTime = 15000;  // 15 detik relay aktif
bool relayActivatedByButton = false; // Status relay diaktifkan oleh tombol
bool doorOpenedByButton = false; // Apakah pintu dibuka oleh tombol

String chat_id; // ID chat aktif

// Fungsi untuk mengecek apakah pengguna diotorisasi
bool isAuthorized(String chat_id) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    if (chat_id == CHAT_IDS[i]) {
      return true;
    }
  }
  return false;
}

// Fungsi untuk mengirim pesan sambutan ke pengguna
void sendWelcomeMessage() {
  String welcome = "Bot telah terhubung ke Telegram.\n";
  welcome += "/start untuk memulai bot\n";
  
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], welcome, ""); // Kirim pesan ke semua pengguna yang diotorisasi
  }
  Serial.println("Sent welcome message to all authorized users");
}

// Fungsi untuk menangani pesan baru yang diterima bot
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id); // Ambil chat ID
    if (!isAuthorized(chat_id)) { // Cek apakah chat ID diotorisasi
      bot.sendMessage(chat_id, "Unauthorized user", ""); // Kirim pesan jika tidak diotorisasi
      continue;
    }

    String text = bot.messages[i].text; // Ambil teks pesan
    Serial.println(text);

    String from_name = bot.messages[i].from_name; // Nama pengirim

    if (text == "/start") { // Jika perintah "/start" diterima
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Gunakan perintah berikut untuk mengontrol kunci pintu.\n\n";
      welcome += "/buka_pintu untuk membuka kunci pintu \n";
      welcome += "/tutup_pintu untuk menutup kunci pintu \n";
      welcome += "/status_pintu untuk cek kondisi pintu \n";
      welcome += "/status_kunci untuk cek status kunci pintu \n"; // Tambahkan perintah baru
      bot.sendMessage(chat_id, welcome, ""); // Kirim pesan sambutan
    }

    if (text == "/buka_pintu") { // Jika perintah membuka pintu diterima
      bot.sendMessage(chat_id, "Kunci pintu telah terbuka", "");
      relayState = HIGH; // Aktifkan relay
      doorOpenedByBot = true; // Tandai pintu dibuka oleh bot
      digitalWrite(relayPin, relayState); // Nyalakan relay
      lastForcedOpenMsgTime = millis(); // Atur waktu pesan terakhir
    }

    if (text == "/tutup_pintu") { // Jika perintah menutup pintu diterima
      bot.sendMessage(chat_id, "Kunci pintu telah terkunci", "");
      relayState = LOW; // Matikan relay
      digitalWrite(relayPin, relayState); // Matikan relay
      doorClosedByBot = true; // Tandai pintu ditutup oleh bot
    }

    if (text == "/status_pintu") { // Jika perintah cek status pintu diterima
      if (digitalRead(sensor) == HIGH) {
        bot.sendMessage(chat_id, "Pintu sedang terbuka", ""); // Kirim status pintu terbuka
      } else {
        bot.sendMessage(chat_id, "Pintu sedang tertutup", ""); // Kirim status pintu tertutup
      }
    }

    // Perintah baru untuk mengecek status kunci pintu (relay)
    if (text == "/status_kunci") {
      if (relayState == HIGH) {
        bot.sendMessage(chat_id, "Kunci pintu sedang terbuka", "");
      } else {
        bot.sendMessage(chat_id, "Kunci pintu sedang terkunci", "");
      }
    }
  }
}

// Fungsi untuk mengirim pesan ke semua pengguna yang diotorisasi
void notifyAllUsers(String message) {
  for (int i = 0; i < NUM_CHAT_IDS; i++) {
    bot.sendMessage(CHAT_IDS[i], message, ""); // Kirim pesan
  }
}

// Fungsi untuk menangani perubahan status pintu
void magnetic_door() {
  doorState = digitalRead(sensor); // Baca status sensor pintu
  if (doorState != lastDoorState) { // Jika status pintu berubah
    if (doorState == LOW) {
      bot.sendMessage(chat_id, "Terimakasih telah menutup pintu kembali!", ""); // Kirim pesan pintu tertutup
      doorOpenedByBot = false; // Reset status pintu dibuka oleh bot
      doorClosedByBot = true; // Tandai pintu ditutup oleh bot
    }
    lastDoorState = doorState; // Simpan status pintu terakhir
  }
}

// Setup awal sistem
void setup() {
  Serial.begin(115200); // Mulai serial

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org"); // Sinkronisasi waktu NTP untuk ESP8266
  client.setTrustAnchors(&cert); // Setel sertifikat untuk HTTPS
#endif

  pinMode(relayPin, OUTPUT); // Atur pin relay sebagai output
  digitalWrite(relayPin, relayState); // Setel status relay awal
  pinMode(sensor, INPUT_PULLUP); // Atur sensor pintu sebagai input dengan pull-up
  pinMode(buttonPin, INPUT_PULLUP);  // Atur pin tombol sebagai input dengan pull-up

  WiFi.mode(WIFI_STA); // Atur mode WiFi sebagai station
  WiFi.begin(ssid, password); // Mulai koneksi WiFi
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Setel sertifikat untuk HTTPS (ESP32)
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi.."); // Cetak pesan saat menghubungkan ke WiFi
  }
  Serial.println("Connected to WiFi"); // Cetak pesan jika terhubung ke WiFi
  Serial.println("IP Address: " + WiFi.localIP().toString()); // Tampilkan alamat IP

  if (bot.getMe()) { // Cek apakah bot terhubung
    Serial.println("Bot is connected to Telegram successfully!"); // Pesan jika berhasil
    sendWelcomeMessage(); // Kirim pesan sambutan
  } else {
    Serial.println("Failed to connect to Telegram."); // Pesan jika gagal
    sendWelcomeMessage(); // Kirim pesan sambutan
  }
}

// Loop utama
void loop() {
  if (WiFi.status() != WL_CONNECTED) { // Cek status WiFi
    Serial.println("WiFi lost connection. Reconnecting..."); // Cetak pesan jika koneksi hilang
    WiFi.begin(ssid, password); // Mulai ulang koneksi WiFi
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Reconnecting to WiFi..."); // Pesan saat menghubungkan ulang
    }
    Serial.println("Reconnected to WiFi"); // Cetak pesan jika berhasil terhubung ulang
  }

  // Menangani jika tombol ditekan
  if (digitalRead(buttonPin) == LOW && !relayActivatedByButton) {
    relayActivatedByButton = true; // Tandai relay diaktifkan oleh tombol
    buttonPressTime = millis(); // Simpan waktu tombol ditekan
    digitalWrite(relayPin, HIGH); // Nyalakan relay
    relayState = HIGH; // Update status relay
    Serial.println("Button pressed, relay activated"); // Cetak pesan
  }

  // Cek apakah waktu relay aktif sudah habis setelah tombol ditekan
  if (relayActivatedByButton && millis() - buttonPressTime >= relayActivationTime) {
    digitalWrite(relayPin, LOW); // Matikan relay
    relayState = LOW; // Update status relay
    relayActivatedByButton = false; // Reset status relay tombol
    Serial.println("Relay deactivated after button press"); // Cetak pesan
  }

  if (millis() > lastTimeBotRan + botRequestDelay) { // Cek interval polling bot
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1); // Ambil pesan baru

    if (numNewMessages > 0) {
      handleNewMessages(numNewMessages); // Tangani pesan baru jika ada
    }
    lastTimeBotRan = millis(); // Update waktu terakhir bot berjalan
  }

  // Otomatis tutup pintu setelah 5 detik jika relay aktif dan pintu tertutup
  if (relayState == HIGH && digitalRead(sensor) == HIGH && (millis() - lastForcedOpenMsgTime > forcedOpenMsgInterval)) {
    relayState = LOW; // Matikan relay
    digitalWrite(relayPin, relayState); // Matikan relay
    bot.sendMessage(chat_id, "Jangan lupa tutup pintu lagi ya!", ""); // Kirim pesan pengingat
    doorClosedByBot = true; // Tandai pintu ditutup oleh bot
    messageSent = true; // Tandai pesan sudah dikirim
  }

  // Logika baru: jika sensor terbuka, matikan relay jika diaktifkan oleh tombol
  if (digitalRead(sensor) == HIGH && relayActivatedByButton) {
    digitalWrite(relayPin, LOW); // Matikan relay
    relayState = LOW; // Update status relay
    relayActivatedByButton = false; // Reset status relay tombol
    doorOpenedByButton = true;  // Tandai pintu dibuka oleh tombol
    Serial.println("Sensor HIGH, relay deactivated"); // Cetak pesan
  }

  magnetic_door();

  // Ketika ada yang membuka pintu secara button maupun bot, maka akan mengirimkan pesan darurat
  if (digitalRead(sensor) == HIGH && relayState == LOW && !messageSent && !doorOpenedByButton && (millis() - lastForceOpenMessageTime > forcedOpenMsgInterval)) {
    notifyAllUsers("Pintu dibuka paksa!, diharapkan untuk periksa kondisi pintu sekarang!"); // Kirim pesan ke semua pengguna yang diotorisasi
    lastForceOpenMessageTime = millis(); // Update waktu pesan terakhir
    messageSent = true;  // Set flag agar pesan tidak terkirim terus-menerus
    doorClosedByBot = false; // Reset status pintu ditutup oleh bot
  }

  // Reset flag saat pintu tertutup dengan benar
  if (digitalRead(sensor) == LOW) {
    doorOpenedByButton = false;  // Reset flag pintu dibuka oleh tombol
    messageSent = false; // Reset status pesan terkirim
    doorClosedByBot = false; // Reset status pintu ditutup oleh bot
  }
}