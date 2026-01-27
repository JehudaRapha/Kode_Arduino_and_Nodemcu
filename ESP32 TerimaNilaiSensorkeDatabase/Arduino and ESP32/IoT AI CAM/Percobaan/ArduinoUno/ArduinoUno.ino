#include <SoftwareSerial.h>
#include <Servo.h>

// ===== Pin Komponen =====
const int buzzerPin = 7;
const int servoInPin = 9;
const int servoOutPin = 6;

// ===== Servo Object =====
Servo servoIn;
Servo servoOut;

// ===== Komunikasi Serial dengan ESP32 =====
const int txPin = 11;  // Arduino TX → ESP32 RX
const int rxPin = 12;  // Arduino RX → ESP32 TX
SoftwareSerial espSerial(rxPin, txPin);

// ===== Variabel =====
int peopleCount = 0;

// ===== Buzzer Functions =====
void makeBeep(int duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(buzzerPin, LOW);
}

// Beep saat berhasil masuk/keluar
void buzzerSuccess() {
  for (int i = 0; i < 3; i++) {
    makeBeep(100);
    delay(100);
  }
}

// Beep saat gagal / API fail
void buzzerFail() {
  makeBeep(500);
}

// Beep saat register sukses
void buzzerRegister() {
  for (int i = 0; i < 4; i++) {
    makeBeep(150);
    delay(100);
  }
}

// Beep saat sistem siap
void buzzerReady() {
  for (int i = 0; i < 3; i++) {
    makeBeep(150);
    delay(200);
  }
}

// ===== Setup =====
void setup() {
  Serial.begin(9600);       // Debug di Serial Monitor
  espSerial.begin(9600);    // Serial komunikasi dengan ESP32

  pinMode(buzzerPin, OUTPUT);
  servoIn.attach(servoInPin);
  servoOut.attach(servoOutPin);

  // Servo default posisi tertutup
  servoIn.write(180);
  servoOut.write(180);

  // Sistem siap
  buzzerReady();
  Serial.println("[SYSTEM] Arduino Ready.");
  espSerial.println("ARDUINO_READY");  // Beri tahu ESP32
}

// ===== Loop =====
void loop() {
  if (espSerial.available()) {
    String cmd = espSerial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) return;

    Serial.print("[ESP CMD] ");
    Serial.println(cmd);

    // ===== UPDATE PEOPLE COUNT =====
    if (cmd.startsWith("PEOPLE_IN:")) {
      String countStr = cmd.substring(10);
      int count = countStr.toInt();
      if (count >= 0) {
        peopleCount = count;
        Serial.print("[INFO] People in room updated: ");
        Serial.println(peopleCount);
      }
    }

    // ===== ORANG MASUK =====
    else if (cmd == "API_OK_MASUK") {
      peopleCount++;
      Serial.print("[EVENT] Masuk ✔, Total: ");
      Serial.println(peopleCount);
      buzzerSuccess();

      servoIn.write(90);  // Servo pintu masuk terbuka
      delay(2000);
      servoIn.write(180); // Kembali tertutup
    }

    // ===== ORANG KELUAR =====
    else if (cmd == "API_OK_KELUAR") {
      if (peopleCount > 0) peopleCount--;
      Serial.print("[EVENT] Keluar ✔, Total: ");
      Serial.println(peopleCount);
      buzzerSuccess();

      servoOut.write(90);  // Servo pintu keluar terbuka
      delay(2000);
      servoOut.write(180); // Kembali tertutup
    }

    // ===== REGISTER SUKSES =====
    else if (cmd == "REGISTER_SUCCESS") {
      Serial.println("[EVENT] Register wajah ✔");
      buzzerRegister();
    }

    // ===== API FAIL =====
    else if (cmd == "API_FAIL") {
      Serial.println("[ERROR] API Fail ❌, coba lagi");
      buzzerFail();
    }

    // ===== READY =====
    else if (cmd == "BUZZER_READY") {
      Serial.println("[SYSTEM] ESP32 Ready, menunggu user...");
      buzzerReady();
    }

    // ===== UNKNOWN COMMAND =====
    else {
      Serial.print("[WARN] Unknown command: ");
      Serial.println(cmd);
    }
  }

  delay(20);
}
