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

// === WiFi dan API ===
const char* ssid = "TOMMY GAS";
const char* password = "SHENG_TAN";
const char* apiEndpoint = "http://192.168.0.107:8000/api/sensors";

int peopleCount = 0;
const int maxPeople = 15;
const int reopenThreshold = 10;
bool roomLocked = false;
bool processingEvent = false;

WebServer server(80);
Preferences prefs;

// ================================
//       NVS HELPERS
// ================================
String idKey(int id) {
  return "id_" + String(id);
}

void storage_set_status(int id, const String& status) {
  String k = idKey(id);
  Serial.printf("[NVS] SET  | %s → %s\n", k.c_str(), status.c_str());
  prefs.putString(k.c_str(), status);
}

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
  http.setTimeout(10000);
  String url = String(apiEndpoint) + "?latest=true";
  http.begin(url);

  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();
    // ukuran dokumen disesuaikan; response ringkas diasumsikan
    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, payload);
    if (!err) {
      int count = doc["summary"]["orang_di_dalam"] | -1;
      http.end();
      return count;
    } else {
      if (verbose) {
        Serial.printf("[API] JSON parse error: %s\n", err.c_str());
      }
    }
  } else {
    if (verbose) {
      Serial.printf("[API] GET failed, HTTP code = %d\n", code);
    }
  }

  http.end();
  return -1;
}

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

bool sendToAPI(String sensorName) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[API] Gagal → WiFi tidak terhubung.");
    return false;
  }

  HTTPClient http;
  http.setTimeout(5000);
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload =
    "{\"sensor_name\":\"" + sensorName + "\",\"value\":1}";

  Serial.printf("[API] POST (%s) → %s\n", sensorName.c_str(), jsonPayload.c_str());

  int code = http.POST(jsonPayload);
  bool ok = (code == 200 || code == 201);
  http.end();

  if (ok) {
    Serial.println("[API] OK (200/201)");
    return true;
  }

  Serial.println("[API] Timeout → polling cek status...");

  int expectedAfter = peopleCount + (sensorName == "masuk" ? 1 : -1);

  for (int i = 0; i < 3; i++) {
    delay(800);
    int srvCount = getServerPeopleCount(false);
    if (srvCount == expectedAfter) {
      Serial.println("[API] Poll success.");
      return true;
    }
  }

  Serial.println("[API] Poll failed.");
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

  // parse JSON
  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    Serial.printf("[EVENT] JSON parse error: %s\n", err.c_str());
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid json\"}");
    return;
  }

  String event = doc["event"] | "";
  event.trim();

  // additional fields (optional)
  int payload_id = doc["id"] | -1;
  String payload_name = doc["name"] | "";
  bool is_new = (doc["new"] | 0) == 1;

  Serial.printf("[EVENT] Jenis event: %s | id=%d | name=%s | new=%d\n",
                event.c_str(), payload_id, payload_name.c_str(), is_new ? 1 : 0);

  // ---------- register_success ----------
  if (event == "register_success") {
    Serial.println("[EVENT] register_success → kirim ke Arduino");
    // beri notifikasi ke Arduino (buzzer / LED)
    Serial2.println("REGISTER_SUCCESS");
    server.send(200, "application/json", "{\"ok\":true}");
    return;
  }

  // ---------- REGISTER ----------
  if (event == "register" || event == "register_new") {
    // register: simpan id dengan status outside (jika belum ada)
    int id = payload_id;
    Serial.printf("[EVENT] REGISTER id=%d name=%s new=%d\n", id, payload_name.c_str(), is_new ? 1 : 0);

    if (id < 0) {
      server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing_id\"}");
      return;
    }

    if (!storage_has_id(id)) {
      storage_set_status(id, "outside");
      Serial.println("[REGISTER] ID baru, status → outside");
    } else {
      Serial.println("[REGISTER] ID sudah terdaftar, tidak diubah.");
    }

    // jika ada flag new, beri tahu Arduino supaya simpan nama/display
    if (is_new) {
      // kirim nama juga jika tersedia (Arduino bisa parse)
      if (payload_name.length() > 0) {
        Serial2.printf("REGISTER:%d:%s\n", id, payload_name.c_str());
      } else {
        Serial2.printf("REGISTER:%d\n", id);
      }
    } else {
      Serial2.printf("REGISTER:%d\n", id);
    }

    server.send(200, "application/json", "{\"ok\":true,\"status\":\"outside\"}");
    return;
  }

  // ---------- QUERY ----------
  if (event == "query") {
    int id = payload_id;
    if (id < 0) {
      server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing_id\"}");
      return;
    }
    String cur = storage_get_status(id);

    Serial.printf("[EVENT] QUERY id=%d → %s\n", id, cur.c_str());

    // return: { ok:true, status:"inside"/"outside", registered:bool }
    if (cur == "unknown") {
      server.send(200, "application/json",
                  "{\"ok\":true,\"status\":\"outside\",\"registered\":false}");
    } else {
      String resp = "{\"ok\":true,\"status\":\"" + cur + "\",\"registered\":true}";
      server.send(200, "application/json", resp);
    }
    return;
  }

  // ---------- MASUK / KELUAR / DENIED ----------
  if (event == "masuk_denied") {
    Serial.printf("[EVENT] MASUK_DENIED id=%d name=%s\n", payload_id, payload_name.c_str());
    // beri tahu Arduino untuk buzzer gagal / lampu merah
    Serial2.println("BUZZER_FAIL_MASUK");
    server.send(200, "application/json", "{\"ok\":true}");
    return;
  }

  if (event == "keluar_denied" || event == "masuk_denied") {
    // already handled masuk_denied above; keep for kelur_denied mapping
  }

  if (event == "keluar_denied") {
    Serial.printf("[EVENT] KELUAR_DENIED id=%d name=%s\n", payload_id, payload_name.c_str());
    Serial2.println("BUZZER_FAIL_KELUAR");
    server.send(200, "application/json", "{\"ok\":true}");
    return;
  }

  if (event != "masuk" && event != "keluar") {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"unsupported event\"}");
    return;
  }

  // From here: event == masuk or keluar
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

      server.send(200, "application/json",
                  "{\"ok\":true,\"action\":\"open\",\"peopleCount\":" + String(peopleCount) + "}");
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

        server.send(200, "application/json",
                    "{\"ok\":true,\"action\":\"close\",\"peopleCount\":" + String(peopleCount) + "}");
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
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  prefs.begin("faceStatus", false);

  Serial.println("\n===== STARTING ESP32 SYSTEM =====");
  Serial.println("[WiFi] Connecting...");

  WiFi.begin(ssid, password);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] Connected! IP: %s\n",
                  WiFi.localIP().toString().c_str());

    Serial2.println("BUZZER_READY");  // Tetap dikirim ke Arduino
    fetchPeopleCountFromAPI();
  } else {
    Serial.println("[WiFi] Gagal connect.");
    peopleCount = 0;
    Serial2.println("PEOPLE_IN:0");
  }

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
  server.handleClient();

  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();

    if (data.length() == 0) return;

    Serial.printf("[UART] Dari Arduino: %s\n", data.c_str());

    if (!processingEvent) {
      processingEvent = true;

      // --- ARDUINO minta MASUK ---
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

      // --- ARDUINO minta KELUAR ---
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

      // --- tambahan: Arduino kirim ID register manual ---
      else if (data.startsWith("MANUAL_REGISTER:")) {
        // format: MANUAL_REGISTER:<id>:<name optional>
        // contoh: MANUAL_REGISTER:12:John
        int idx1 = data.indexOf(':');
        int idx2 = data.indexOf(':', idx1 + 1);
        if (idx1 > 0 && idx2 > idx1) {
          String idStr = data.substring(idx1 + 1, idx2);
          String name = data.substring(idx2 + 1);
          int id = idStr.toInt();
          storage_set_status(id, "outside");
          Serial.printf("[MANUAL_REGISTER] id=%d name=%s\n", id, name.c_str());
          Serial2.println("REGISTER_DONE");
        } else if (idx1 > 0) {
          String idStr = data.substring(idx1 + 1);
          int id = idStr.toInt();
          storage_set_status(id, "outside");
          Serial.printf("[MANUAL_REGISTER] id=%d\n", id);
          Serial2.println("REGISTER_DONE");
        } else {
          Serial.println("[MANUAL_REGISTER] format salah.");
          Serial2.println("REGISTER_FAIL");
        }
      }

      processingEvent = false;
    }
  }

  delay(20);
}
