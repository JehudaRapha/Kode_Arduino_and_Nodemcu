// ESP32
#define RXD1 4  // Pin RX (sesuai dengan pin GPIO ESP32)
#define TXD1 5  // Pin TX (sesuai dengan pin GPIO ESP32)

void setup() {
  // Menginisialisasi Serial Monitor untuk debugging
  Serial.begin(115200);

  // Menginisialisasi Serial1 (dengan pin RX dan TX yang kita tetapkan) pada baud rate 9600
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
}

void loop() {
  // Mengirimkan perintah untuk menghidupkan relay ke Arduino
  Serial1.println("TURN_ON_RELAY");
  Serial.println("Mengirim TURN_ON_RELAY");

  // Tunggu 5 detik, kemudian kirim perintah untuk mematikan relay
  delay(5000);

  // Mengirimkan perintah untuk mematikan relay ke Arduino
  Serial1.println("TURN_OFF_RELAY");
  Serial.println("Mengirim TURN_OFF_RELAY");

  // Tunggu 5 detik sebelum mengulangi siklus
  delay(5000);
}
