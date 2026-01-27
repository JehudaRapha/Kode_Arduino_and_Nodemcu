  #include <WiFi.h>
  #include <HTTPClient.h>

  // === Pin RX/TX Komunikasi dengan Arduino ===
  #define RXD2 25  // RX → Arduino TX
  #define TXD2 26  // TX → Arduino RX

  // === Variabel Global ===
  int peopleCount = 0;
  const int maxPeople = 15;
  bool wifiConnected = false;
  bool isFull = false; // status ruangan penuh
  unsigned long lastUpdate = 0;
  const unsigned long updateCooldown = 2000; // jeda minimal antar update

  // === WiFi & API ===
  const char* ssid = "TOMMY GAS";
  const char* password = "SHENG_TAN";
  const char* apiEndpoint = "http://192.168.0.106:8000/api/sensors";


  void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

    // --- Koneksi WiFi ---
    Serial.println("Menghubungkan ke WiFi...");
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.println("✓ WiFi terhubung!");
      Serial.print("IP ESP32: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("✗ Gagal koneksi WiFi, mode offline aktif.");
    }

    // --- Tes koneksi ke API ---
    if (wifiConnected) {
      Serial.println("🌐 Mengecek koneksi ke API...");
      if (testAPIConnection()) {
        Serial.println("✅ API Terhubung!");
      } else {
        Serial.println("⚠️ Gagal terhubung ke API!");
      }
    }

    Serial.println("ESP32 siap menerima data dari Arduino.");
  }

  void loop() {
    // ==== Baca data dari Arduino ====
    if (Serial2.available()) {
      String data = Serial2.readStringUntil('\n');
      data.trim();

      if (data.length() > 0) {
        Serial.print("Data diterima dari Arduino: ");
        Serial.println(data);

        // Parsing data
        if (data.startsWith("U1=1")) {
          handlePersonIn();
        } else if (data.startsWith("HW1=1")) {
          handlePersonOut();
        }
      }
    }

    // ==== Kirim status FULL ke Arduino secara otomatis ====
    // (jaga agar servo tetap terkunci kalau masih penuh)
    static unsigned long lastFullCheck = 0;
    if (millis() - lastFullCheck > 5000) { // setiap 5 detik cek ulang status
      lastFullCheck = millis();

      if (isFull) {
        Serial.println("🔒 Ruangan masih penuh, kirim ulang FULL=1 ke Arduino.");
        Serial2.println("FULL=1");
      } else {
        Serial.println("🔓 Ruangan tidak penuh, kirim ulang FULL=0 ke Arduino.");
        Serial2.println("FULL=0");
      }
    }
  }

  // ===== Fungsi: Orang Masuk =====
  void handlePersonIn() {
    unsigned long now = millis();
    if (now - lastUpdate < updateCooldown) return;

    if (peopleCount < maxPeople) {
      peopleCount++;
      Serial.print("🟢 Orang MASUK. Total: ");
      Serial.println(peopleCount);

      sendCountToAPI();
      lastUpdate = now;

      // Jika baru mencapai batas penuh → kirim FULL=1
      if (peopleCount >= maxPeople && !isFull) {
        isFull = true;
        Serial.println("🚨 Ruangan penuh! Kirim FULL=1 ke Arduino.");
        Serial2.println("FULL=1");
      }
    } else {
      Serial.println("⚠️ Ruangan sudah penuh! Kirim FULL=1 ke Arduino lagi (blokir pintu).");
      Serial2.println("FULL=1");
      delay(1000);
    }
  }

  // ===== Fungsi: Orang Keluar =====
  void handlePersonOut() {
    unsigned long now = millis();
    if (now - lastUpdate < updateCooldown) return;

    if (peopleCount > 0) {
      peopleCount--;
      Serial.print("🔵 Orang KELUAR. Total: ");
      Serial.println(peopleCount);

      sendCountToAPI();
      lastUpdate = now;

      // Jika sebelumnya penuh lalu sekarang tidak penuh lagi → kirim FULL=0
      if (isFull && peopleCount < maxPeople) {
        isFull = false;
        Serial.println("✅ Ruangan tidak penuh lagi, kirim FULL=0 ke Arduino.");
        Serial2.println("FULL=0");
      }
    }
  }

  // ===== Kirim data jumlah orang ke API =====
  void sendCountToAPI() {
    if (!wifiConnected) {
      Serial.println("⚠️ WiFi tidak terhubung, data tidak dikirim ke API.");
      return;
    }

    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"sensor_name\":\"jumlah\",\"value\":" + String(peopleCount) + "}";

    Serial.println("🌐 Mengirim data ke API...");
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("✅ API merespon, kode: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("❌ Gagal mengirim ke API. Error: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  }

  // ===== Tes koneksi API di awal =====
  bool testAPIConnection() {
    HTTPClient http;
    http.begin(apiEndpoint);
    int httpResponseCode = http.GET();
    http.end();

    return (httpResponseCode == 200 || httpResponseCode == 201);
  }
