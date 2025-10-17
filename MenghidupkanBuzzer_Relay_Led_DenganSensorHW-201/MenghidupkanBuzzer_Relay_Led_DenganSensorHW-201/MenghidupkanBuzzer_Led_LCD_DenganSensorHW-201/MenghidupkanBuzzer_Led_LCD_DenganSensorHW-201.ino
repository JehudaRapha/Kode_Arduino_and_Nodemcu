#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>  // untuk komunikasi ESP

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = 2;
const int buzzerPin = 3;  
const int ledPin    = 4;

// SoftwareSerial untuk komunikasi ESP (RX Arduino ke TX ESP, TX Arduino ke RX ESP opsional)
SoftwareSerial espSerial(5, 6); 

// Status sensor & animasi
String line1 = "Welcome";
String line2 = "SATU UNIVERSITY";
unsigned long lastAnim = 0;
unsigned long animInterval = 10000; 
unsigned long lastCharTime = 0;
int animStep = 0;
bool animRunning = false;
bool sensorActive = false; 

bool ledFromESP = false;     // status LED dari ESP
bool buzzerFromESP = false;  // status Buzzer dari ESP

// ===== Fungsi animasi tanpa delay =====
void startTypingAnimation() {
  lcd.clear();
  animStep = 0;
  animRunning = true;
  lastCharTime = millis();
}

void updateTypingAnimation() {
  if (!animRunning) return;
  unsigned long now = millis();

  if (animStep < line1.length()) {
    if (now - lastCharTime >= 200) {
      int startCol = (16 - line1.length()) / 2;
      lcd.setCursor(startCol + animStep, 0);
      lcd.print(line1[animStep]);
      animStep++;
      lastCharTime = now;
    }
    return;
  }

  if (animStep < line1.length() + line2.length()) {
    if (now - lastCharTime >= 150) {
      int i = animStep - line1.length();
      int startCol = (16 - line2.length()) / 2;
      lcd.setCursor(startCol + i, 1);
      lcd.print(line2[i]);
      animStep++;
      lastCharTime = now;
    }
    return;
  }

  animRunning = false;
}

// ===== Setup =====
void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);       // Debugging
  espSerial.begin(9600);    // Komunikasi ESP

  lcd.begin();
  lcd.backlight();

  startTypingAnimation();
  lastAnim = millis();
}

// ===== Loop utama =====
void loop() {
  // ==== 1. Baca data dari ESP ====
  if (espSerial.available()) {
    String data = espSerial.readStringUntil('\n');
    data.trim();

    // LED dari ESP
    if (data == "LED_ON") ledFromESP = true;
    else if (data == "LED_OFF") ledFromESP = false;

    // Buzzer dari ESP
    if (data == "BUZZER_ON") buzzerFromESP = true;
    else if (data == "BUZZER_OFF") buzzerFromESP = false;
  }

  // ==== 2. Sensor ====
  int sensorValue = digitalRead(sensorPin);

  // ==== LED ====
  if (sensorValue == LOW || ledFromESP) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  // ==== Buzzer ====
  if (sensorValue == LOW) {
    analogWrite(buzzerPin, 60); // Buzzer sensor
  } else if (buzzerFromESP) {
    analogWrite(buzzerPin, 60); // Buzzer dari Blynk/ESP
  } else {
    analogWrite(buzzerPin, 0);  // mati
  }

  // ==== 3. Animasi LCD ====
  if (sensorValue == LOW) {
    if (!sensorActive) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Status Sensor:");
      lcd.setCursor(0, 1);
      lcd.print("Objek Terdeteksi");
      sensorActive = true;
    }
  } else {
    if (sensorActive) {
      startTypingAnimation();
      lastAnim = millis();
      sensorActive = false;
    }

    updateTypingAnimation();

    unsigned long now = millis();
    if (now - lastAnim >= animInterval) {
      startTypingAnimation();
      lastAnim = now;
    }
  }
}
