const int sensorPin = 2;
const int relayPin = 3;

float calibrationFactor = 12.0;

volatile byte pulses;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
unsigned long relayTimer = 0;
unsigned long lastFlowTime = 0;

bool waterFullNotified = false;
bool relayState = false;
unsigned long relayOnDuration = 3000;  // 5 seconds

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(sensorPin, HIGH);
  pulses = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = millis();
  relayTimer = millis();

  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  digitalWrite(relayPin, HIGH);
}

void loop() {
  unsigned long currentMillis = millis();
  if ((millis() - oldTime) > 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));
    flowRate = ((1000.0 / (millis() - oldTime)) * pulses) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;

    if (flowMilliLitres > 0) {
      totalMilliLitres += flowMilliLitres;
      lastFlowTime = millis();
    }

    if ((currentMillis - relayTimer) > 3600000) {
      relayTimer = currentMillis;
      relayState = true;
      digitalWrite(relayPin, LOW);
      Serial.println("Relay HIDUP (ON) every hour");
    }

    // Check if the relay has been on for the specified duration, turn it off
    if (relayState && (currentMillis - relayTimer > relayOnDuration)) {
      relayState = false;
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay OFF after 5 seconds");
    }

    if (relayState && totalMilliLitres == 1000) {
      // Relay is ON, and totalMilliLitres reached 1000
      Serial.println(totalMilliLitres);
      relayState = false;
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay dimatikan setelah 1000 mililiter");
      totalMilliLitres = 0;  // Reset total water only if the relay was previously ON
      delay(1000);

    } else if (!relayState && totalMilliLitres >= 1000) {
      // Relay is OFF, and totalMilliLitres reached or exceeded 1000
      relayState = false;
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay dimatikan setelah 1000 mililiter");
      totalMilliLitres = 0;  // Reset total water when it reaches or exceeds 1000
      delay(1000);
    } else if ((millis() - lastFlowTime < 3600000) && lastFlowTime != 0 && totalMilliLitres >= 10 ) {
      // If less than 1 hour has passed since the last flow
      relayState = false;
      digitalWrite(relayPin, LOW);
      Serial.println("Relay HIDUP");
    } else if (!relayState && (millis() - lastFlowTime > 50)) {
      // Relay is OFF, and it's been more than 50 milliseconds since the last flow
      relayState = true;
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay OFF");
      if (lastFlowTime < 1000) {
        totalMilliLitres = lastFlowTime;
      } else if (totalMilliLitres >= 1000) {
        totalMilliLitres = 0;  // Reset total water when it reaches or exceeds 1000
      }
    }


    Serial.print("Total Air yang Telah Mengalir: ");
    Serial.print(totalMilliLitres);
    Serial.println(" mililiter");

    pulses = 0;
    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  }
}

void pulseCounter() {
  pulses++;
}
