#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial myserial(8, 9); // RX TX

const int sensorPin = 2;
const int relayPin = 3;

float calibrationFactor = 11;

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
  Serial.begin(9600);
  myserial.begin(9600);
  initPins();
}

void loop() {
  unsigned long currentMillis = millis();
  receivedData();
  if ((millis() - oldTime) > 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));
    calculateFlowRate();  // Menghitung laju aliran air
    updateFlow();         // Memperbarui total air yang telah mengalir
    controlRelay();       // Mengontrol relay berdasarkan kondisi aliran air
    sendTotalMilliLitres(); // Mengirim totalMilliLitres ke NodeMCU
    resetPulseCounter();  // Reset hitungan pulsa
    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  }
  displayDataOnMonitor();
  openRelayEveryHour();  // Buka relay setiap 1 jam

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

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void controlRelay() {
  unsigned long currentMillis = millis();
  unsigned long delayBeforeReset = 5000;  // Delay sebelum reset totalMilliLitres (dalam milidetik), contoh: 5000 ms = 5 detik

  if (totalMilliLitres >= 20000) {
    if (relayState) {
      relayState = false;
      digitalWrite(relayPin, HIGH); // Matikan relay
      relayTimer = currentMillis;  // Mulai hitung delay untuk reset totalMilliLitres
    }

    // Reset totalMilliLitres menjadi 0 setelah delay selesai
    if (!relayState && currentMillis - relayTimer >= delayBeforeReset) {
      totalMilliLitres = 0;
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
void receivedData() {
  if (myserial.available() > 0) {
    String data = myserial.readStringUntil('\n');
    String state = getValue(data,'|',0);
    String value = getValue(data,'|',1);

    if(state == "state" && value == "1"){
      Serial.print("relay on");
      digitalWrite(relayPin, LOW);
    }else if(state == "state" && value == "0"){
      Serial.print("relay off");
      digitalWrite(relayPin, HIGH);
    }else{
      return;
    }
    Serial.print("data blynk : ");
    Serial.println(state);
  }
}
void displayDataOnMonitor() {
  Serial.print("Air: ");
  Serial.print(totalMilliLitres);
  Serial.println(" ml    ");
  delay(200);
  if (totalMilliLitres >= 20000) {
    Serial.print("Air telah penuh");
  }
}



void sendTotalMilliLitres() {
  myserial.print(totalMilliLitres);
  myserial.print("\n"); // Kirim newline untuk menandai akhir pesan
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
      Serial.print("Air Sdng Mngalir");
      delay(10000);  // Tunda 10 detik
      digitalWrite(relayPin, HIGH);
      relayState = false;
    }
  }
}
