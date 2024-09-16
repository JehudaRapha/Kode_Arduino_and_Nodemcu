// Arduino
#define RELAY_PIN 3 // Pin untuk relay

void setup() {
  // Menginisialisasi Serial komunikasi di pin TX (Pin 1) dan RX (Pin 0) pada baud rate 9600
  Serial.begin(9600);

  // Mengatur pin relay sebagai OUTPUT
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Pastikan relay dalam keadaan mati saat memulai
}

void loop() {
  // Cek apakah ada data yang masuk dari ESP32
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    
    // Debugging: Tampilkan perintah yang diterima
    Serial.print("Command received: ");
    Serial.println(command);
    command.trim();  // Menghapus spasi di awal dan akhir string
    
    // Periksa apakah perintah adalah untuk menghidupkan relay
    if (command == "TURN_ON_RELAY") {
      digitalWrite(RELAY_PIN, LOW); // Menghidupkan relay
      Serial.println("Relay turned on");
    } 
    // Perintah untuk mematikan relay
    else if (command == "TURN_OFF_RELAY") {
      digitalWrite(RELAY_PIN, HIGH); // Mematikan relay
      Serial.println("Relay turned off");
    } else {
      Serial.println("Unknown command");
    }
  }
}
