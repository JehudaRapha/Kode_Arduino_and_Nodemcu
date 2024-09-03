#define MAGNETIC_SENSOR_PIN D3

void setup() {
  // Inisialisasi serial monitor untuk debugging
  Serial.begin(9600);

  // Set pin D3 sebagai input untuk membaca status sensor
  pinMode(MAGNETIC_SENSOR_PIN, INPUT);
}

void loop() {
  // Membaca status dari sensor magnetik
  int sensorState = digitalRead(MAGNETIC_SENSOR_PIN);
  Serial.println(sensorState);
  delay(500); 
}
