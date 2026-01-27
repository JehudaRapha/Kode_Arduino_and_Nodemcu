#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Preferences.h>

// ================================
//           CONFIG
// ================================
#define RXD2 25
#define TXD2 26

// WiFi dan API
const char* ssid = "SatuConnect-Event";
const char* password = "univ11univ";
const char* apiEndpoint = "http://10.103.26.148:8000/api/sensors";

int peopleCount = 0;
const int maxPeople = 15;          // Batas maksimal orang di ruangan
const int reopenThreshold = 10;    // Jika orang turun di bawah ini, ruangan dibuka
bool roomLocked = false;           // Status ruangan terkunci
bool processingEvent = false;      // Status sedang memproses event

WebServer server(80);              // HTTP server
Preferences prefs;                 // Penyimpanan NVS


// ================================
//       NVS HELPERS
// ================================
String idKey(int id) {
  return "id_" + String(id);
}

// Simpan status (inside/outside) ke NVS
void storage_set_status(int id, const String& status) {
  String k = idKey(id);
  Serial.printf("[NVS] SET  | %s → %s\n", k.c_str(), status.c_str());
  prefs.putString(k.c_str(), status);
}

// Ambil status dari NVS
String storage_get_status(int id) {
  String k = idKey(id);
  if (!prefs.isKey(k.c_str())) {
    Serial.printf("[NVS] MISS | %s belum ada → 'unknown'\n", k.c_str());
    return "unknown";
  }
  String s = prefs.getString(k.c_str(), "outside");
  Serial.printf("[NVS] GET  | %s → %s\n", k.c_str(), s.c_str());
  return s;
}

// Cek apakah ID sudah tersimpan
bool storage_has_id(int id) {
  return prefs.isKey(idKey(id).c_str());
}


// ================================
//       API COMMUNICATION
// ================================
int getServerPeopleCount(bool verbose = true) {
  if (WiFi.status() != WL_CONNECTED) {
    if (verbose) Serial.println("[WiFi] Tidak terhubung → skip cek server.");
    return -1;
  }

  HTTPClient http;
  http.setTimeout(5000);
  http.begin(String(apiEndpoint) + "?latest=true");

  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);
    if (!deserializeJson(doc, payload)) {
      int count = doc["summary"]["orang_di_dalam"] | -1;
      http.end();
      return count;
    }
  }
  http.end();
  return -1;
}

// Sinkronisasi peopleCount dari server
bool fetchPeopleCountFromAPI() {
  for (int attempt = 0; attempt < 5; attempt++) {
    int srv = getServerPeopleCount();
    if (srv >= 0) {
      peopleCount = srv;
      Serial.printf("[SYNC] People count diperbarui dari server → %d\n", peopleCount);
      Serial2.println("PEOPLE_IN:" + String(peopleCount));
      return true;
    }
    delay(2000);
  }

  Serial.println("[SYNC] Gagal sync. Set peopleCount = 0");
  peopleCount = 0;
  Serial2.println("PEOPLE_IN:0");
  return false;
}

// Kirim event ke server API (masuk/keluar)
bool sendToAPI(String sensorName) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[API] Gagal → WiFi tidak terhubung.");
    return false;
  }

  HTTPClient http;
  http.setTimeout(5000);
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"sensor_name\":\"" + sensorName + "\",\"value\":1}";
  Serial.printf("[API] POST (%s) → %s\n", sensorName.c_str(), jsonPayload.c_str());

  int code = http.POST(jsonPayload);
  bool ok = (code == 200 || code == 201);
  http.end();

  if (ok) {
    Serial.println("[API] OK (200/201)");
    return true;
  }

  Serial.println("[API] Timeout → cek server secara polling...");
  int expectedAfter = peopleCount + (sensorName == "masuk" ? 1 : -1);

  for (int i = 0; i < 3; i++) {
    delay(800);
    int srvCount = getServerPeopleCount(false);
    if (srvCount == expectedAfter) {
      Serial.println("[API] Polling sukses, count sesuai server.");
      return true;
    }
  }

  Serial.println("[API] Polling gagal.");
  return false;
}


// ================================
//           HTTP HANDLERS
// ================================
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}

void handleNotFound() {
  server.send(404, "application/json", "{\"ok\":false,\"error\":\"not found\"}");
}

// Handle request masuk ke endpoint /event
void handleEvent() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

  String body = server.arg("plain");
  Serial.println("\n===== [HTTP EVENT] Request Masuk =====");
  Serial.println(body);

  if (body.length() == 0) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"empty body\"}");
    return;
  }

  DynamicJsonDocument doc(2048);
  if (deserializeJson(doc, body)) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid json\"}");
    return;
  }

  String event = doc["event"] | "";
  event.trim();
  Serial.printf("[EVENT] Jenis event: %s\n", event.c_str());

  // ---------- register_success ----------
  if (event == "register_success") {
    Serial.println("[EVENT] register_success → kirim ke Arduino");
    Serial2.println("REGISTER_SUCCESS");
    server.send(200, "application/json", "{\"ok\":true}");
    return;
  }

  // ---------- REGISTER ----------
  if (event == "register") {
    int id = doc["id"] | -1;
    Serial.printf("[EVENT] REGISTER id=%d\n", id);
    if (!storage_has_id(id)) {
      storage_set_status(id, "outside");
      Serial.println("[REGISTER] ID baru, status → outside");
    }
    server.send(200, "application/json", "{\"ok\":true,\"status\":\"outside\"}");
    return;
  }

  // ---------- QUERY ----------
  if (event == "query") {
    int id = doc["id"] | -1;
    String cur = storage_get_status(id);
    Serial.printf("[EVENT] QUERY id=%d → %s\n", id, cur.c_str());

    if (cur == "unknown") {
      server.send(200, "application/json", "{\"ok\":true,\"status\":\"outside\",\"registered\":false}");
    } else {
      server.send(200, "application/json", "{\"ok\":true,\"status\":\"" + cur + "\",\"registered\":true}");
    }
    return;
  }

  // ---------- MASUK / KELUAR ----------
  if (event != "masuk" && event != "keluar") {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"unsupported event\"}");
    return;
  }

  int payload_id = doc["id"] | -1;

  if (processingEvent) {
    server.send(429, "application/json", "{\"ok\":false,\"error\":\"busy\"}");
    return;
  }
  processingEvent = true;

  // ---- MASUK ----
  if (event == "masuk") {
    Serial.println("[EVENT] Permintaan MASUK diterima.");

    if (roomLocked || peopleCount >= maxPeople) {
      Serial.println("[MASUK] DITOLAK → Ruangan penuh.");
      Serial2.println("API_FAIL");
      server.send(409, "application/json", "{\"ok\":false,\"error\":\"room_full\"}");
      processingEvent = false;
      return;
    }

    bool ok = sendToAPI("masuk");

    if (ok) {
      peopleCount++;
      Serial.printf("[MASUK] OK. People = %d\n", peopleCount);
      Serial2.println("API_OK_MASUK");

      if (peopleCount >= maxPeople) {
        roomLocked = true;
        Serial.println("[ROOM] Ruangan mencapai limit → Dikunci.");
      }

      if (payload_id >= 0) storage_set_status(payload_id, "inside");

      server.send(200, "application/json", "{\"ok\":true,\"action\":\"open\",\"peopleCount\":" + String(peopleCount) + "}");
    } else {
      Serial2.println("API_FAIL");
      server.send(500, "application/json", "{\"ok\":false,\"error\":\"api_failed\"}");
    }
  }

  // ---- KELUAR ----
  else if (event == "keluar") {
    Serial.println("[EVENT] Permintaan KELUAR diterima.");

    if (peopleCount > 0) {
      bool ok = sendToAPI("keluar");

      if (ok) {
        peopleCount--;
        Serial.printf("[KELUAR] OK. People = %d\n", peopleCount);
        Serial2.println("API_OK_KELUAR");

        if (roomLocked && peopleCount <= reopenThreshold) {
          roomLocked = false;
          Serial.println("[ROOM] Dibuka kembali.");
        }

        if (payload_id >= 0) storage_set_status(payload_id, "outside");

        server.send(200, "application/json", "{\"ok\":true,\"action\":\"close\",\"peopleCount\":" + String(peopleCount) + "}");
      } else {
        Serial2.println("API_FAIL");
        server.send(500, "application/json", "{\"ok\":false,\"error\":\"api_failed\"}");
      }
    } else {
      server.send(400, "application/json", "{\"ok\":false,\"error\":\"no_people\"}");
    }
  }

  processingEvent = false;
}


// ================================
//             SETUP
// ================================
void setup() {
  Serial.begin(115200);                            // Serial monitor
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);    // Serial ke Arduino

  prefs.begin("faceStatus", false);               // Init NVS

  Serial.println("\n===== STARTING ESP32 SYSTEM =====");
  Serial.println("[WiFi] Connecting...");

  WiFi.begin(ssid, password);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    Serial2.println("BUZZER_READY");              // Beri tahu Arduino siap
    fetchPeopleCountFromAPI();                     // Sync awal
  } else {
    Serial.println("[WiFi] Gagal connect.");
    peopleCount = 0;
    Serial2.println("PEOPLE_IN:0");
  }

  // HTTP server routes
  server.on("/event", HTTP_POST, handleEvent);
  server.on("/event", HTTP_OPTIONS, handleOptions);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("[HTTP] Server ready on port 80.");
}


// ================================
//             LOOP
// ================================
void loop() {
  server.handleClient();  // Handle request HTTP

  // ----- Serial dari Arduino -----
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();

    if (data.length() == 0) return;

    Serial.printf("[UART] Dari Arduino: %s\n", data.c_str());

    if (!processingEvent) {
      processingEvent = true;

      // --- Arduino minta MASUK ---
      if (data == "REQ_MASUK") {
        Serial.println("[REQ] Arduino minta MASUK.");
        if (roomLocked || peopleCount >= maxPeople) {
          Serial.println("[REQ_MASUK] Ditolak → Ruangan penuh.");
          Serial2.println("API_FAIL");
        } else {
          bool ok = sendToAPI("masuk");
          if (ok) {
            peopleCount++;
            Serial2.println("API_OK_MASUK");
            if (peopleCount >= maxPeople) roomLocked = true;
          } else {
            Serial2.println("API_FAIL");
          }
        }
      }

      // --- Arduino minta KELUAR ---
      else if (data == "REQ_KELUAR") {
        Serial.println("[REQ] Arduino minta KELUAR.");
        if (peopleCount > 0) {
          bool ok = sendToAPI("keluar");
          if (ok) {
            peopleCount--;
            Serial2.println("API_OK_KELUAR");
            if (roomLocked && peopleCount <= reopenThreshold) roomLocked = false;
          } else {
            Serial2.println("API_FAIL");
          }
        } else {
          Serial2.println("API_FAIL");
        }
      }

      processingEvent = false;
    }
  }

  delay(20);  // small delay to avoid CPU overload
}
