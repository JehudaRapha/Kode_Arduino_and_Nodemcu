#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = 2;
const int relayPin = 3;

float calibrationFactor = 7.5;

volatile byte pulses;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
unsigned long relayTimer = 0;
unsigned long lastFlowTime = 0;

bool relayState = false;
unsigned long relayOnDuration = 5000;  // 5 seconds

void setup() {
  Serial.begin(115200);
  initLCD();   // Inisialisasi LCD
  initPins();  // Inisialisasi pin sensor dan relay
}

void loop() {
  unsigned long currentMillis = millis();

  if ((millis() - oldTime) > 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));
    calculateFlowRate();  // Menghitung laju aliran air
    updateFlow();         // Memperbarui total air yang telah mengalir
    controlRelay();       // Mengontrol relay berdasarkan kondisi aliran air
    displayDataOnLCD();   // Menampilkan data pada LCD
    resetPulseCounter();  // Reset hitungan pulsa
    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  }

  openRelayEveryHour();  // Buka relay setiap 1 jam
}

void initLCD() {
  lcd.init();       // Inisialisasi LCD
  lcd.backlight();  // Aktifkan backlight LCD
  lcd.setCursor(0, 0);
  lcd.print("Air:");
}

void initPins() {
  pinMode(sensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(sensorPin, HIGH);
  pulses = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = millis();
  relayTimer = millis();
  digitalWrite(relayPin, HIGH);  // Matikan relay saat inisialisasi
}

void calculateFlowRate() {
  flowRate = ((1000.0 / (millis() - oldTime)) * pulses) / calibrationFactor;
  oldTime = millis();
  flowMilliLitres = (flowRate / 60) * 1000;
}

void updateFlow() {
  if (flowMilliLitres > 0) {
    totalMilliLitres += flowMilliLitres;
    lastFlowTime = millis();
  }
}

void controlRelay() {
  unsigned long currentMillis = millis();
  unsigned long delayBeforeReset = 5000;  // Delay sebelum reset totalMilliLitres (dalam milidetik), contoh: 5000 ms = 5 detik

  if (totalMilliLitres >= 1000) {
    if (relayState) {
      relayState = false;
      digitalWrite(relayPin, HIGH); // Matikan relay
      relayTimer = currentMillis;  // Mulai hitung delay untuk reset totalMilliLitres
    }
    
    // Reset totalMilliLitres menjadi 0 setelah delay selesai
    if (!relayState && currentMillis - relayTimer >= delayBeforeReset) {
      totalMilliLitres = 0;
      lcd.clear();
    }
  } else {
    if (flowMilliLitres > 0) {
      relayState = true;
      digitalWrite(relayPin, LOW);
      relayTimer = millis();
    } else {
      if (relayState && (currentMillis - relayTimer >= relayOnDuration)) {
        relayState = false;
        digitalWrite(relayPin, HIGH);
      }
    }
  }
}

void displayDataOnLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Air: ");
  lcd.print(totalMilliLitres);
  lcd.print(" ml    ");

  if (totalMilliLitres >= 1000) {
    lcd.setCursor(0, 1);
    lcd.print("Air telah penuh");
  }
}

void resetPulseCounter() {
  pulses = 0;
}

void pulseCounter() {
  pulses++;
}

void openRelayEveryHour() {
  unsigned long currentMillis = millis();
  if ((currentMillis - relayTimer) >= 60000) {
    relayTimer = currentMillis;
    if (totalMilliLitres < 1000) {
      digitalWrite(relayPin, LOW);
      relayState = true;
      lcd.setCursor(0, 1);
      lcd.print("Air Sdng Mngalir");
      delay(10000);  // Tunda 10 detik
      digitalWrite(relayPin, HIGH);
      relayState = false;
      lcd.clear();
    }
  }
}
