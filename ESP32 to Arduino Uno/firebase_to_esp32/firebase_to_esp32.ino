#include <WiFi.h>
#include <FirebaseESP32.h>

// Informasi WiFi
#define WIFI_SSID "TOMMY_GAS"      // Ganti dengan nama WiFi kamu
#define WIFI_PASSWORD "SHENG_TAN" // Ganti dengan password WiFi kamu

// Informasi Firebase
#define FIREBASE_HOST "https://controlrelay-29d5d-default-rtdb.asia-southeast1.firebasedatabase.app"  // Ganti dengan URL Database Firebase kamu
#define FIREBASE_AUTH "AIzaSyDGC8U0v1U8aN10q7Gf20rKxo319L23KzQ"         // Ganti dengan API Key Firebase kamu

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

void setup() {
  // Menginisialisasi Serial Monitor
  Serial.begin(115200);

  // Menghubungkan ke WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Terhubung ke WiFi!");

  // Konfigurasi Firebase
  config.host = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;

  // Menghubungkan ke Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Cek koneksi Firebase
  if (Firebase.ready()) {
    Serial.println("ESP32 berhasil terhubung ke Firebase!");
  } else {
    Serial.println("Gagal terhubung ke Firebase!");
  }
}

void loop() {
  // Tidak ada kode di loop untuk contoh ini
}
