#include <SoftwareSerial.h>
#include <Servo.h>

// ===== Pin Sensor HW-201 =====
const int sensorOutPin = 4; // Sensor HW untuk ORANG KELUAR
const int sensorInPin  = 5; // Sensor HW untuk ORANG MASUK

// ===== Pin Buzzer =====
const int buzzerPin = 13;

// ===== Pin Servo =====
const int servoInPin  = 9; // Servo untuk orang masuk
const int servoOutPin = 6; // Servo untuk orang keluar

// ===== Servo Object =====
Servo servoIn;
Servo servoOut;

// ===== Pin Serial Komunikasi dengan ESP32 =====
const int txPin = 11; // Arduino TX → ESP32 RX
const int rxPin = 12; // Arduino RX → ESP32 TX
SoftwareSerial espSerial(rxPin, txPin);

// ===== Variabel =====
bool personDetectedIn = false;
bool personDetectedOut = false;
int peopleCount = 0;
const int maxPeople = 15;
bool sensorInLocked = false; // lock sensor masuk
int peopleAtLock = 0;        // jumlah orang saat lock aktif

// ===== Beep Functions =====
void startupBeep() {
  for (int i = 0; i < 2; i++) { tone(buzzerPin, 800, 100); delay(200); }
  noTone(buzzerPin);
}

void beepShort() {
  tone(buzzerPin, 850, 100); delay(120);
  tone(buzzerPin, 950, 100); delay(100);
  noTone(buzzerPin);
}

void beepReady() {
  for (int i = 0; i < 3; i++) { tone(buzzerPin, 850, 100); delay(150); }
  noTone(buzzerPin);
}

void beepSuccess() {
  tone(buzzerPin, 900, 80); delay(150);
  tone(buzzerPin, 900, 80); delay(100);
  noTone(buzzerPin);
}

void beepFail() {
  tone(buzzerPin, 700, 200); delay(200);
  noTone(buzzerPin);
}

void beepFull() {
  for (int i = 0; i < 2; i++) { tone(buzzerPin, 600, 120); delay(150); }
  noTone(buzzerPin);
}

// ===== Fungsi Deteksi Stabil 2 Detik =====
bool isPersonStable(int pin) {
  unsigned long startTime = millis();
  while (digitalRead(pin) == LOW) {
    if (millis() - startTime >= 1000) {
      return true; // Terbaca stabil selama 1 detik penuh
    }
  }
  return false; // Lepas sebelum 2 detik → bukan orang
}

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  pinMode(sensorOutPin, INPUT);
  pinMode(sensorInPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  servoIn.attach(servoInPin);
  servoOut.attach(servoOutPin);
  // posisi awal servo berlawanan arah
  servoIn.write(180);
  servoOut.write(180);

  startupBeep();
  Serial.println("✅ Sistem siap. Sensor, Servo, dan Buzzer aktif (arah servo dibalik).");
  espSerial.println("✅ Arduino siap.");
}

void loop() {
  int sensorInState = digitalRead(sensorInPin);
  int sensorOutState = digitalRead(sensorOutPin);

  // ===== SENSOR MASUK =====
  if (sensorInState == LOW && !personDetectedIn) {
    Serial.println("👀 Deteksi awal pada sensor MASUK...");
    if (isPersonStable(sensorInPin)) {
      personDetectedIn = true;

      // ===== cek lock sensor masuk =====
      if (sensorInLocked) {
        if ((peopleAtLock - peopleCount) >= 5) {
          sensorInLocked = false; // unlock sensor
          Serial.println("✅ Sensor masuk UNLOCKED, sudah cukup orang keluar.");
        } else {
          beepFull();
          Serial.println("🚫 Sensor MASUK terkunci, harus keluar 5 orang dulu.");
          return; // jangan aktifkan servo
        }
      }

      if (peopleCount < maxPeople) {
        espSerial.println("REQ_MASUK");
        Serial.println("📤 Kirim permintaan MASUK ke ESP32...");
      } else {
        sensorInLocked = true; 
        peopleAtLock = peopleCount; 
        beepFull();
        Serial.println("🚫 Ruangan penuh! Sensor masuk terkunci.");
      }
    }
  }

  if (sensorInState == HIGH && personDetectedIn) {
    personDetectedIn = false;
    delay(200);
  }

  // ===== SENSOR KELUAR =====
  if (sensorOutState == LOW && !personDetectedOut) {
    Serial.println("👀 Deteksi awal pada sensor KELUAR...");
    if (isPersonStable(sensorOutPin)) {
      personDetectedOut = true;

      if (peopleCount > 0) {
        espSerial.println("REQ_KELUAR");
        Serial.println("📤 Kirim permintaan KELUAR ke ESP32...");
      } else {
        Serial.println("⚠️ Tidak ada orang untuk dikurangi.");
      }
    }
  }

  if (sensorOutState == HIGH && personDetectedOut) {
    personDetectedOut = false;
    delay(200);
  }

  // ===== RESPON DARI ESP32 =====
  if (espSerial.available()) {
    String cmd = espSerial.readStringUntil('\n');
    cmd.trim();

    // ===== Terima jumlah orang dari ESP32 =====
    if (cmd.startsWith("PEOPLE_IN:")) {
      String countStr = cmd.substring(10);
      int count = countStr.toInt();
      if (count >= 0) {
        peopleCount = count;
        Serial.print("✅ Jumlah orang di dalam ruangan diterima dari ESP32: ");
        Serial.println(peopleCount);
      }
    }

    // === API OK MASUK ===
    else if (cmd == "API_OK_MASUK") {
      peopleCount++;
      Serial.print("🟢 Orang MASUK. Total: ");
      Serial.println(peopleCount);
      beepSuccess();

      // servo masuk berlawanan arah → dari 180° ke 90°
      servoIn.write(90);
      Serial.println("🚪 Servo pintu MASUK terbuka (arah terbalik), menunggu orang lewat...");
      while (digitalRead(sensorInPin) == LOW) delay(100);
      delay(1000);
      servoIn.write(180);
      Serial.println("🚪 Servo pintu MASUK tertutup.");
    }

    // === API OK KELUAR ===
    else if (cmd == "API_OK_KELUAR") {
      if (peopleCount > 0) peopleCount--;
      Serial.print("🔵 Orang KELUAR. Total: ");
      Serial.println(peopleCount);
      beepSuccess();

      // servo keluar berlawanan arah → dari 180° ke 90°
      servoOut.write(90);
      Serial.println("🚪 Servo pintu KELUAR terbuka (arah terbalik), menunggu orang lewat...");
      while (digitalRead(sensorOutPin) == LOW) delay(100);
      delay(2000);
      servoOut.write(180);
      Serial.println("🚪 Servo pintu KELUAR tertutup.");
    }

    else if (cmd == "API_FAIL") {
      Serial.println("❌ Gagal kirim API. Hitungan tidak berubah.");
      beepFail();
    }

    else if (cmd == "BUZZER_READY") {
      beepReady();
    }
  }

  delay(50);
}
