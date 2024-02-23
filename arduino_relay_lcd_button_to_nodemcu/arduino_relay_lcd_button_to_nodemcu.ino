#define BUTTON_PIN 3
#define BUZZER_PIN 2

int melody[] = {262, 294, 330, 349, 392, 440, 494, 523}; // Contoh melodi
int noteDurations[] = {200, 200, 200, 200, 200, 200, 200, 200}; // Durasi masing-masing not

int volume = 100; // Inisialisasi volume (0 - 255)
int buttonState = HIGH; // Inisialisasi status tombol
int lastButtonState = HIGH; // Inisialisasi status tombol sebelumnya
unsigned long lastDebounceTime = 0; // Inisialisasi waktu penundaan untuk bouncing
unsigned long debounceDelay = 50; // Waktu penundaan untuk bouncing (millisecond)

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Atur pin tombol sebagai input dengan resistor pull-up internal
  pinMode(BUZZER_PIN, OUTPUT); // Atur pin buzzer sebagai output
}

void loop() {
  int reading = digitalRead(BUTTON_PIN); // Baca status tombol
  if (reading != lastButtonState) { // Periksa perubahan status tombol
    lastDebounceTime = millis(); // Perbarui waktu bouncing
  }

  if (millis() - lastDebounceTime > debounceDelay) { // Pastikan bouncing sudah selesai
    if (reading != buttonState) { // Periksa jika status tombol berubah
      buttonState = reading; // Perbarui status tombol
      if (buttonState == LOW) { // Jika tombol ditekan
        volume += 10; // Tambahkan volume
        if (volume > 255) volume = 255; // Batasi volume maksimal
      }
    }
  }

  lastButtonState = reading; // Perbarui status tombol terakhir

  playMelody(); // Panggil fungsi untuk memainkan melodi dengan volume yang diatur
}

void playMelody() {
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    playTone(melody[i], noteDurations[i], volume); // Panggil fungsi untuk memainkan satu not dengan volume yang diatur
    delay(10); // Tambahkan jeda kecil antara setiap not
  }
}

void playTone(int tone, int duration, int volume) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER_PIN, HIGH); // Nyalakan buzzer
    delayMicroseconds(tone * volume / 255); // Atur volume dengan PWM
    digitalWrite(BUZZER_PIN, LOW); // Matikan buzzer
    delayMicroseconds(tone * volume / 255); // Atur volume dengan PWM
  }
}
