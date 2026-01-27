#include <SoftwareSerial.h>
#include <Servo.h>

// ===== Pin sensor HW-201 =====
const int sensorOutPin = 2; // Sensor HW untuk ORANG KELUAR
const int sensorInPin = 4;  // Sensor HW untuk ORANG MASUK

// ===== Pin Buzzer =====
const int buzzerPin = 7;

// ===== Pin Servo =====
const int servoInPin = 9;   // Servo untuk orang masuk
const int servoOutPin = 6;  // Servo untuk orang keluar

// ===== SoftwareSerial ke ESP32 =====
const int txPin = 11;   // Arduino TX → ESP32 RX
const int rxPin = 12;   // Arduino RX → ESP32 TX
SoftwareSerial espSerial(rxPin, txPin);

// ===== Servo Objects =====
Servo servoIn;
Servo servoOut;

// ===== Variabel status dari ESP32 =====
bool isFull = false;  // Status apakah ruangan penuh

// ===== Variabel kontrol servo masuk =====
bool personDetectedIn = false;
unsigned long lastDetectionInTime = 0;
const unsigned long closeDelayIn = 1000; // waktu tunggu sebelum servo menutup (1 detik)

// ===== Variabel kontrol servo keluar =====
bool personDetectedOut = false;
unsigned long lastDetectionOutTime = 0;
const unsigned long closeDelayOut = 1000; // waktu tunggu sebelum servo menutup (1 detik)

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  pinMode(sensorOutPin, INPUT);
  pinMode(sensorInPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  servoIn.attach(servoInPin);
  servoOut.attach(servoOutPin);

  servoIn.write(0);
  servoOut.write(0);
  digitalWrite(buzzerPin, LOW);

  Serial.println("Arduino siap dengan 2 sensor HW-201 (masuk & keluar).");
}

void loop() {
  // ===== Baca Sensor HW untuk orang masuk =====
  int sensorInRaw = digitalRead(sensorInPin);
  int sensorInValue = (sensorInRaw == LOW) ? 1 : 0;  // LOW = terdeteksi

  // ===== Logika Orang Masuk =====
  if (sensorInValue == 1) {
    if (!personDetectedIn) {
      personDetectedIn = true;
      lastDetectionInTime = millis();

      if (!isFull) {
        espSerial.println("U1=1");
        Serial.println("👤 Sensor MASUK mendeteksi orang → servo masuk buka");
        beepBuzzer(100);
        servoIn.write(90); // buka servo
      } else {
        Serial.println("❌ Ruangan penuh, servo masuk tidak dibuka!");
        beepBuzzer(300);
      }
    } else {
      lastDetectionInTime = millis(); // perbarui waktu jika orang masih di depan sensor
    }
  } else {
    if (personDetectedIn && (millis() - lastDetectionInTime > closeDelayIn)) {
      personDetectedIn = false;

      if (!isFull) {
        Serial.println("✅ Orang sudah melewati sensor MASUK → servo menutup.");
        servoIn.write(0);
      }
    }
  }

  // ===== Baca Sensor HW untuk orang keluar =====
  int sensorOutRaw = digitalRead(sensorOutPin);
  int sensorOutValue = (sensorOutRaw == LOW) ? 1 : 0; // LOW = terdeteksi

  // ===== Logika Orang Keluar =====
  if (sensorOutValue == 1) {
    if (!personDetectedOut) {
      personDetectedOut = true;
      lastDetectionOutTime = millis();

      espSerial.println("HW1=1");
      Serial.println("🚪 Sensor KELUAR mendeteksi orang → servo keluar buka");
      beepBuzzer(100);
      servoOut.write(90); // buka servo
    } else {
      lastDetectionOutTime = millis(); // update waktu selama orang masih di depan sensor
    }
  } else {
    if (personDetectedOut && (millis() - lastDetectionOutTime > closeDelayOut)) {
      personDetectedOut = false;
      Serial.println("✅ Orang sudah melewati sensor KELUAR → servo menutup.");
      servoOut.write(0);
    }
  }

  // ===== Terima data dari ESP32 =====
  if (espSerial.available()) {
    String msg = espSerial.readStringUntil('\n');
    msg.trim();

    if (msg == "FULL=1") {
      isFull = true;
      Serial.println("🚨 Ruangan penuh! Servo masuk dikunci & buzzer bunyi 1 detik.");
      beepBuzzer(1000);
      servoIn.write(0);
    } 
    else if (msg == "FULL=0") {
      isFull = false;
      Serial.println("✅ Ruangan tidak penuh lagi. Servo masuk aktif kembali.");
    }
  }

  delay(50);
}

// ===== Fungsi buzzer =====
void beepBuzzer(int durationMs) {
  digitalWrite(buzzerPin, HIGH);
  delay(durationMs);
  digitalWrite(buzzerPin, LOW);
}
