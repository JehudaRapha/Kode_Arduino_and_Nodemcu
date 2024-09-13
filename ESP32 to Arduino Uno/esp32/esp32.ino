// ESP32
#include <SoftwareSerial.h>
SoftwareSerial myserial(15, 2); // RX TX

void setup() {
  // Memulai komunikasi serial pada baud rate 115200 untuk monitor serial
  Serial.begin(115200);
  
  // Memulai SoftwareSerial pada baud rate 115200 untuk komunikasi dengan Arduino Uno
  myserial.begin(115200);
  Serial.println("ESP32 Ready to send data");
}

void loop() {
  // Mengirim pesan ke Arduino Uno setiap 2 detik
  myserial.println("Hello from ESP32!");
  Serial.println("Sent: Hello from ESP32!");
  
  // Tunggu 2 detik sebelum mengirim pesan lagi
  delay(2000);
}
