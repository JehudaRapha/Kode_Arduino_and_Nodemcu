#define BUZZER_PIN 5

// Tentukan pin yang terhubung ke sensor TCRT5000
const int TCRT5000_PIN_LEFT = A0;  // Pin analog untuk sensor TCRT5000 kiri
const int TCRT5000_PIN_RIGHT = A1;  // Pin analog untuk sensor TCRT5000 kanan

void activateBuzzer(int duration) {
  analogWrite(BUZZER_PIN, 2);
  delay(duration);
  analogWrite(BUZZER_PIN, 0);
}

void setup() {
  Serial.begin(9600);  // Inisialisasi komunikasi serial untuk debugging
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Baca nilai analog dari sensor TCRT5000
  int sensorValueLeft = analogRead(TCRT5000_PIN_LEFT);
  int sensorValueRight = analogRead(TCRT5000_PIN_RIGHT);

  // Tampilkan nilai sensor di serial monitor
  Serial.print("Nilai sensor kiri: ");
  Serial.println(sensorValueLeft);
  Serial.print("Nilai sensor kanan: ");
  Serial.println(sensorValueRight);

  // Sesuaikan nilai ambang batas sesuai lingkungan Anda
  int thresholdLeft = 500;
  int thresholdRight = 500;

  // Periksa apakah sensor kiri mendeteksi garis
  if (sensorValueLeft > thresholdLeft) {
    Serial.println("Sensor kiri: Garis terdeteksi!");
    activateBuzzer(500);  // Aktifkan buzzer selama 500ms jika sensor kiri terdeteksi
  } else {
    // Matikan buzzer jika nilai sensor kiri lebih besar dari 600
    if (sensorValueLeft > 600) {
      analogWrite(BUZZER_PIN, 0);
      Serial.println("Garis tidak terdeteksi (kiri)");
    } else {
      Serial.println("Sensor kiri: Tidak ada garis terdeteksi");
    }
  }

  // Periksa apakah sensor kanan mendeteksi garis
  if (sensorValueRight > thresholdRight) {
    Serial.println("Sensor kanan: Garis terdeteksi!");
    activateBuzzer(500);  // Aktifkan buzzer selama 500ms jika sensor kanan terdeteksi
  } else {
    // Matikan buzzer jika nilai sensor kanan lebih besar dari 600
    if (sensorValueRight > 600) {
      analogWrite(BUZZER_PIN, 0);
      Serial.println("Garis tidak terdeteksi (kanan)");
    } else {
      Serial.println("Sensor kanan: Tidak ada garis terdeteksi");
    }
  }

  delay(1000);  // Tambahkan jeda kecil untuk kestabilan
}
