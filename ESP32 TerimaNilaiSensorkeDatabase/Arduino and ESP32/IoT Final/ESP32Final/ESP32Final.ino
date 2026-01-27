#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define RXD2 25  // RX → Arduino TX
#define TXD2 26  // TX → Arduino RX

// === WiFi dan API ===
const char* ssid = "SatuConnect-Event";
const char* password = "univ11univ";
const char* apiEndpoint = "http://10.103.48.28:8000/api/sensors";

int peopleCount = 0;        // jumlah orang di dalam
const int maxPeople = 15;
bool processingEvent = false; // guard supaya satu event diproses sekaligus

// ===== Util: Ambil count terbaru dari server =====
int getServerPeopleCount(bool verbose = true) {
  if (WiFi.status() != WL_CONNECTED) {
    if (verbose) Serial.println("⚠️ WiFi tidak terhubung, tidak bisa cek server.");
    return -1;
  }

  HTTPClient http;
  http.setTimeout(5000); // timeout 5 detik biar tidak nunggu lama
  String url = String(apiEndpoint) + "?latest=true";
  http.begin(url);
  int code = http.GET();

  if (code == 200) {
    String payload = http.getString();
    if (verbose) {
      Serial.println("🌐 Response API (latest):");
      Serial.println(payload);
    }

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      int count = doc["summary"]["orang_di_dalam"] | -1;
      http.end();
      if (verbose) Serial.printf("🔎 Server peopleCount: %d\n", count);
      return count;
    } else {
      if (verbose) Serial.printf("❌ Gagal parse JSON: %s\n", error.c_str());
    }
  } else {
    if (verbose) Serial.printf("❌ Gagal GET API latest, code: %d\n", code);
  }

  http.end();
  return -1; // gagal dapatkan count dari server
}

// ===== Fungsi Ambil Data dari API saat Startup dengan Retry =====
bool fetchPeopleCountFromAPI() {
  int maxRetries = 5;
  for (int attempt = 0; attempt < maxRetries; attempt++) {
    int srv = getServerPeopleCount(); // verbose = true
    if (srv >= 0) {
      peopleCount = srv;
      Serial.printf("✅ Sinkron dari server. peopleCount = %d\n", peopleCount);

      // Kirim ke Arduino
      Serial2.println("PEOPLE_IN:" + String(peopleCount));
      return true;
    }
    Serial.println("🔁 Mencoba ulang dalam 2 detik...");
    delay(2000);
  }

  Serial.println("⚠️ Gagal ambil data dari API setelah 5 percobaan. Mulai dengan 0.");
  peopleCount = 0;
  Serial2.println("PEOPLE_IN:0");
  return false;
}

// ===== Fungsi Kirim ke API (dengan cek perubahan jika gagal) =====
bool sendToAPI(String sensorName) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi putus, tidak bisa POST.");
    // Tanpa koneksi kita tidak bisa cek ke server → anggap gagal
    return false;
  }

  HTTPClient http;
  http.setTimeout(5000);
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"sensor_name\":\"" + sensorName + "\",\"value\":1}";
  Serial.println("🌐 Mengirim ke API...");
  Serial.println("Payload: " + jsonPayload);

  int httpResponseCode = http.POST(jsonPayload);
  bool success = (httpResponseCode == 200 || httpResponseCode == 201);

  if (success) {
    Serial.println("✅ Data berhasil dikirim ke API!");
    http.end();
    return true;
  } else {
    Serial.printf("❌ Gagal kirim ke API. Kode respons: %d\n", httpResponseCode);
    Serial2.println("API_FAIL");
    http.end();
  }

  // === Mitigasi duplikasi: cek apakah server sudah mencatat perubahan ===
  int expectedAfter = peopleCount + (sensorName == "masuk" ? 1 : -1);
  const int pollTimes = 3;       // berapa kali cek
  const int pollDelayMs = 800;   // jeda antar cek

  for (int i = 0; i < pollTimes; i++) {
    delay(pollDelayMs);
    int srvCount = getServerPeopleCount(false); // verbose = false
    if (srvCount == expectedAfter) {
      Serial.println("ℹ️ Server menunjukkan perubahan sesuai request walau POST gagal → anggap SUKSES.");
      return true;
    }
  }

  Serial.println("⛔ POST gagal dan tidak ada perubahan di server.");
  return false;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    Serial.print(".");
    attempt++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi Terhubung!");
    Serial.print("IP ESP32: "); Serial.println(WiFi.localIP());

    Serial2.println("BUZZER_READY");

    // Ambil peopleCount terakhir dari API dengan retry
    fetchPeopleCountFromAPI();
  } else {
    Serial.println("⚠️ Gagal konek WiFi!");
    peopleCount = 0;
    Serial2.println("PEOPLE_IN:0");
  }

  Serial.println("ESP32 siap menerima data dari Arduino...");
  Serial.println("--------------------------------------------");
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();

    if (data.length() > 0) {
      Serial.print("📥 Data dari Arduino: "); Serial.println(data);

      if (processingEvent) {
        Serial.println("⏳ Masih memproses event sebelumnya, abaikan input ini untuk mencegah duplikasi.");
        return;
      }

      processingEvent = true; // lock

      if (data == "REQ_MASUK") {
        if (peopleCount < maxPeople) {
          Serial.println("🟢 Permintaan MASUK diterima, kirim ke API...");
          bool result = sendToAPI("masuk");
          if (result) {
            peopleCount++;
            Serial.print("✅ Sukses. Total orang: "); Serial.println(peopleCount);
            Serial2.println("API_OK_MASUK");
          } else {
            Serial.println("❌ API gagal. Tidak menambah orang.");
            Serial2.println("API_FAIL");
          }
        } else {
          Serial.println("🚫 Ruangan penuh, tidak bisa masuk.");
          Serial2.println("API_FAIL");
        }
      } else if (data == "REQ_KELUAR") {
        if (peopleCount > 0) {
          Serial.println("🔵 Permintaan KELUAR diterima, kirim ke API...");
          bool result = sendToAPI("keluar");
          if (result) {
            peopleCount--;
            Serial.print("✅ Sukses. Total orang: "); Serial.println(peopleCount);
            Serial2.println("API_OK_KELUAR");
          } else {
            Serial.println("❌ API gagal. Tidak mengurangi orang.");
            Serial2.println("API_FAIL");
          }
        } else {
          Serial.println("⚠️ Tidak ada orang di dalam ruangan.");
          Serial2.println("API_FAIL");
        }
      }

      processingEvent = false; // unlock
    }
  }

  delay(50);
}