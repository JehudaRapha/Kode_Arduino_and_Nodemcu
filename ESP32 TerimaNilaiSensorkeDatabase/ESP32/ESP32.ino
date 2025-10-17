// Pin sensor Ultrasonic
const int trigPin = 26;   // GPIO26 (Trig)
const int echoPin = 33;   // GPIO33 (Echo)

// Pin sensor HW-201 (infrared obstacle avoidance)
const int hw201Pin = 25;  // GPIO25 (HW-201 DO)

// Pin Buzzer
const int buzzerPin = 27; // GPIO27 (Buzzer)

// Variabel jarak
unsigned long duration;
int distance;

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(hw201Pin, INPUT);
}

void loop() {
  // --- Baca sensor Ultrasonic ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  distance = duration * 0.034 / 2;

  bool ultrasonicDetect = (distance > 0 && distance < 50);

  // --- Baca sensor HW-201 ---
  bool hw201Detect = digitalRead(hw201Pin) == LOW; 
  // Catatan: beberapa modul output LOW saat ada objek.
  // Kalau kebalik, ubah jadi == HIGH

  // --- Logika gabungan ---
  if (ultrasonicDetect || hw201Detect) {
    if (ultrasonicDetect) {
      Serial.print("Ultrasonic: Objek < 50 cm, jarak = ");
      Serial.print(distance);
      Serial.println(" cm");
    }
    if (hw201Detect) {
      Serial.println("HW-201: Objek terdeteksi!");
    }
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
    Serial.println("Tidak ada deteksi.");
  }

  delay(200);
}
