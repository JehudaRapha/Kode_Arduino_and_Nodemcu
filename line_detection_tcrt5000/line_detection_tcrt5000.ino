// Define the pin connections for TCRT5000
const int TCRT5000_PIN = A0;  // Analog pin for TCRT5000 sensor

void setup() {
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop() {
  // Read the analog value from the TCRT5000 sensor untuk sensor
  int sensorValue = analogRead(TCRT5000_PIN);

  // Print the sensor value to the serial monitor
  Serial.println(sensorValue);

  // Adjust the threshold value according to your environment
  int threshold = 500;

  // Check if the sensor value is above the threshold
  if (sensorValue > threshold) {
    // Detected line
    Serial.println("Line detected!");
  } else {
    // No line detected
    Serial.println("No line detected");
  }

  delay(100);  // Add a small delay for stability
}
