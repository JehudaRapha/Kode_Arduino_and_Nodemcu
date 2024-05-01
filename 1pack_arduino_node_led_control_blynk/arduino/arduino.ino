const int ledPin = 5;
const int relayPin = 4;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  digitalWrite(relayPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  const int ledPin = 5;  // Pin untuk LED
const int relayPin = 4;  // Pin untuk relay

void setup() {
  pinMode(ledPin, OUTPUT);  // Atur pin LED sebagai output
  pinMode(relayPin, OUTPUT);  // Atur pin relay sebagai output

  // Matikan LED pada awalnya
  digitalWrite(ledPin, LOW);
  // Matikan relay pada awalnya (jika relay memerlukan logika inverted)
  digitalWrite(relayPin, LOW);
}

void loop() {
  int relayState = digitalRead(relayPin);  // Baca status relay

  // Jika relay mati (LOW), matikan LED
  if (relayState == LOW) {
    digitalWrite(ledPin, LOW);
  }
  // Jika relay hidup (HIGH), hidupkan LED
  else {
    digitalWrite(ledPin, HIGH);
  }
}

}
