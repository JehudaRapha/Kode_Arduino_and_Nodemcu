#include <WiFi.h>
#include <HTTPClient.h>

// === Pin Sensor Ultrasonic ===
const int trigPinIn = 26;   // GPIO26 (Trig masuk)
const int echoPinIn = 33;   // GPIO33 (Echo masuk)
const int trigPinOut = 25;  // GPIO25 (Trig keluar)
const int echoPinOut = 32;  // GPIO32 (Echo keluar)

// === Pin Buzzer ===
const int buzzerPin = 27;   // GPIO27 (Buzzer)

// === Variabel global ===
unsigned long durationIn, durationOut; 
int distanceIn, distanceOut;

// Counter orang
int peopleCount = 0;
const int maxPeople = 5;    // batas maksimal orang

// Flag deteksi sebelumnya
bool prevInDetect = false;
bool prevOutDetect = false;

// === WiFi & API ===
const char* ssid = "SatuConnect-Event";
const char* password = "univ11univ";
const char* apiEndpoint = "http://10.103.48.118:8000/api/sensors"; // endpoint untuk update jumlah
bool wifiConnected = false;

void setup() {
  Serial.begin(115200);

  pinMode(trigPinIn, OUTPUT);
  pinMode(echoPinIn, INPUT);

  pinMode(trigPinOut, OUTPUT);
  pinMode(echoPinOut, INPUT);

  pinMode(buzzerPin, OUTPUT);

  Serial.println("Sistem siap.");

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
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
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("✗ Gagal koneksi WiFi, mode lokal aktif");
  }
}

void loop() {
  // --- Baca sensor Ultrasonic MASUK ---
  digitalWrite(trigPinIn, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinIn, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinIn, LOW);

  durationIn = pulseIn(echoPinIn, HIGH, 30000);
  distanceIn = durationIn * 0.034 / 2;

  bool inDetect = (distanceIn > 0 && distanceIn < 50);

  // --- Baca sensor Ultrasonic KELUAR ---
  digitalWrite(trigPinOut, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinOut, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinOut, LOW);

  durationOut = pulseIn(echoPinOut, HIGH, 30000);
  distanceOut = durationOut * 0.034 / 2;

  bool outDetect = (distanceOut > 0 && distanceOut < 10);

  // --- Orang MASUK ---
  if (inDetect && !prevInDetect) {
    if (peopleCount < maxPeople) { // batas maksimal
      peopleCount++;
      Serial.print("Orang MASUK. Total orang: ");
      Serial.println(peopleCount);

      // Buzzer nyala 0.1 detik
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);

      sendCountToAPI();
    } else {
      Serial.println("Ruangan sudah penuh, tidak bisa menambah orang.");
      // opsional: buzzer khusus atau tanda lain
      digitalWrite(buzzerPin, HIGH);
      delay(2000);
      digitalWrite(buzzerPin, LOW);

    }
  }

  // --- Orang KELUAR ---
  if (outDetect && !prevOutDetect) {
    if (peopleCount > 0) {
      peopleCount--;
      Serial.print("Orang KELUAR. Total orang: ");
      Serial.println(peopleCount);

      // Buzzer nyala 0.1 detik
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);

      sendCountToAPI();
    }
  }

  prevInDetect = inDetect;
  prevOutDetect = outDetect;

  delay(100);
}

// === Fungsi untuk mengirim jumlah orang ke API/database ===
void sendCountToAPI() {
  if (!wifiConnected) return; // jika WiFi tidak terhubung, skip

  HTTPClient http;
  http.begin(apiEndpoint);
  http.addHeader("Content-Type", "application/json");

  // JSON payload: selalu kirim {"count": X}
  String jsonPayload = "{\"sensor_name\":\"jumlah\",\"value\":" + String(peopleCount) + "}";

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    Serial.print("Update API berhasil, HTTP code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Gagal update API, error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}
