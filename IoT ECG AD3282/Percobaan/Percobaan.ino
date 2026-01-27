/**
 * ESP32 ECG Logger + Web Downloader + Lead-Off Detection
 * - Tambah LO+ (pin 14) dan LO- (pin 27)
 * - Jika elektroda terlepas → sampling berhenti otomatis
 */

#include <WiFi.h>
#include <WebServer.h>
#include "SPIFFS.h"

// ===============================
// Konfigurasi ECG
// ===============================
#define ECG_PIN 32
#define LO_PLUS 14
#define LO_MINUS 27

#define FS_HZ 128
#define DURATION_MS 60000UL

unsigned long startMs = 0;
unsigned long lastSampleUs = 0;
const unsigned long intervalUs = 1000000UL / FS_HZ;

bool samplingDone = false;
bool leadError = false;

std::vector<float> samples;

// ===============================
// WiFi router login
// ===============================
const char* ssid     = "TOMMY GAS";
const char* password = "SHENG_TAN";

WebServer server(80);

// ===============================
// WEB HANDLER
// ===============================
void handleRoot() {
  String html = "<h1>ECG File Downloader</h1><hr>";
  html += "<p>File tersimpan di SPIFFS:</p><ul>";

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file) {
    html += "<li><a href='" + String(file.name()) + "'>" + String(file.name()) +
            "</a> (" + String(file.size()) + " bytes)</li>";
    file = root.openNextFile();
  }

  html += "</ul>";
  server.send(200, "text/html", html);
}

void handleFileDownload() {
  String path = server.uri();
  File file = SPIFFS.open(path, "r");

  if (!file) {
    server.send(404, "text/plain", "File tidak ditemukan");
    return;
  }

  server.streamFile(file, "text/plain");
  file.close();
}

// ===============================
// WI-FI CONNECT
// ===============================
void connectWiFi() {
  Serial.println("[INFO] Menghubungkan ke WiFi router...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    retries++;

    if (retries > 40) {  
      Serial.println("\n[❌] Gagal connect ke WiFi. Program dihentikan!");
      while (true);
    }
  }

  Serial.println("\n[✅] WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ===============================
// SETUP
// ===============================
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);

  connectWiFi();

  if (!SPIFFS.begin(true)) {
    Serial.println("[❌] Gagal mount SPIFFS!");
  } else {
    Serial.println("[✅] SPIFFS siap.");
  }

  samples.reserve((DURATION_MS / 1000) * FS_HZ);

  startMs = millis();
  lastSampleUs = micros();
  samplingDone = false;
  leadError = false;

  Serial.println("[INFO] Mulai sampling ECG 1 menit...");
  Serial.println("[INFO] Memeriksa elektroda...");
}

// ===============================
// SAVE TO FILE
// ===============================
void saveToTxt() {
  char path[32];
  snprintf(path, sizeof(path), "/ecg_%lu.txt", millis());

  File f = SPIFFS.open(path, FILE_WRITE);
  if (!f) {
    Serial.println("[❌] Tidak bisa membuka file.");
    return;
  }

  char buf[32];
  for (float v : samples) {
    int n = snprintf(buf, sizeof(buf), "%.6f\n", v);
    f.write((const uint8_t*)buf, n);
  }

  f.close();

  Serial.printf("[✅] Disimpan: %s (%u sampel)\n", path, (unsigned)samples.size());
}

// ===============================
// LOOP
// ===============================
void loop() {

  // =======================
  // CEK LEADS
  // =======================
  if (digitalRead(LO_PLUS) == HIGH || digitalRead(LO_MINUS) == HIGH) {
    leadError = true;

    Serial.println("[❌] Lead terputus! Sampling dihentikan.");
    samplingDone = true;  // hentikan proses

    return;
  }

  // =======================
  // SAMPLING ECG
  // =======================
  if (!samplingDone) {

    unsigned long nowMs = millis();
    unsigned long nowUs = micros();

    if ((nowUs - lastSampleUs) >= intervalUs) {
      lastSampleUs += intervalUs;

      int raw = analogRead(ECG_PIN);
      float volt = (raw * 3.3f) / 4095.0f;
      samples.push_back(volt);

      Serial.println(volt, 6);
    }

    if ((nowMs - startMs) >= DURATION_MS) {
      samplingDone = true;
      Serial.println("[INFO] Durasi selesai, menyimpan...");

      if (!leadError) {
        saveToTxt();

        server.on("/", handleRoot);
        server.onNotFound(handleFileDownload);
        server.begin();

        Serial.println("[✅] Server siap!");
        Serial.print("Download via: http://");
        Serial.println(WiFi.localIP());
      }

    }

    return;
  }

  // =======================
  // WEB SERVER
  // =======================
  if (!leadError) {
    server.handleClient();
  }
}
