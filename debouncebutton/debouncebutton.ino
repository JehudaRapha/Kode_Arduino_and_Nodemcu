const int tombol = 3;
const int ledPin = 13;  // Pin untuk LED (sesuaikan dengan kebutuhan Anda)
int nilai;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool ledState = false;  // Menyimpan status LED

void setup() {
  Serial.begin(9600);
  pinMode(tombol, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int reading = digitalRead(tombol);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != nilai) {
      nilai = reading;

      // Mengontrol LED
      if (nilai == HIGH) {
        // Tombol ditekan
        ledState = !ledState;                         // Toggle status LED
        digitalWrite(ledPin, ledState ? HIGH : LOW);  // Atur LED sesuai dengan status

        if (ledState == HIGH) {
          Serial.println("led on");
        }else{
          Serial.println("led off");

        }
      }
    }
  }

  lastButtonState = reading;
}
